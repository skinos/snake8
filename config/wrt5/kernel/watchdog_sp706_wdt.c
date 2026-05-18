// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for watchdog device controlled through GPIO-line
 *
 * Author: 2013, Alexander Shiyan <shc_work@mail.ru>
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/watchdog.h>

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
		"Watchdog cannot be stopped once started (default="
				__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

#define SOFT_TIMEOUT_MIN	1
#define SOFT_TIMEOUT_DEF	60

enum {
	HW_ALGO_TOGGLE,
	HW_ALGO_LEVEL,
};

struct sp706_wdt_priv {
	struct gpio_desc	*gpiod;
	bool			state;
	bool			always_running;
	unsigned int		hw_algo;
	struct watchdog_device	wdd;
//	unsigned long next_heartbeat;
	struct timer_list timer;
	unsigned long heartbeat;
	int hrcur_count;
	int hrmax_count;	
};

static inline void sp706_wdt_reset(struct sp706_wdt_priv * wdt)
{
	switch (wdt->hw_algo) {
	case HW_ALGO_TOGGLE:
		/* Toggle output pin */
		wdt->state = !wdt->state;
		gpiod_set_value_cansleep(wdt->gpiod, wdt->state);
		break;
	case HW_ALGO_LEVEL:
		/* Pulse */
		gpiod_set_value_cansleep(wdt->gpiod, 1);
		udelay(1);
		gpiod_set_value_cansleep(wdt->gpiod, 0);
		break;
	}
	return ;
}

static void sp706_wdt_disable(struct sp706_wdt_priv *wdt)
{
	/* Eternal ping */
	gpiod_set_value_cansleep(wdt->gpiod, 1);

	/* Put GPIO back to tristate */
	if (wdt->hw_algo == HW_ALGO_TOGGLE)
		gpiod_direction_input(wdt->gpiod);
	
	wdt->hrcur_count = 0;
}

static int sp706_wdt_ping(struct watchdog_device *wdd)
{
	struct sp706_wdt_priv *wdt = watchdog_get_drvdata(wdd);

	wdt->hrcur_count = wdt->hrmax_count;
	return 0;
}

static int sp706_wdt_start(struct watchdog_device *wdd)
{
	struct sp706_wdt_priv *wdt = watchdog_get_drvdata(wdd);

	//printk("gpio_wdt_start\n");
	wdt->state = 0;
	gpiod_direction_output(wdt->gpiod, wdt->state);

	set_bit(WDOG_HW_RUNNING, &wdd->status);

	sp706_wdt_reset(wdt);
	return sp706_wdt_ping(wdd);
}

static int sp706_wdt_stop(struct watchdog_device *wdd)
{
	struct sp706_wdt_priv *priv = watchdog_get_drvdata(wdd);

	if (!priv->always_running) {
		sp706_wdt_disable(priv);
	} else {
		set_bit(WDOG_HW_RUNNING, &wdd->status);
	}

	return 0;
}

static void sp706_wdt_handle(struct timer_list *t)
{
	struct sp706_wdt_priv * wdt = from_timer(wdt, t, timer);

	//if (!watchdog_active(&wdt->wdd)) {
		if (wdt->hrcur_count > 0) {
			wdt->hrcur_count -= 1;
			sp706_wdt_reset(wdt);
			//printk("===sp706 wdt reset %lld\n", ktime_get());
			mod_timer(&wdt->timer, jiffies + wdt->heartbeat);
			return;
		} 
	//} 
		
	pr_crit("SP706: I will reset your machine !\n");
}

static void sp706_wdt_timer_init(struct platform_device *pdev, struct sp706_wdt_priv *wdt)
{
	timer_setup(&wdt->timer, sp706_wdt_handle, 0);
	mod_timer(&wdt->timer, jiffies + wdt->heartbeat);
}

static const struct watchdog_info sp706_wdt_ident = {
	.options	= WDIOF_MAGICCLOSE | WDIOF_KEEPALIVEPING |
			  WDIOF_SETTIMEOUT,
	.identity	= "SP706 Watchdog",
};

static const struct watchdog_ops sp706_wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= sp706_wdt_start,
	.stop		= sp706_wdt_stop,
	.ping		= sp706_wdt_ping,
};

static int sp706_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct sp706_wdt_priv *priv;
	enum gpiod_flags gflags;
	unsigned int hw_margin;
	const char *algo;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_set_drvdata(pdev, priv);
	ret = device_property_read_string(dev, "hw_algo", &algo);
	if (ret)
		return ret;
	if (!strcmp(algo, "toggle")) {
		priv->hw_algo = HW_ALGO_TOGGLE;
		gflags = GPIOD_IN;
	} else if (!strcmp(algo, "level")) {
		priv->hw_algo = HW_ALGO_LEVEL;
		gflags = GPIOD_OUT_LOW;
	} else {
		return -EINVAL;
	}

	priv->gpiod = devm_gpiod_get(dev, NULL, gflags);
	if (IS_ERR(priv->gpiod))
		return PTR_ERR(priv->gpiod);

	ret = device_property_read_u32(dev, "hw_margin_ms", &hw_margin);
	if (ret)
		return ret;
	
	if (hw_margin < 100 || hw_margin > 1000)
		priv->heartbeat = msecs_to_jiffies(500);
	else
		priv->heartbeat = msecs_to_jiffies(hw_margin);

	ret = device_property_read_u32(dev, "hw_timer_times", &hw_margin);
	if (ret) {
		priv->hrmax_count = 5;
	}  

	if (hw_margin < 2 || hw_margin > 30)
		priv->hrmax_count = 5; //5 times
	else
		priv->hrmax_count = hw_margin;
		
	priv->always_running = 1;

	watchdog_set_drvdata(&priv->wdd, priv);

	priv->wdd.info		= &sp706_wdt_ident;
	priv->wdd.ops		= &sp706_wdt_ops;
	priv->wdd.min_timeout	= SOFT_TIMEOUT_MIN;
	priv->wdd.max_hw_heartbeat_ms = hw_margin;
	priv->wdd.parent	= dev;
	priv->wdd.timeout	= SOFT_TIMEOUT_DEF;

	watchdog_init_timeout(&priv->wdd, 0, dev);
	watchdog_set_nowayout(&priv->wdd, nowayout);

	watchdog_stop_on_reboot(&priv->wdd);

	sp706_wdt_start(&priv->wdd);
	sp706_wdt_timer_init(pdev, priv);

	return devm_watchdog_register_device(dev, &priv->wdd);
}

static const struct of_device_id sp706_wdt_dt_ids[] = {
	{ .compatible = "linux,wdt-sp706", },
	{ }
};
MODULE_DEVICE_TABLE(of, sp706_wdt_dt_ids);

static struct platform_driver sp706_wdt_driver = {
	.driver	= {
		.name		= "sp706-wdt",
		.of_match_table	= sp706_wdt_dt_ids,
	},
	.probe	= sp706_wdt_probe,
};

static int __init sp706_wdt_init(void)
{
	return platform_driver_register(&sp706_wdt_driver);
}
arch_initcall_sync(sp706_wdt_init);

MODULE_AUTHOR("qingcheng <qingchengrd@outlook.com>");
MODULE_DESCRIPTION("SP706 Watchdog");
MODULE_LICENSE("GPL");

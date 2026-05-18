// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for Vitesse PHYs
 *
 * Author: Kriston Carson
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>

#define PHY_ID_JL31X3			0x937C4018

#define JL31X3_DEVAD_EXT1	0x1
#define JL31X3_DEVAD_EXT2	0x3
#define JL31X3_DEVAD_EXT3	0x1F

#define JL31X3_MDIO_CTRL1_REG	0x121
#define JL31X3_PMX_CTRL_REG		0x834
#define JL31X3_XMII_UCFG_REG	0x8800
#define JL31X3_TC10_CFG_REG		0x8707
#define JL31X3_PMX_CTRL1_REG	0x0


MODULE_DESCRIPTION("JLSemi PHY driver");
MODULE_AUTHOR("Qingcheng");
MODULE_LICENSE("GPL");
/*
static void _jl31x3_power_down(struct phy_device *phydev)
{
	u32 val;
	val = phy_read_mmd(phydev, JL31X3_PMX_CTRL1_REG);
	val |= (1 << 11);
	phy_write_mmd(phydev, JL31X3_PMX_CTRL1_REG, val);
}
*/
static void _jl31x3_clk_timming_set(struct phy_device *phydev)
{
	u32 val;

	val = phy_read_mmd(phydev, JL31X3_DEVAD_EXT2,  JL31X3_XMII_UCFG_REG);
	val &= ~(3 << 14);
	val |= (1 << 14);
	phy_write_mmd(phydev, JL31X3_DEVAD_EXT2, JL31X3_XMII_UCFG_REG, val);
}

static void _jl31x3_disable_mdio_brodcast(struct phy_device *phydev)
{
	u32 val;

	val = 0x9F00;
	phy_write_mmd(phydev, JL31X3_DEVAD_EXT3, JL31X3_MDIO_CTRL1_REG, val);
} 

static void _jl31x3_set_master(struct phy_device *phydev, int master)
{
	u32 val;

	val = phy_read_mmd(phydev, JL31X3_DEVAD_EXT1, JL31X3_PMX_CTRL_REG);
	if (master) 
		val |= (1 << 14);
	else 
		val &= ~(1 << 14);
	phy_write_mmd(phydev, JL31X3_DEVAD_EXT1, JL31X3_PMX_CTRL_REG, val);
}

static void _jl31x3_set_speed(struct phy_device *phydev, int speed)
{
	u32 val;

	val = phy_read_mmd(phydev, JL31X3_PMX_CTRL1_REG, JL31X3_PMX_CTRL1_REG);
	val &= ~((1 << 13) | (1 << 6));
	if (speed == 100) 
		val |= (1 << 13);
	else 
		val |= (1 << 6);
	phy_write_mmd(phydev, JL31X3_DEVAD_EXT1, JL31X3_PMX_CTRL1_REG, val);
}

static void _jl31x3_tc10_disable(struct phy_device *phydev)
{
	phy_write_mmd(phydev, JL31X3_DEVAD_EXT2, JL31X3_TC10_CFG_REG, 0);
}

static void _jl31x3_soft_reset(struct phy_device *phydev)
{
	u32 val;

	val = phy_read_mmd(phydev, JL31X3_DEVAD_EXT1, JL31X3_PMX_CTRL1_REG);
	val |= (1 << 15);
	msleep(4);
	phy_write_mmd(phydev, JL31X3_DEVAD_EXT1, JL31X3_PMX_CTRL1_REG, val);
	msleep(6);
	val = phy_read_mmd(phydev, JL31X3_DEVAD_EXT1, JL31X3_PMX_CTRL1_REG);
	printk("==>JLSemi phy status 0 [%08x] done\n", val);
}

/*
static int jl31x3_config_init(struct phy_device *phydev)
{
	int err = 0;

	err = 0;
#ifdef CONFIG_JL31X3_MASTER
	printk("==============================>set jl31x3 master\n");
	_jl31x3_set_master(phydev, 1);
#else	
	_jl31x3_set_master(phydev, 0);
#endif
	mdelay(1);
	_jl31x3_set_speed(phydev, 1000);
	//_jl31x3_clk_timming_set(phydev);
	_jl31x3_soft_reset(phydev);

	printk("==============>jl31x3 config init\n");
	return err;
}
*/

static int jl31x3_probe(struct phy_device *phydev)
{
	printk("Found jl31x3 phy!\n");
	_jl31x3_disable_mdio_brodcast(phydev);
	_jl31x3_tc10_disable(phydev);
	//_jl31x3_set_master(phydev, 1);
	//mdelay(1);
	//_jl31x3_set_speed(phydev, 1000);
	_jl31x3_clk_timming_set(phydev);
#ifdef CONFIG_JL31X3_MASTER
	printk("==============================>set jl31x3 master\n");
	_jl31x3_set_master(phydev, 1);
#else	
	_jl31x3_set_master(phydev, 0);
#endif
	mdelay(1);
	_jl31x3_set_speed(phydev, 1000);

	_jl31x3_soft_reset(phydev);
	mdelay(2);

	return 0;
}
/*
static int jl31x3_read_status(struct phy_device *phydev)
{
	int ret;	

	printk("==============>jl31x3 config init\n");
	ret = genphy_read_status(phydev);
	if (ret)
		return ret;

	printk("==============>jl31x3 speed 1000 duplex FULL\n");
	phydev->speed = SPEED_1000;
	phydev->duplex = DUPLEX_FULL;

	return 0;
}
*/

static struct phy_driver jl31x3_driver[] = {
{
	.phy_id         = PHY_ID_JL31X3,
	.name           = "JLSemi jl31x3",
	.phy_id_mask    = 0x0000fff0,
	/* PHY_GBIT_FEATURES */
	.probe			= &jl31x3_probe,
//	.config_init    = &jl31x3_config_init,
//	.read_status	= &jl31x3_read_status,
//	.config_aneg    = &vsc82x4_config_aneg,
//	.config_intr    = &vsc82xx_config_intr,
} };

module_phy_driver(jl31x3_driver);

static struct mdio_device_id __maybe_unused jl31x3_tbl[] = {
	{ PHY_ID_JL31X3, 0x0000fff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, jl31x3_tbl);

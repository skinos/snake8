## arch@powersave — Power Save Management

### Overview

**`arch@powersave`** coordinates **selective power-down and wake-up** of major platform domains on supported products: application services, CPU cores, serial console, Wi-Fi, Ethernet PHY, and LTE modem. Callers may use **aggregate** entry points (**`on`** / **`off`**) or **per-domain** methods when only part of the system should sleep or resume.

- six **domains** map to paired enter / restore APIs: **`app`** / **`app_off`**, **`cpu`** / **`cpu_off`**, **`tty`** / **`tty_off`**, **`wifi`** / **`wifi_off`**, **`eth`** / **`eth_off`**, **`lte`** / **`lte_off`**
    > domain methods change runtime state only; this component does not expose saved configuration
- **`on`** runs all six enter-domain methods in a fixed order; **`off`** runs all six restore-domain methods in a fixed order (hardware-oriented domains before application services)
    > use per-domain APIs when a partial sleep or staged wake-up is required
- methods are implemented as a shell component; each call completes with **`ttrue`** when the scripted steps finish without reporting failure


### Concepts

**Power-save domain**
* A logical slice of the platform (applications, CPU, TTY, Wi-Fi, Ethernet, LTE) that can be powered down or restored independently.
* Enter-domain APIs (**`app`**, **`cpu`**, …) move that slice toward a low-power state; restore-domain APIs (**`app_off`**, **`cpu_off`**, …) reverse those steps.

**Enter vs restore naming**
* Names such as **`app`** and **`wifi`** mean “apply power-save actions for this domain”, not “turn the domain on”.
* The **`_off`** suffix on restore APIs (**`app_off`**, **`wifi_off`**, …) means “undo the power-save shutdown for this domain” (wake / bring back), not “turn the domain off”.

**Aggregate vs per-domain control**
* **`on`** and **`off`** are convenience wrappers that invoke every domain method in sequence.
* Per-domain calls allow sleep or wake of one area without touching the others; callers must ensure domain order matches product constraints when composing their own sequences.


### API Reference

#### Management APIs


#### Query APIs


#### Control APIs

+ `on` **enter full power-save mode across all domains**
    - failed return tfalse
    - succeed return ttrue
    - Runs **`app`**, **`cpu`**, **`tty`**, **`wifi`**, **`eth`**, and **`lte`** in sequence

    Example, enter full power-save mode
    ```shell
    arch@powersave.on
    ttrue
    ```

+ `off` **restore all domains from full power-save mode**
    - failed return tfalse
    - succeed return ttrue
    - Runs **`lte_off`**, **`eth_off`**, **`wifi_off`**, **`tty_off`**, **`cpu_off`**, and **`app_off`** in sequence

    Example, restore full power-save mode
    ```shell
    arch@powersave.off
    ttrue
    ```

+ `app` **power down application-layer services for sleep**
    - failed return tfalse
    - succeed return ttrue
    - Stops **`network@connect`**, shuts syslog, admin UI, DHCP, agents, clock restart, irqbalance, and related **`arch@`** GPIO / USB / PCI helpers; drives system status GPIO low; sends LTE sleep-oriented AT commands

    Example, power down application services only
    ```shell
    arch@powersave.app
    ttrue
    ```

+ `cpu` **reduce CPU usage for sleep**
    - failed return tfalse
    - succeed return ttrue
    - Raises kernel console log level and takes secondary CPU cores (**cpu1**–**cpu3**) offline

    Example, offline secondary CPU cores
    ```shell
    arch@powersave.cpu
    ttrue
    ```

+ `tty` **power down the auxiliary serial port**
    - failed return tfalse
    - succeed return ttrue
    - Shuts **`uart@tty2`** and removes TTY2 power via GPIO

    Example, power down TTY2
    ```shell
    arch@powersave.tty
    ttrue
    ```

+ `wifi` **power down Wi-Fi stacks and unload drivers**
    - failed return tfalse
    - succeed return ttrue
    - Shuts **`wifi@n`** and **`wifi@a`**, deletes related wireless interfaces, and unloads Wi-Fi kernel modules

    Example, power down Wi-Fi
    ```shell
    arch@powersave.wifi
    ttrue
    ```

+ `eth` **power down Ethernet PHY ports and LAN3 supply**
    - failed return tfalse
    - succeed return ttrue
    - Writes low-power PHY settings on switch ports and disables **`ethernet@lan3`** power via GPIO

    Example, power down Ethernet PHY
    ```shell
    arch@powersave.eth
    ttrue
    ```

+ `lte` **power down the cellular modem**
    - failed return tfalse
    - succeed return ttrue
    - Shuts **`modem@lte`** and asserts the LTE reset GPIO

    Example, power down LTE modem
    ```shell
    arch@powersave.lte
    ttrue
    ```

+ `app_off` **restore application-layer services after sleep**
    - failed return tfalse
    - succeed return ttrue
    - Restores syslog, GPIO, USB, PCI, irqbalance, agents, admin UI, DHCP, clock restart, and **`network@connect`**; sends LTE wake-oriented AT commands

    Example, restore application services only
    ```shell
    arch@powersave.app_off
    ttrue
    ```

+ `cpu_off` **bring secondary CPU cores back online**
    - failed return tfalse
    - succeed return ttrue
    - Sets **cpu1**–**cpu3** online

    Example, online secondary CPU cores
    ```shell
    arch@powersave.cpu_off
    ttrue
    ```

+ `tty_off` **restore the auxiliary serial port**
    - failed return tfalse
    - succeed return ttrue
    - Enables TTY2 power via GPIO and runs **`uart@tty2`** setup

    Example, restore TTY2
    ```shell
    arch@powersave.tty_off
    ttrue
    ```

+ `wifi_off` **reload Wi-Fi drivers and restore Wi-Fi setup**
    - failed return tfalse
    - succeed return ttrue
    - Runs **`kmodloader`**, then **`wifi@a`** and **`wifi@n`** setup

    Example, restore Wi-Fi stacks
    ```shell
    arch@powersave.wifi_off
    ttrue
    ```

+ `eth_off` **restore Ethernet PHY ports and LAN3 supply**
    - failed return tfalse
    - succeed return ttrue
    - Enables **`ethernet@lan3`** power via GPIO and writes normal PHY settings on switch ports

    Example, restore Ethernet PHY
    ```shell
    arch@powersave.eth_off
    ttrue
    ```

+ `lte_off` **restore the cellular modem**
    - failed return tfalse
    - succeed return ttrue
    - Deasserts the LTE reset GPIO and runs **`modem@lte`** setup

    Example, restore LTE modem
    ```shell
    arch@powersave.lte_off
    ttrue
    ```


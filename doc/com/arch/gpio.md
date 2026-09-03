## arch@gpio — GPIO and LED Control

### Overview

**`arch@gpio`** maps **board GPIO aliases to pin numbers**, drives **LEDs and outputs**, samples **inputs**, and runs a **netlink button service**. Callers use named aliases from the saved map (or a numeric pin) with **`in`** / **`out`** / **`timer`**. Joint and machine events are applied through **`action`**.

- each JSON key that is not a suffix field is an **alias → pin number**; **`setup`** copies those values into the object register for later **`in`** / **`out`** / **`timer`**
    > suffix keys: **`<alias>_out`** / **`<alias>_in`** invert the logic level; **`<alias>_init`** applies **`out[<alias>, <value>]`** at **`setup`**; **`<alias>_force`** does the same then stores pin **-1** so later **`in`** / **`out`** / **`timer`** cannot use that alias
- **`led`** selects LED policy; register **`factory_mode` > 0** forces factory blink and overrides **`led`**
    > **`disable`**: power and sys LEDs off; unset / normal: power on and sys blink 500/500 ms; factory: power on and sys blink 1000/4000 ms
- **`service`** watches kernel button uevents; a long **reset** press restores default (≥5 s) or factory (≥40 s)
- if **`gpio-setup.sh`** exists in the project search path, **`setup`** runs it after the LED policy


### Configuration reference ( arch@gpio )

```json
// Attributes introduction 
{
    "led": "LED policy",                                    // [ "disable" ], omit for normal LED policy
    "sys": "system LED GPIO pin",                           // [ string ], pin number as string
    "sys_out": "sys output active-level invert",            // [ string ], "1" inverts the driven level
    "power": "power LED GPIO pin",                          // [ string ], used at setup for LED policy
    "alias": "GPIO pin number for this name",               // [ string ], any extra key is an alias, e.g. "modem@lte_reset"
    "alias_out": "output active-level invert",              // [ string ], "1" inverts out/timer for that alias
    "alias_in": "input active-level invert",                // [ string ], "1" inverts in for that alias
    "alias_init": "level applied at setup",                 // [ string ], key without the _init suffix is driven
    "alias_force": "level applied at setup, then drop alias", // [ string ], key without the _force suffix is driven, then the alias pin is set to -1
    "alias_intro": "human label"                            // [ string ], documentation only
}
```

#### Configuration example

Example, show a typical board GPIO map
```shell
arch@gpio
{
    "sys":"27",                                             # system LED pin 27
    "modem@lte_reset":"23",                                 # LTE reset output
    "modem@lte_state":"13",                                 # LTE state LED
    "modem@lte_signal":"14",                                # LTE signal LED
    "modem@lte2_reset":"22",                                # second LTE reset
    "modem@lte2_state":"15",                                # second LTE state LED
    "modem@lte2_signal":"16",                                # second LTE signal LED
    "storage@media_state":"17",                             # storage present LED
    "g1":"32",                                              # spare GPIO 1
    "g1_out":"1"                                            # spare GPIO 1 active-high invert
}
```

#### Configuration settings example

Example, set the system LED pin
```shell
arch@gpio:sys=27
ttrue
```

Example, merge set LED and LTE reset pins( include "sys" "modem@lte_reset" "modem@lte_reset_out" )
```shell
arch@gpio|{"sys":"27","modem@lte_reset":"23","modem@lte_reset_out":"1"}
ttrue
```


### API Reference

#### Management APIs

+ `setup[]` **load the GPIO map, apply LED policy, start the button service**
    - failed return tfalse
    - succeed return ttrue
    - Copies each configured alias into the object register, turns LTE / connect LEDs off, applies **`led`** / factory policy, runs **`gpio-setup.sh`** when present, applies **`*_init`** outputs, then starts **`service`**
    - Lifecycle method; product **`uninit`** calls **`shut`**

+ `shut[]` **stop the button service**
    - failed return tfalse
    - succeed return ttrue

+ `service[]` **netlink loop for the reset button**
    - failed return tfalse
    - succeed return ttrue
    - Listens for **`SUBSYSTEM=button`** / **`BUTTON=reset`** / **`ACTION=released`**
    - Hold ≥40 s: **`land@machine.factory`** after **`restart_unblock`**; hold ≥5 s: **`land@machine.default`**
    - Started by **`setup`**; not intended for manual invocation

+ `monitor[ gpio, pid, type ]` **watch one GPIO and signal a process**
    - gpio -------------- [ string ], alias or numeric pin
    - pid ---------------- [ string ], optional, process to **`SIGWINCH`** on edge; omit to exit on first change
    - type --------------- [ string ], optional, irq type passed to the kernel (default **`both`**)
    - failed return terror
    - succeed return tfalse
    - Long-running worker started by **`listen`**; returns **`tfalse`** when the watch ends


#### Query APIs

+ `in[ name ]` **sample a GPIO input**
    - name --------------- [ string ], numeric pin or configured alias
    - failed return NULL
    - succeed return [ number ], 0 or 1 after optional **`<name>_in`** invert

    Example, read the system LED sense pin
    ```shell
    arch@gpio.in[ sys ]
    1
    ```


#### Control APIs

+ `out[ name, value ]` **drive a GPIO or LED output**
    - name --------------- [ string ], numeric pin or configured alias
    - value -------------- [ string ], **`0`** or **`1`** before optional **`<name>_out`** invert
    - failed return tfalse
    - succeed return ttrue
    - Uses the LED sysfs path when that GPIO is exported as an LED; otherwise sysfs GPIO out
    - Stops an active **`listen`** monitor on the same name

    Example, assert LTE reset
    ```shell
    arch@gpio.out[ modem@lte_reset, 1 ]
    ttrue
    ```

+ `timer[ name, high, low ]` **blink an LED with high/low intervals**
    - name --------------- [ string ], numeric pin or configured alias
    - high --------------- [ string ], milliseconds the LED is on (swapped when **`<name>_out`** inverts)
    - low ---------------- [ string ], milliseconds the LED is off
    - failed return tfalse
    - succeed return ttrue
    - Stops an active **`listen`** monitor on the same name

    Example, blink the system LED 500 ms on / 500 ms off
    ```shell
    arch@gpio.timer[ sys, 500, 500 ]
    ttrue
    ```

+ `listen[ name, pid, type ]` **start edge watch on a GPIO**
    - name --------------- [ string ], alias or numeric pin
    - pid ---------------- [ string ], process id to signal
    - type --------------- [ string ], optional, irq type (default **`both`**)
    - failed return tfalse
    - succeed return ttrue
    - Spawns **`monitor`** as **`<name>-monitor`**

    Example, watch g1 and signal pid 1234
    ```shell
    arch@gpio.listen[ g1, 1234, both ]
    ttrue
    ```

+ `action[ id, object, paramter ]` **apply an LED / GPIO pattern for an event**
    - id ----------------- [ string ], event id such as **`modem/ready`** or **`firmware/upgrading`**
    - object ------------- [ string ], optional, object prefix for modem / network LEDs
    - paramter ----------- [ string ], optional, extra value (modem signal level 1–4)
    - failed return tfalse
    - succeed return ttrue
    - Factory / upgrade / restart ids always run; modem / network / storage / system ids run only in normal LED mode

    Example, show LTE ready on the first modem
    ```shell
    arch@gpio.action[ modem/ready, modem@lte ]
    ttrue
    ```

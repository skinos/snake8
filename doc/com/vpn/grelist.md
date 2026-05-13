***
## Management of GRE Tunnel Instance
Management of GRE tunnel Instance


#### **Methods**

+ `setup[]` **setup the gre frame at system boot**, *succeed return ttrue, failed return tfalse, error return terror*

+ `shut[]` **shutdown the gre frame that will stop all tunnel**, *succeed return ttrue, failed return tfalse, error return terror*

+ `list[]` **list all gre tunnel configure**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "vpn@gre":                     // [ "vpn@gre", "vpn@gre2", "vpn@gre3", ... ]:
        {
        }
        // ... more tunnel
    }
    ```
    ```shell
    # examples, list all gre tunnel
    vpn@grelist.list
    {
    }
    ```

+ `status[]` **list all gre tunnel status**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "vpn@gre":                     // [ "vpn@gre", "vpn@gre2", "vpn@gre3", ... ]:
        {
        }
        // ... more tunnel
    }
    ```
    ```shell
    # examples, list all gre tunnel
    vpn@grelist.status
    {
    }
    ```

+ `add[ [peer] ]` **add a gre tunnel**, *succeed return the gre object name, failed return NULL*
    ```shell
    # examples
    vpn@grelist.add[ www.gretest.com ]
    vpn@gre4
    ```

+ `delte[ gre object ]` **delete a gre tunnel**, *succeed return ttrue, failed return tfalse*
    ```shell
    # examples
    vpn@grelist.delete[ vpn@gre4 ]
    ttrue
    ```

***
## Management of PPTP Client Instance
Management of PPTP client Instance


#### **Methods**

+ `setup[]` **setup the pptp frame at system boot**, *succeed return ttrue, failed return tfalse, error return terror*

+ `shut[]` **shutdown the pptp frame that will stop all client**, *succeed return ttrue, failed return tfalse, error return terror*

+ `list[]` **list all pptp client configure**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "vpn@pptp":                     // [ "vpn@pptp", "vpn@pptp2", "vpn@pptp3", ... ]:
        {
        }
        // ... more client
    }
    ```
    ```shell
    # examples, list all pptp client
    vpn@pptplist.list
    {
    }
    ```

+ `status[]` **list all pptp client status**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "vpn@pptp":                     // [ "vpn@pptp", "vpn@pptp2", "vpn@pptp3", ... ]:
        {
        }
        // ... more client
    }
    ```
    ```shell
    # examples, list all pptp client status
    vpn@pptplist.status
    {
    }
    ```

+ `add[ [server] ]` **add a pptp client**, *succeed return the pptp object name, failed return NULL*
    ```shell
    # examples
    vpn@pptplist.add[ www.pptptest.com ]
    vpn@pptp4
    ```

+ `delte[ pptp object ]` **delete a pptp client**, *succeed return ttrue, failed return tfalse*
    ```shell
    # examples
    vpn@pptplist.delete[ vpn@pptp4 ]
    ttrue
    ```


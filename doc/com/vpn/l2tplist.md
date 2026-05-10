***
## Management of L2TP Client Instance
Management of L2TP client Instance


#### **Methods**

+ `setup[]` **setup the l2tp frame at system boot**, *succeed return ttrue, failed return tfalse, error return terror*

+ `shut[]` **shutdown the l2tp frame that will stop all client**, *succeed return ttrue, failed return tfalse, error return terror*

+ `list[]` **list all l2tp client configure**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "vpn@l2tp":                     // [ "vpn@l2tp", "vpn@l2tp2", "vpn@l2tp3", ... ]:
        {
        }
        // ... more client
    }
    ```
    ```shell
    # examples, list all l2tp client
    vpn@l2tpclient.list
    {
    }
    ```

+ `status[]` **list all l2tp client status**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "vpn@l2tp":                     // [ "vpn@l2tp", "vpn@l2tp2", "vpn@l2tp3", ... ]:
        {
        }
        // ... more client
    }
    ```
    ```shell
    # examples, list all l2tp client
    vpn@l2tpclient.list
    {
    }
    ```

+ `add[ [server], [port] ]` **add a l2tp client**, *succeed return the l2tp object name, failed return NULL*
    ```shell
    # examples
    vpn@l2tpclient.add[ www.l2tptest.com, 1701 ]
    vpn@l2tp4
    ```

+ `delte[ l2tp object ]` **delete a l2tp client**, *succeed return ttrue, failed return tfalse*
    ```shell
    # examples
    vpn@l2tpclient.delete[ vpn@l2tp4 ]
    ttrue
    ```


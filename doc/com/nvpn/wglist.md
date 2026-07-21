***
## Management of WireGuard Interface List
Management of WireGuard Interface List

#### Configuration( nvpn@wglist )
**nvpn@wglist** manages up to 10 WireGuard instances: `nvpn@wg`, `nvpn@wg2` ... `nvpn@wg10`.

#### **Methods**

+ `setup[]` **setup WireGuard infrastructure**, *succeed return ttrue, failed return tfalse*
+ `shut[]` **shutdown all WireGuard instances**, *succeed return ttrue, failed return tfalse*
+ `list[]` **list all WireGuard instance configurations**, *succeed return talk keyed by object path*
+ `status[]` **list runtime status of all instances**, *succeed return talk keyed by object path*
+ `add[ip,mask]` **create a new WireGuard instance**, *succeed return object path, failed return NULL*
    ```shell
    nvpn@wglist.add[10.0.0.2,255.255.255.0]
    nvpn@wg
    ```
+ `delete[fullpath]` **delete a WireGuard instance**, *succeed return ttrue, failed return tfalse*
    ```shell
    nvpn@wglist.delete[nvpn@wg2]
    ```

Runtime requires platform `wg` tool and `wireguard.ko` (pdriver).

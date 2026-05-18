## land@fpk — FPK Package Management

Manage system FPK projects.

### Concepts

#### Project concept
* A project is a collection of programs developed to meet specific requirements, similar to an application on Windows or Android.
* Each project has a project directory under `./project` in the SDK. Source code and resources are stored in that directory.
* All development work should be organized within a project.
* Each project directory must include:
    - Project information file, named **prj.json**
    - Project build Makefile. For example, when using the OpenWrt build system, the Makefile follows OpenWrt format.
* Each project directory can contain:
    - Executable files or executable source code (SDK contains source; device installation contains binaries only)
    - Libraries or library source code (SDK contains source; device installation contains binaries only)
    - Kernel drivers or driver source code (SDK contains source; device installation contains binaries only)
    - Component source code (SDK contains source; device installation contains component binaries only)
    - Default profiles for the component or project
    - Web page files for user management
    - Language files for the web page interface
    - Script files and other resource files

#### FPK concept
* After development is complete, the project is packaged into an FPK (installation package) and installed into the system.
* An FPK is similar to an installer package on Windows or an APK on Android.
* The FPK filename ends with `.fpk`
* The FPK can be installed through the web page or command line.
* Every FPK must include:
	- Project information file named **prj.json**
* Each FPK can contain:
	- Libraries
	- Executable files
	- Driver files
	- Component files ending with `.com` / `.ash`
	- Configuration files ending with `.cfg`
	- Web page files ending with `.html`
	- Language files ending with `.json`
	- Shell script files ending with `.sh`
	- Other resource files
	- Library header files for development under `install/include` (used in SDK compilation)
	- Libraries for development under `install/lib` (used in SDK compilation)

#### Runtime install paths (symbols)

Documentation uses **angle-bracket placeholders** instead of fixed paths. They correspond to **platform build macros** (values vary by platform / product); the table below lists the usual macro names:

| Symbol | Typical meaning | C macro (reference) |
|--------|-----------------|---------------------|
| **`⟨PRJ_ROOT⟩`** | Root directory of **installed** projects (FPK payloads) | **`PROJECT_DIR`** |
| **`⟨PRJ_NAME⟩`** | One project’s subdirectory (same as `prj.json` → `name`) | — |
| **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** | That project’s install prefix on the device | — |
| **`⟨LIB_DIR⟩`** | Global shared-library directory used for symlinks | **`PROJECT_LIB_DIR`** |
| **`⟨BIN_DIR⟩`** | Global command directory used for symlinks | **`PROJECT_BIN_DIR`** |
| **`⟨SYS_ROOT⟩`** | Running system root (for merged `rootfs/` trees, `/etc`, …) | — |

Examples such as **`land@fpk.list`** JSON fields use **`⟨PRJ_ROOT⟩/…`** so they stay valid when **`PROJECT_DIR`** is changed at build time.

#### prj.json: project information file
This file is generated automatically when the project is created. Understanding its format helps validate project development output.
```json
// Attributes
{
    "name":"project (FPK) name",                   // [ string ]
    "intro":"project (FPK) introduction",          // [ string ]
    "desc":"detailed description of the project",  // [ string ]
    "type":"project (FPK) type",                   // [ "root" ], "root" means root permission is required
    "version":"project (FPK) version",             // [ string ]
    "author":"project (FPK) author",               // [ string ]

    "osc":                           // all open-source programs included in the project are shown in these attributes
    {
        "open-source program directory":"description"
        // "...":"..." One property is shown for each open-source program
    },
    "lib":                           // all libraries included in the project are shown in these attributes
    {
        "library directory":"description"
        // "...":"..." One property is shown for each library
    },
    "exe":                           // all executable programs included in the project are shown in these attributes
    {
        "executable program directory":"description"
        // "...":"..." One property is shown for each executable program
    },
    "com":                           // all components included in the project are shown in these attributes
    {
        "component directory":"description"
        // "...":"..." One property is shown for each component
    },
    "res":                           // all resource files or directories included in the project are shown in these attributes
    {
        "resource file or directory":"description"
        // "...":"..." One property is shown for each resource file or directory
    },
    "obj":                           // all objects (dynamic components) included in the project are shown in these attributes
    {
        "object name":"actual components"
        // "...":"..." One property is shown for each object (dynamic component)
    },
    "init":                          // all startup items included in the project are shown in these attributes
    {
        "initialize level":"components method"
        // "...":"..." One property is shown for each startup item
    },
    "uninit":                        // all shutdown items included in the project are shown in these attributes
    {
        "shutdown level":"components method"
        // "...":"..." One property is shown for each shutdown item
    },
    "joint":                         // all joint process items included in the project are shown in these attributes
    {
        "joint event":"components method"
        // "...":"..." One property is shown for each joint process item
    }
}
// Example
{
    "name":"arch",                              // arch project
    "intro":"mtk platform layer for farm os",   // project introduction
    "desc":"This project for MTK chips provides a unified management and usage interface for upper-layer projects",
                                                // project description
    "type":"root",                              // indicates root permission is required
    "version":"6.0.0",                          // version is 6.0.0
    "author":"dimmalex@gmail.com",              // author is dimmalex@gmail.com
    "osc":                                      // has 1 open-source program
    {
        "ntpclient":"ntp client"                      // ntpclient
    },
    "lib":                                      // has 1 library
    {
        "land":"core library"                         // land library
    },
    "exe":                                      // has 2 executable programs
    {
        "daemon":"service daemon",                    // daemon, service implementation
        "he":"tools for calling all components"       // he command tool for skinos
    },
    "com":                                      // has 7 components
    {
        "device":"device information",                // device, manages all devices
        "data":"data management",                     // data, manages configuration and EEPROM
        "firmware":"firmware management",             // firmware
        "gpio":"register and gpio management",        // gpio
        "test":"test device management",              // test, manages factory tests
        "ethernet":"ethernet switch management",      // ethernet
        "mt7628":"802.11n wireless management"        // mt7628
    },
    "res":                                      // has 1 resource file
    {
        "testpage.py":"test only"                    // test tools for factory
    },
    "obj":                                      // has 2 objects (dynamic components)
    {
        "wifi@nradio":"mt7628",                      // object is wifi@nradio, actual components is mt7628
        "test":"test"                                // object is test, actual components is test
    },
    "init":                                      // has 3 startup items
    {
        "ethernet":"arch@ethernet.setup",            // called at initialize level ethernet
        "nradio":"wifi@nradio.setup",                // called at initialize level nradio
        "aradio":"wifi@aradio.setup"                 // called at initialize level aradio
    },
    "uninit":                                      // has 2 shutdown items
    {
        "nradio":"wifi@nradio.shut",                 // called at shutdown level nradio
        "aradio":"wifi@aradio.shut"                  // called at shutdown level aradio
    },
    "joint":                                      // has 14 joint process items
    {
        "firmware/upgrading":"arch@gpio.event",  // called when firmware/upgrading happens
        "firmware/upgraded":"arch@gpio.event",   // arch@gpio.event call when firmware/upgraded happens
        "network/arise":"arch@gpio.event",       // arch@gpio.event call when network/arise happens
        "network/ready":"arch@gpio.event",
        "network/lining":"arch@gpio.event",
        "network/online":"arch@gpio.event",
        "network/offline":"arch@gpio.event",
        "modem/poweron":"arch@gpio.event",
        "modem/poweroff":"arch@gpio.event",
        "modem/msim":"arch@gpio.event",
        "modem/bsim":"arch@gpio.event",
        "signal/flash":"arch@gpio.event",
        "nssid/up":"arch@gpio.event",
        "nssid/down":"arch@gpio.event"
    }    
}
```

### Configuration ( `land@fpk` )

The **saved configuration object** for `land@fpk` (query/set via `land@fpk`, `land@fpk:path`, merge `|{json}`, etc.).


`land@fpk` does **not** use a standalone JSON configuration document like feature components. Each installed project carries its own **`prj.json`** under **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`**. The methods below register, unregister, or inspect those projects at runtime.

### Component API

+ `register[ project directory [, ...] ]` **register project(s) to the system**. This API is called at startup to register all projects.
    - project directory ----------- [ string ], project directory
    - ... ------------------------- [ string ], register multiple project directories
    - failed return tfalse
    - succeed return ttrue

    Example, register one project
    ```shell
    land@fpk.register[ ⟨PRJ_ROOT⟩/uart ]
    ttrue
    ```

+ `unregister[ project name [, ...] ]` **unregister project(s) from the system**
    - project name ----------- [ string ], project name
    - ... ------------------------- [ string ], unregister multiple project names
    - failed return tfalse
    - succeed return ttrue

    Example, unregister one project
    ```shell
    land@fpk.unregister[ uart ]
    ttrue
    ```

+ `list[ [project] ]` **list project information**
    - project ----------- [ string ], when provided, get details for the specified project
    - returns JSON

    Example, show project information for `ifname`
    ```shell
    land@fpk.list[ifname]
    {
        "name":"ifname",
        "intro":"skinos common network connection",
        "desc":"skinos ip connect and wifi connect component",
        "type":"root",
        "version":"8.0.0",
        "author":"dimmalex@gmail.com",
        "com":
        {
            "ethcon":"ethernet connect component",
            "ltecon":"lte modem connect component"
        },
        "path":"⟨PRJ_ROOT⟩/ifname/",
        "size":"74923"
    }
    ```
    Example, show all project information
    ```
    land@fpk.list
    {
        "agent":
        {
            "path":"⟨PRJ_ROOT⟩/agent/",
            "size":"155997",
            "intro":"agent for remote or cloud control",
            "version":"7.0.0",
            "author":"dimmalex@gmail.com"
        },
        "arch":
        {
            "path":"⟨PRJ_ROOT⟩/arch/",
            "size":"289294",
            "intro":"mtk mt7981 platform layer for skinos",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "client":
        {
            "path":"⟨PRJ_ROOT⟩/client/",
            "size":"66180",
            "intro":"Client management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "clock":
        {
            "path":"⟨PRJ_ROOT⟩/clock/",
            "size":"59199",
            "intro":"System clock management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "forward":
        {
            "path":"⟨PRJ_ROOT⟩/forward/",
            "size":"157035",
            "intro":"Network forward function",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "ifname":
        {
            "path":"⟨PRJ_ROOT⟩/ifname/",
            "size":"74923",
            "intro":"skinos common network connection",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "land":
        {
            "path":"⟨PRJ_ROOT⟩/land/",
            "size":"153711",
            "intro":"component infrastructure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "modem":
        {
            "path":"⟨PRJ_ROOT⟩/modem/",
            "size":"185095",
            "intro":"modem management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "network":
        {
            "path":"⟨PRJ_ROOT⟩/network/",
            "size":"479667",
            "intro":"network infrastructure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "pdriver":
        {
            "path":"⟨PRJ_ROOT⟩/pdriver/",
            "size":"467419",
            "intro":"Portable driver",
            "version":"6.0.0",
            "author":"dimmalex@gmail.com"
        },
        "tui":
        {
            "path":"⟨PRJ_ROOT⟩/tui/",
            "size":"37987",
            "intro":"Terminal user interface service",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "webs":
        {
            "path":"⟨PRJ_ROOT⟩/webs/",
            "size":"49283",
            "intro":"web server",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "wifi":
        {
            "path":"⟨PRJ_ROOT⟩/wifi/",
            "size":"108456",
            "intro":"skinos wireless configure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "wui":
        {
            "path":"⟨PRJ_ROOT⟩/wui/",
            "size":"20066",
            "intro":"web user interface page",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        }
    }
    ```


+ `install[ path [,...] ]` **install FPK archive(s) or expanded project directory(ies)**
    - path --------------- [ string ], each argument may be a **`.fpk` / tarball** (extracted under `/tmp` first) or a **directory** that already contains **`prj.json`**
    - ... ---------------- [ string ], install multiple paths in one call
    - failed return tfalse
    - succeed return ttrue (behavior described above: directory or archive)

    Example, install the `wui` project FPK to the system
    ```shell
    land@fpk.install[ wui-7.0.0-x86.fpk ]
    ttrue
    ```

+ `uninstall[ project name [,...] ]` **uninstall project(s) from the system**
    - project name ----------- [ string ], project name
    - ... ---------------- [ string ], uninstall multiple projects
    - failed return tfalse
    - succeed return ttrue

    Example, uninstall the `wui` project from the system
    ```shell
    land@fpk.uninstall[ wui ]
    ttrue
    ```

+ `number` **get the number of installed projects**
    - failed return tfalse
    - return the number of installed projects

    Example, get the number of installed projects
    ```shell
    land@fpk.number
    15
    ```

+ `project_add[ name, [intro] ]` **create a minimal writable project** (under **`PROJECT_APP_DIR`**, same tree **`install`** uses)
    - **name** ----------- [ string ], directory / `prj.json` → `name` (required)
    - **intro** ---------- [ string ], optional; written into **`prj.json`**
    - failed return **NULL** (**EINVAL** if name missing)
    - succeed return JSON (includes **`path`** to the new project directory, default **`version`**, **`author`** set to **`land@fpk`**)

+ `project_delete[ name ]` **delete a project directory**
    - **name** ----------- [ string ], project name whose path is resolved by **`project_path`**
    - failed return **tfalse**
    - succeed return **ttrue** (runs **`rm -fr`** on that directory)

+ `project_check[ name ]` **validate that a project exists and passes internal checks**
    - **name** ----------- [ string ]
    - failed return **tfalse**
    - succeed return **ttrue** when the project passes **`project_check`**

+ `project_pack[ name ]` **pack a writable project into a `.fpk` tarball**
    - **name** ----------- [ string ], must resolve under **`PROJECT_APP_DIR`** (not under firmware **`PROJECT_DIR`** — otherwise **EPERM** / **NULL**)
    - failed return **NULL**
    - succeed return talk string path to **`⟨TMP⟩/name-⟨version⟩-⟨hardware⟩.fpk`** ( **`PROJECT_TMP_DIR`**, **`PROJECT_DEFAULT_VERSION`**, register **`hardware`**; archive is **`tar zcf`** of project contents)

+ `com_add[ prj, name, [intro] ]` **add a shell-backed component stub to a project**
    - **prj** ------------ [ string ], project name (path via **`project_path`**)
    - **name** ----------- [ string ], component short name; runtime object is **`prj`⟨@⟩`name`** (see **`PROJECT_OBJECT_GAPC`**)
    - **intro** ---------- [ string ], optional description stored under **`prj.json` → `com`**
    - copies template **`comshell`** into the project, updates **`prj.json`**, then calls **`com_register( prj@name, path_to_comshell, 0 )`** — second-argument semantics match **`land@component.register`** / **`com_register(..., 0)`** (**`COM_COM`** map lookup for **`type` 0**)
    - failed return **NULL**
    - succeed return talk string: project directory path

+ `wui_add[ prj, name, menu ]` **add a Web UI menu entry and scaffold files**
    - **prj** ------------ [ string ], project name
    - **name** ----------- [ string ], key under **`prj.json` → `wui`**
    - **menu** ----------- [ string ], English menu label (stored in **`en`**)
    - creates **`name.html`** from template, **`name-cn.json`** / **`name-en.json`**, updates **`prj.json`**
    - failed return **NULL**
    - succeed return JSON for the new **`wui`** entry (paths filled to absolute files)

+ `obj_add[ prj, object, origin ]` **add a dynamic object mapping and register it**
    - **prj** ------------ [ string ], project name
    - **object** --------- [ string ], object name ( **`prj.json` → `obj`** key)
    - **origin** --------- [ string ], backing component catalog name ( **`obj`** value), used to build **`land@component.register`** second argument as **`prj`⟨@⟩`origin`**
    - updates **`prj.json`**, then **`land@component.register[ object, prj@origin ]`**
    - failed return **tfalse**
    - succeed return **ttrue**

+ `init_add[ prj, level, call ]` **append an init slot and register it at runtime**
    - **prj** / **level** / **call** --- [ string ]; **`level`** is the key under **`prj.json` → `init`**, **`call`** is the method string (e.g. **`arch@ethernet.setup`**)
    - updates **`prj.json`**, then **`land@init.register[ level, call ]`**
    - failed return **tfalse**, succeed **ttrue**

+ `uninit_add[ prj, level, call ]` **append an uninit slot and register it**
    - same shape as **`init_add`**, for **`prj.json` → `uninit`** and **`land@uninit.register`**

+ `joint_add[ prj, level, call ]` **append a joint slot and register it**
    - same shape as **`init_add`**, for **`prj.json` → `joint`** and **`land@joint.register`** ( **`level`** is the event key)

+ `wui_menu[ [type] ]` **get the Web UI menu structure**
    - **type** ----------- [ string ], optional; names the **`prj.json`** top-level object to scan (default **`wui`**). Use another key if your project stores pages there under a different section name.
    - Skips projects whose name maps to **`disable`** in the **`project`** object under the platform **custom** component (object name varies by product; often **`arch@custom`**).
    - For each menu item: may require existing config (**`config`** / **`attr`**) or a live **`object`**/**`api`** (**`com_have`**); entries that fail those checks are omitted.
    - failed return NULL
    - return JSON describing the Web UI menu structure (**`page`** / **`lang`** paths rewritten to absolute paths)

    Example, get the Web UI menu
    ```shell
    land@fpk.wui_menu
    {
        "wifi_aclient":
        {
            "menu":"Wireless",
            "cn":"5.8G客户端",
            "en":"5.8G Clients",
            "page":"/skinos/wifi/client.html",
            "object":"wifi@a",
            "lang":
            {
                "cn":"/skinos/wifi/cn",
                "en":"/skinos/wifi/en"
            }
        },
        "wui_webs":
        {
            "menu":"System",
            "cn":"WEB服务器",
            "en":"Web Server",
            "page":"/skinos/wui/admin.html",
            "config":"wui@admin",
            "lang":
            {
                "cn":"/skinos/wui/cn",
                "en":"/skinos/wui/en"
            }
        }
    }
    ```

### Lifecycle API

+ **No** `setup[]` in the usual sense — **`land@fpk`** exposes **register/install** APIs used during boot.
+ See **Component API** for **`register[]`**, **`install[]`**, etc.


### C Code Example

```c
#include "skin/skin.h"

static void example_land_fpk(void)
{
    talk_t ret = scall("land@fpk", "number", NULL);
    (void)ret;
}
```



***
## FPK Management
Manage system FPK(project)

#### The following describes the project concept 
* A project is a collection of programs developed for the completion of specific needs, equivalent to a Windows application, an apk in Android
* Each project has a corresponding directory, called the project directory, stored in the ./project directory in the SDK, and the project-related source code and resource files are stored in the project directory
* All development and programming should be in a project
* Each project directory must include: 
    - Project information file, named **prj.json**
    - Projects compile Makefile, such as When using Openwrt's compilation system, Makefile uses Openwrt's format
* Each project directory can contain:
    - Executable program or executable program source code (in the SDK it will be the source code of the executable program, and only the executable program will be installed on the device)
    - Library or library source code (in the SDK there will be the source code for the library, whereas installed to the device there will only be binaries of the library) 
    - Kernel driver or its source code (in the SDK there will be driver source code, whereas only driver binaries will be installed on the device)  
    - Component source code (component source code in the SDK, only component binaries installed in the device)  
    - Default profile for component or project
    - Webpage interface files for user Management
    - Language files for webpage interface
    - Script files and other resource files

#### The following introduces the FPK concept
* After the project development is completed, then package into a FPK(installation package), the FPK will be installed into the system
* FPK is equivalent to the software installation package in Windows, the apk file in Android
* The FPK ending in .fpk
* The FPK can be installed into the system through the webpage or command
* Every FPK must include: 
	- Project information file, named **prj.json**
* Each FPK can contain:
	- library
	- executable file
	- Driver files
	- .com/.ash end of the component file
	- .cfg the end of the configuration file
	- .html end of the webpage interface file
	- Language file ending with .json
	- .sh end of the SHELL script file
	- Other resource files
	- Library header files for development under install/include (used in SDK compilation)
	- Libraries for development under install/lib (used in SDK compiled)


#### prj.json: Project information file
It is generated automatically when the project is created, you can learn his format to judge your work on development projects
```json
// Attributes introduction 
{
    "name":"project(FPK) name",                    // [ string ]
    "intro":"project(FPK) introduction",           // [ string ]
    "desc":"detailed description of the project",  // [ string ]
    "type":"project(FPK) type",                    // [ "root" ], root indicates that the root permission is required
    "version":"project(FPK) version",              // [ string ]
    "author":"project(FPK) author",                // [ string ]

    "osc":                           // all open source programs included in the project show in this attributes
    {
        "open source program directory":"description"
        // "...":"..." How many open source program show how many properties
    },
    "lib":                           // all libraray included in the project show in this attributes
    {
        "library directory":"description"
        // "...":"..." How many library show how many properties
    },
    "exe":                           // all execute program included in the project show in this attributes
    {
        "execute program directory":"description"
        // "...":"..." How many execute program show how many properties
    },
    "com":                           // all component included in the project show in this attributes
    {
        "component directory":"description"
        // "...":"..." How many component show how many properties
    },
    "res":                           // all resource file or directory included in the project show in this attributes
    {
        "resource file or directory":"description"
        // "...":"..." How many resource file or directory show how many properties
    },
    "obj":                           // all object( Dynamic components ) included in the project show in this attributes
    {
        "object name":"actual components"
        // "...":"..." How many object( Dynamic components ) show how many properties
    },
    "init":                           // all starting items included in the project show in this attributes
    {
        "initialize level":"components method"
        // "...":"..." How many starting items show how many properties
    },
    "uninit":                           // all shutdown items included in the project show in this attributes
    {
        "shutdown level":"components method"
        // "...":"..." How many shutdown items show how many properties
    },
    "joint":                           // all joint process items included in the project show in this attributes
    {
        "joint event":"components method"
        // "...":"..." How many joint process items show how many properties
    }
}
// examples
{
    "name":"arch",                              // arch project
    "intro":"mtk platform layer for farm os",   // project introduction
    "desc":"This project at MTK chip will provide a unified management or use interface to the upper layerthe proejct", 
                                                // project description
    "type":"root",                              // that the root permission is required
    "version":"6.0.0",                          // version is 6.0.0
    "author":"dimmalex@gmail.com",              // author is dimmalex@gmail.com
    "osc":                                      // have 1 open source program
    {
        "ntpclient":"ntp client"                      // ntpclient, is a ntp client
    },
    "lib":                                      // have 1 library
    {
        "land":"core library"                         // land library, core library
    },
    "exe":                                      // have 3 execute program
    {
        "daemon":"service daemon",                    // daemon, service implementation
        "he":"tools for call all component"           // he, command tools for skinos
    },
    "com":                                      // have 7 execute program
    {
        "device":"device infomation",                 // device, manage all device
        "data":"data management",                     // data, manage configure and eeprom
        "firmware":"firmware management",             // firmware, manage firmware
        "gpio":"register and gpio management",        // gpio, manage mtk gpio
        "test":"test the device management",          // test, manage factory test
        "ethernet":"ethernet switch management",      // ethernet, manage mtk switch
        "mt7628":"802.11n wireless management"        // mt7628, manage mt7628 wireless radio
    },
    "res":                                      // have 1 resource file
    {
        "testpage.py":"only test"                    // testpage.py, test tools at factory
    },
    "obj":                                      // have 2 object( Dynamic components )
    {
        "wifi@nradio":"mt7628",                      // object is wifi@nradio, actual components is mt7628
        "test":"test"                                // object is test, actual components is test
    },
    "init":                                      // have 3 starting items
    {
        "ethernet":"arch@ethernet.setup",            // arch@ethernet.setup call at the ethernet of initialize level
        "nradio":"wifi@nradio.setup",                // wifi@nradio.setup call at the nradio of initialize level
        "aradio":"wifi@aradio.setup"                 // wifi@aradio.setup call at the aradio of initialize level
    },
    "uninit":                                      // have 2 shutdown items
    {
        "nradio":"wifi@nradio.shut",                 // wifi@nradio.shut call at the nradio of initialize level
        "aradio":"wifi@aradio.shut"                  // wifi@aradio.shut call at the aradio of initialize level
    },
    "joint":                                      // have 14 joint process items
    {
        "firmware/upgrading":"arch@gpio.event",  // arch@reggpio.event call when the firmware/upgrading happened
        "firmware/upgraded":"arch@gpio.event",   // arch@reggpio.event call when the firmware/upgraded happened
        "network/arise":"arch@gpio.event",       // arch@reggpio.event call when the firmware/arise happened
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


#### **API( land@fpk )**

+ `register[ project directory [, ...] ]` **register project to system**, this API call at the system startup to register all project
    - project directory ----------- [ string ], project directory  
    - ... ------------------------- [ string ], can register many project directory 
    - failed return tfalse
    - succeed return ttrue

    Example, register a project
    ```shell
    land@fpk.register[ /usr/share/skinos/uart ]
    ttrue
    ```

+ `unregister[ <project directory> [, ...] ]` **unregister project from system**,
    - project directory ----------- [ string ], project directory  
    - ... ------------------------- [ string ], can unregister many project directory 
    - failed return tfalse
    - succeed return ttrue

    Example, unregister a project
    ```shell
    land@fpk.unregister[ /usr/share/skinos/uart ]
    ttrue
    ```

+ `list[ [project] ]` **list project infomation**
    - project ----------- [ string ], get the proejct detail when give proejct
    - return json to describes   

    Example, list ifname project infomation
    ```shell
    and@fpk.list[ifname]
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
        "path":"/usr/share/skinos/ifname/",
        "size":"74923"
    }
    ```
    Example, list all project infomation
    ```
    and@fpk.list
    {
        "agent":
        {
            "path":"/usr/share/skinos/agent/",
            "size":"155997",
            "intro":"agent for remote or cloud control",
            "version":"7.0.0",
            "author":"dimmalex@gmail.com"
        },
        "arch":
        {
            "path":"/usr/share/skinos/arch/",
            "size":"289294",
            "intro":"mtk mt7981 platform layer for skinos",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "client":
        {
            "path":"/usr/share/skinos/client/",
            "size":"66180",
            "intro":"Client management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "clock":
        {
            "path":"/usr/share/skinos/clock/",
            "size":"59199",
            "intro":"System clock management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "forward":
        {
            "path":"/usr/share/skinos/forward/",
            "size":"157035",
            "intro":"Network forward function",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "ifname":
        {
            "path":"/usr/share/skinos/ifname/",
            "size":"74923",
            "intro":"skinos common network connection",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "land":
        {
            "path":"/usr/share/skinos/land/",
            "size":"153711",
            "intro":"component infrastructure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "modem":
        {
            "path":"/usr/share/skinos/modem/",
            "size":"185095",
            "intro":"modem management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "network":
        {
            "path":"/usr/share/skinos/network/",
            "size":"479667",
            "intro":"network infrastructure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "pdriver":
        {
            "path":"/usr/share/skinos/pdriver/",
            "size":"467419",
            "intro":"Portable driver",
            "version":"6.0.0",
            "author":"dimmalex@gmail.com"
        },
        "tui":
        {
            "path":"/usr/share/skinos/tui/",
            "size":"37987",
            "intro":"Terminal user interface service",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "webs":
        {
            "path":"/usr/share/skinos/webs/",
            "size":"49283",
            "intro":"web server",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "wifi":
        {
            "path":"/usr/share/skinos/wifi/",
            "size":"108456",
            "intro":"skinos wireless configure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "wui":
        {
            "path":"/usr/share/skinos/wui/",
            "size":"20066",
            "intro":"web user interface page",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        }
    }
    ```


+ `install[ FPK file [,...] ]` **install FPK to system**
    - FPK file ----------- [ string ], FPK file  
    - ... ---------------- [ string ], can install many FPK file 
    - failed return tfalse
    - succeed return ttrue

    Example, install wui project FPK to system
    ```shell
    land@fpk.install[ wui-7.0.0-x86.fpk ]
    ttrue
    ```

+ `uninstall[ project name [,...] ]` **uninstall project from system**
    - project name ----------- [ string ], project name  
    - ... ---------------- [ string ], can uninstall many project 
    - failed return tfalse
    - succeed return ttrue

    Example, uninstall wui project from system
    ```shell
    land@fpk.uninstall[ wui ]
    ttrue
    ```



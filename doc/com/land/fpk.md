## land@fpk — FPK Package Management

### Overview

Manage SkinOS fpk projects. An fpk project is a collection of programs, components, libraries, web UI pages and resources packaged into an installation archive.
- create and delete projects in the application directory
- add components, web UI pages, objects, and lifecycle hooks to a project
- register and unregister project components with the system
- install and uninstall fpk packages from files or directories
- pack a project directory into an fpk archive file
- list installed projects and query project information

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
| **`⟨PRJ_NAME⟩`** | One project's subdirectory (same as `prj.json` → `name`) | — |
| **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** | That project's install prefix on the device | — |
| **`⟨LIB_DIR⟩`** | Global shared-library directory used for symlinks | **`PROJECT_LIB_DIR`** |
| **`⟨BIN_DIR⟩`** | Global command directory used for symlinks | **`PROJECT_BIN_DIR`** |
| **`⟨SYS_ROOT⟩`** | Running system root (for merged `rootfs/` trees, `/etc`, …) | — |

Examples such as **`land@fpk.list`** JSON fields use **`⟨PRJ_ROOT⟩/…`** so they stay valid when **`PROJECT_DIR`** is changed at build time.


### API Reference

#### Control APIs

+ `register[ path, ... ]` **register one or more project directories with the system**   
    - path, ... ----------- [ string ], one or more project directory paths to register
    - failed return tfalse
    - succeed return ttrue

    Example, register one project
    ```shell
    land@fpk.register[ /skinos/uart ]
    ttrue
    ```

+ `unregister[ name, ... ]` **unregister one or more projects from the system**   
    - name, ... ----------- [ string ], one or more project names to unregister
    - failed return tfalse
    - succeed return ttrue

    Example, unregister one project
    ```shell
    land@fpk.unregister[ uart ]
    ttrue
    ```

+ `install[ path, ... ]` **install one or more fpk packages from files or directories**   
    - path, ... ----------- [ string ], each argument may be an fpk archive file or a directory containing prj.json
    - failed return tfalse
    - succeed return ttrue
    - After files are placed, calls `land@uninit.knock[ name ]` (if replacing) then `land@init.knock[ name ]` so the package's `prj.json` init/uninit hooks run without reboot

    Example, install an fpk package
    ```shell
    land@fpk.install[ wui-7.0.0-x86.fpk ]
    ttrue
    ```

+ `uninstall[ name, ... ]` **uninstall one or more projects by name**   
    - name, ... ----------- [ string ], one or more project names to uninstall; firmware projects are skipped
    - failed return tfalse
    - succeed return ttrue
    - Before removal, calls `land@uninit.knock[ name ]` to run that project's `prj.json` uninit hooks

    Example, uninstall the wui project
    ```shell
    land@fpk.uninstall[ wui ]
    ttrue
    ```

+ `project_add[ name, intro ]` **create a new empty project**   
    - name ----------- [ string ], the project name, used as the directory name under the application directory
    - intro ---------- [ string ], optional, a short description of the project
    - failed return NULL
    - succeed return [ json ], project information including name, version, author, intro and path

    ```json
    {
        "name": "project name",                // [ string ], the project name
        "version": "project version",          // [ string ], default version
        "author": "project author",            // [ string ], set to land@fpk
        "intro": "project introduction",       // [ string ], the introduction text
        "path": "project directory path"       // [ string ], absolute path to the project directory
    }
    ```

    Example, create a new project called myapp
    ```shell
    land@fpk.project_add[ myapp, my application ]
    {
        "name":"myapp",                        # the project name
        "version":"1.0.0",                     # default version
        "author":"land@fpk",                   # set to land@fpk
        "intro":"my application",              # the introduction text
        "path":"/skinos/app/myapp"             # absolute path to the project directory
    }
    ```

+ `project_delete[ name ]` **delete a project and all its files**   
    - name ----------- [ string ], the project name to delete
    - failed return tfalse
    - succeed return ttrue

    Example, delete the project myapp
    ```shell
    land@fpk.project_delete[ myapp ]
    ttrue
    ```

+ `project_check[ name ]` **check whether a project exists and is valid**   
    - name ----------- [ string ], the project name to check
    - failed return tfalse
    - succeed return ttrue

    Example, check if myapp exists
    ```shell
    land@fpk.project_check[ myapp ]
    ttrue
    ```

+ `project_pack[ name ]` **pack a project directory into an fpk archive file**   
    - name ----------- [ string ], the project name to pack, must be under the application directory
    - failed return NULL
    - succeed return [ string ], the full path of the generated fpk file

    Example, pack the project myapp
    ```shell
    land@fpk.project_pack[ myapp ]
    /tmp/myapp-1.0.0-rk3568.fpk
    ```

+ `com_add[ project, name, intro ]` **add a new component to a project**   
    - project -------- [ string ], the project name
    - name ----------- [ string ], the component name
    - intro ---------- [ string ], optional, a short description of the component
    - failed return NULL
    - succeed return [ string ], the project directory path

    Example, add a component called sta to project myapp
    ```shell
    land@fpk.com_add[ myapp, sta, station component ]
    /skinos/app/myapp
    ```

+ `wui_add[ project, name, menu ]` **add a web UI page to a project**   
    - project -------- [ string ], the project name
    - name ----------- [ string ], the web UI page name
    - menu ----------- [ string ], the menu label displayed in the web interface
    - failed return NULL
    - succeed return [ json ], web UI information including page path and language file paths

    ```json
    {
        "en": "menu label",                    // [ string ], the English menu label
        "page": "html file path",              // [ string ], absolute path to the HTML page
        "lang":                                // [ json ], language file paths
        {
            "cn": "chinese language file path",   // [ string ], path to Chinese language JSON
            "en": "english language file path"    // [ string ], path to English language JSON
        }
    }
    ```

    Example, add a web UI page called settings to project myapp
    ```shell
    land@fpk.wui_add[ myapp, settings, Settings ]
    {
        "en":"Settings",                           # the English menu label
        "page":"/skinos/app/myapp/settings.html",  # absolute path to the HTML page
        "lang":
        {
            "cn":"/skinos/app/myapp/settings-cn.json",  # path to Chinese language JSON
            "en":"/skinos/app/myapp/settings-en.json"   # path to English language JSON
        }
    }
    ```

+ `obj_add[ project, object, origin ]` **add an object mapping to a project**   
    - project -------- [ string ], the project name
    - object --------- [ string ], the object name to register in the system
    - origin --------- [ string ], the source component name within the project
    - failed return tfalse
    - succeed return ttrue

    Example, register object station pointing to component sta in myapp
    ```shell
    land@fpk.obj_add[ myapp, station, sta ]
    ttrue
    ```

+ `init_add[ project, level, call ]` **add an init hook to a project**   
    - project -------- [ string ], the project name
    - level ---------- [ string ], the init level or stage name
    - call ----------- [ string ], the component method to call at init time
    - failed return tfalse
    - succeed return ttrue

    Example, add an init hook to myapp
    ```shell
    land@fpk.init_add[ myapp, ethernet, myapp.sta.setup ]
    ttrue
    ```

+ `uninit_add[ project, level, call ]` **add an uninit hook to a project**   
    - project -------- [ string ], the project name
    - level ---------- [ string ], the uninit level or stage name
    - call ----------- [ string ], the component method to call at uninit time
    - failed return tfalse
    - succeed return ttrue

    Example, add an uninit hook to myapp
    ```shell
    land@fpk.uninit_add[ myapp, nradio, myapp.sta.shut ]
    ttrue
    ```

+ `joint_add[ project, level, call ]` **add a joint event hook to a project**   
    - project -------- [ string ], the project name
    - level ---------- [ string ], the joint event name to subscribe to
    - call ----------- [ string ], the component method to call when the event fires
    - failed return tfalse
    - succeed return ttrue

    Example, add a joint hook for network/online event to myapp
    ```shell
    land@fpk.joint_add[ myapp, network/online, myapp.sta.event ]
    ttrue
    ```

#### Query APIs

+ `list[ project ]` **list all installed projects or get details of a specific project**   
    - project ----------- [ string ], optional, when provided, return detailed info of that project only
    - failed return NULL
    - succeed return [ json ], when no argument: a map of project name to summary; when project given: the full project info

    ```json
    {
        "project name":                          // [ string ]: { json }, project name in system
        {                                             // project summary information
            "path": "project directory path",     // [ string ], absolute path to the project directory
            "size": "directory size in bytes",    // [ number ], total size of the project directory
            "intro": "project introduction",      // [ string ], short description from prj.json
            "version": "project version",         // [ string ], version string from prj.json
            "author": "project author"            // [ string ], author from prj.json
        }
        // "...":{...}  How many projects show how many properties
    }
    ```

    Example, list all installed projects
    ```shell
    land@fpk.list
    {
        "agent":
        {
            "path":"/skinos/agent/",                     # absolute path to the project directory
            "size":155997,                               # total size of the project directory
            "intro":"agent for remote or cloud control", # short description
            "version":"7.0.0",                           # version string
            "author":"dimmalex@gmail.com"                # author
        },
        "arch":
        {
            "path":"/skinos/arch/",
            "size":289294,
            "intro":"mtk mt7981 platform layer for skinos",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        }
    }
    ```

    Example, get details of a specific project
    ```shell
    land@fpk.list[ ifname ]
    {
        "name":"ifname",                                 # the project name
        "intro":"skinos common network connection",      # short description
        "desc":"skinos ip connect and wifi connect component",  # detailed description
        "type":"root",                                   # project type
        "version":"8.0.0",                               # version string
        "author":"dimmalex@gmail.com",                   # author
        "com":
        {
            "ethcon":"ethernet connect component",       # component name and description
            "ltecon":"lte modem connect component"       # component name and description
        },
        "path":"/skinos/ifname/",                        # absolute path to the project directory
        "size":74923                                     # total size of the project directory
    }
    ```

+ `number[]` **get the total number of installed projects**   
    - failed return [ number ], returns 0 when no projects exist
    - succeed return [ number ], the count of installed projects

    Example, count installed projects
    ```shell
    land@fpk.number[]
    15
    ```

+ `wui_menu[ type ]` **get the web UI menu structure for all enabled projects**   
    - type ----------- [ string ], optional, the prj.json top-level key to scan, default is "wui"
    - failed return NULL
    - succeed return [ json ], a map of app name to web UI entry with page path, language files and display conditions

    ```json
    {
        "app_menu name":                         // [ string ]: { json }, combined key of project and menu name
        {                                             // web UI menu entry
            "menu": "menu category",              // [ string ], the menu category label
            "cn": "chinese label",                // [ string ], Chinese display name
            "en": "english label",                // [ string ], English display name
            "page": "html page path",             // [ string ], absolute path to the HTML page
            "lang":                               // [ json ], language file paths
            {
                "cn": "chinese lang path",        // [ string ], path to Chinese language file
                "en": "english lang path"         // [ string ], path to English language file
            }
        }
        // "...":{...}  How many menu entries show how many properties
    }
    ```

    Example, get the web UI menu
    ```shell
    land@fpk.wui_menu
    {
        "wifi_aclient":
        {
            "menu":"Wireless",                       # the menu category label
            "cn":"5.8G客户端",                        # Chinese display name
            "en":"5.8G Clients",                     # English display name
            "page":"/skinos/wifi/client.html",       # absolute path to the HTML page
            "lang":
            {
                "cn":"/skinos/wifi/cn",              # path to Chinese language file
                "en":"/skinos/wifi/en"               # path to English language file
            }
        },
        "wui_webs":
        {
            "menu":"System",
            "cn":"WEB服务器",
            "en":"Web Server",
            "page":"/skinos/wui/admin.html",
            "lang":
            {
                "cn":"/skinos/wui/cn",
                "en":"/skinos/wui/en"
            }
        }
    }
    ```

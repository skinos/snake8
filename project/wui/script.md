## wui@script — Shell Script Component Manager

### Overview

Manage executable shell components stored under the **wui** project internal directory and registered as HE objects in the **`script`** project namespace (`script@<name>`).
- register existing executable scripts at boot via `setup` (`prj.json` `init.app`)
- add a new script from the package `comshell` template
- delete scripts that live under the wui internal directory
- view source of any `COM_FILE_EXECUTE` object as Base64
- save Base64 content back only for scripts under the wui internal directory
- list `script@*` objects with path and discovered bash methods



### Concepts

Registered object names use project id **`script`** (not `wui`): filename `netcap` becomes **`script@netcap`**.
Scripts are regular files with owner execute permission under the wui **internal** path (`internal2path`).
`add` copies the FPK misc file **`comshell`** as the starting template (`misc2path`).
`list` parses the script text for lines that look like `name(` and reports those names under `methods`, skipping names that start with `_`.
`view` may open any registered executable component path; `save` and `delete` refuse paths outside the wui internal directory (`EPERM`).



### API Reference

#### Management APIs

+ `setup[]` **register executable scripts from the wui internal directory**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation
    - Scans the internal directory; each regular file with user-execute bit is registered as `script@<filename>`


#### Query APIs

+ `list` **list registered script objects with path and methods**
    - failed return NULL
    - succeed return [ json ], map of object name to path and methods
    ```json
    {
        "object name":                  // [ string ]: { json }, e.g. script@netcap
        {
            "path":"absolute script path",    // [ string ], on-disk executable path
            "methods":                        // [ json ], bash function names discovered in the file
            {
                "method name":""                    // [ string ]: [ string ], method name; value is empty string
                // "...":""  How many methods show how many properties
            }
        }
        // "...":{ ... }  How many objects show how many properties
    }
    ```

    Example, list shell script components
    ```shell
    wui@script.list
    {
        "script@netcap":
        {
            "path":"/mnt/skinos/internal/wui/netcap",
            "methods":
            {
                "stop":"",
                "lte":"",
                "lan":""
            }
        }
    }
    ```

+ `view[ object ]` **return Base64-encoded file content of an executable component**
    - object ----------- [ string ], HE object name, e.g. `script@netcap` or another `COM_FILE_EXECUTE` object
    - failed return tfalse
    - succeed return [ string ], Base64 of the component file bytes
    - Fails with `EINVAL` (empty object), `EPERM` when the object is a library (`COM_FILE_LIB`), or when the path cannot be opened

    Example, view a script (output truncated)
    ```shell
    wui@script.view[ script@netcap ]
    IyEvYmluL2Jhc2gKLiAkY2hlYWRlcgo...
    ```


#### Control APIs

+ `add[ name ]` **create a new script from the comshell template and register it**
    - name ------------- [ string ], filename under the wui internal directory; must not contain `/`
    - failed return tfalse
    - succeed return ttrue
    - Common failures: `EINVAL` (bad name), `EEXIST` (file already present), copy/chmod/register errors
    - On success the object is `script@<name>`

    Example, add a new script named demo
    ```shell
    wui@script.add[ demo ]
    ttrue
    ```

+ `save[ object, content ]` **write Base64-decoded content to an internal script file**
    - object ----------- [ string ], HE object name whose path is under the wui internal directory
    - content ---------- [ string ], Base64 payload of the new file body
    - failed return tfalse
    - succeed return ttrue
    - Restores execute mode `0755` after write
    - Refuses non-executable objects and paths outside the internal directory (`EPERM`)

    Example, save updated script content
    ```shell
    wui@script.save[ script@demo, IyEvYmluL2Jhc2gK ]
    ttrue
    ```

+ `delete[ object ]` **unregister and remove an internal script file**
    - object ----------- [ string ], HE object name under the wui internal directory
    - failed return tfalse
    - succeed return ttrue
    - Unregisters the object then unlinks the file
    - Refuses library objects and paths outside the internal directory (`EPERM`)

    Example, delete a script
    ```shell
    wui@script.delete[ script@demo ]
    ttrue
    ```

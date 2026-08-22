## wui@file — Temporary File Manager

### Overview

Manage regular files under a fixed temporary directory used by the Web UI and capture tools.
- directory path is `/tmp/file` (compile-time `FILE_DIR`)
- list regular files with absolute path and byte size
- add a file by copying a source path (stored name is the basename)
- delete a file by filename only (no path separators)
- create the directory at boot via `setup` (`prj.json` `init.app`)



### Concepts

Files live only under `/tmp/file`.
`add` takes a **source file path**; the stored name is that path's basename (for example `/tmp/var/capture.pcap` → `capture.pcap`).
`delete` takes a **filename** only (for example `capture.pcap`), never a path containing `/`.
Dot entries (`.`, `..`), names that start with `.`, and non-regular files are rejected or skipped.
Web upload uses `/upload` with `object=wui@file` and `api=add`; the HTTP layer passes the temporary upload path as `file`, then removes that temporary file.
Web download of a listed file uses `/download` with `object=wui@file`, `api=list`, and attribute `a=<filename>/path` so the HTTP layer resolves the string path from the list JSON.



### API Reference

#### Management APIs

+ `setup[]` **ensure the temporary file directory exists**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation
    - Fails with `ENOTDIR` when `/tmp/file` exists but is not a directory


#### Query APIs

+ `list` **list regular files under the temporary directory**
    - failed return NULL
    - succeed return [ json ], map of filename to path and size; empty `{}` when no regular files
    ```json
    {
        "file name":                    // [ string ]: { json }, filename under /tmp/file
        {
            "path":"absolute file path",      // [ string ], full path, e.g. /tmp/file/dump.log
            "size":"file size in bytes"       // [ number ], byte length, capped at INT_MAX
        }
        // "...":{ ... }  How many files show how many properties
    }
    ```

    Example, list temporary files
    ```shell
    wui@file.list
    {
        "dump.log":
        {
            "path":"/tmp/file/dump.log",
            "size":1024
        },
        "capture.pcap":
        {
            "path":"/tmp/file/capture.pcap",
            "size":10485760
        }
    }
    ```


#### Control APIs

+ `add[ file ]` **copy a source file into the temporary directory**
    - file ------------- [ string ], source file path (web upload temporary path, or any regular file)
    - failed return tfalse
    - succeed return ttrue
    - Destination name is the basename of `file`. Overwrites an existing file of the same name. Common failures: `EINVAL` (empty path, basename empty or starts with `.`), `EPERM` (path escape), `EISDIR` (source is not a regular file), `ENOTDIR` (`/tmp/file` exists but is not a directory), or errno from `stat` / `mkdir` / copy

    Example, add a file from a local path
    ```shell
    wui@file.add[ /tmp/var/capture.pcap ]
    ttrue
    ```


+ `delete[ name ]` **delete one regular file by filename**
    - name ------------- [ string ], filename only; must not contain `/`; not `.` or `..`
    - failed return tfalse
    - succeed return ttrue
    - Common failures: `EINVAL` (bad name), `EPERM` (path escape), `EISDIR` (not a regular file), or errno from `stat` / `unlink`

    Example, delete a capture file
    ```shell
    wui@file.delete[ capture.pcap ]
    ttrue
    ```

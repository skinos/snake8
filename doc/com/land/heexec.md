## heexec — Service loader after daemon fork

### Overview

Minimal loader that runs one component method in a fresh process image. The daemon uses it when `land@daemon` `exec` is `enable`: after `fork` it `exec`s `heexec` instead of calling the `.com` inside the daemon clone.
- read `OBJECT`, `API`, and `PARAM1…` from the environment (same as `execute_*` / `MAIN2COM`)
- open the component and call the method
- exit with the same codes the daemon `service_wait` already understands
- exe/ash services still `exec` their own binary; `heexec` is for `.com` library services



### Other

`heexec` is a `cmd` next to `he` / `daemon`. It is not an HE object. The daemon sets the environment and `exec`s the binary beside itself (`…/bin/heexec`), with `argv[0]` equal to the service name.

+ `OBJECT` **component object name**
    - required -------- [ string ], value of `execute_object` (for example `test@land`)
    - missing or empty → exit `EXIT_ECOM` (105)

+ `API` **method name**
    - optional -------- [ string ], value of `execute_api` (for example `service`); empty means the default empty method name

+ `PARAM_SIZE` / `PARAM1…` **method arguments**
    - PARAM_SIZE ------ [ number ], count of string slots for `execute_param`
    - PARAM1… --------- [ string ], optional, 1-based slots; unset slots are empty

    Example, run `test@land.com_sleep` with one argument (normally the daemon sets this)
    ```shell
    OBJECT=test@land API=com_sleep PARAM_SIZE=1 PARAM1=30000 heexec
    ```

Exit codes match the daemon child: `0` for heap/`NULL` talk, `EXIT_ttrue` (101), `EXIT_tfalse` (102), `EXIT_terror` (103), `EXIT_EPERM` (104), `EXIT_ECOM` (105), `EXIT_EFUNC` (106).

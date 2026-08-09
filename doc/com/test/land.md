## test@land — libskin + land component test harness

### Overview

Executable that stress-tests:

1. **libskin** public APIs across every `skin/*.c` module (one `test@land.<name>` entry per source file, plus com helpers).
2. **land components** (under `project/land/`, excluding the skin library itself): `machine`, `auth`, `component`, `fpk`, `init`/`joint`/`uninit`, `register` (as `regcom`), `service`, `syslog`, plus cmds `he`/`daemon`/`eline` smoke (`cmds`).
3. **Orchestrator** `test@land.all` for batch / stability / RSS leak / perf sweeps.

Lives in `rice/test` as exe `land` (object `test@land`). Deploy with `make obj=test` → FPK (not part of core `land` firmware).

Register suite uses dedicated namespaces (`land@regstress` with `@`, `regstress_ro` without `@`); does not write production `machine` keys via `wreg`.

Component suites are **safe-first**: read-only + scratch names only. Dangerous APIs (`reboot`, `factory`, `syslog.shut`, `daemon exit`, system `fpk.uninstall`, production `init.call`/`knock`, …) are **SKIP** with `NOTE:`.

Mismatches print as `rs FAIL:` / `cs FAIL:` / module `FAIL:` with expected vs actual. Suspected library defects are reported as `BUG:` / `NOTE:` (do not silently patch `skin/`). Dangerous system calls (`iptables` / `rmmod` / real uart open, etc.) are skipped with `SKIP:`.



### API Reference

#### Control APIs

+ `all[ workers, rounds, scenario ]` **orchestrate full test@land coverage**
    - workers -------- [ number ], optional (reserved; child suites use their own defaults)
    - rounds ---------- [ number ], optional (stability/leak iterations; omit → 3)
    - scenario -------- [ string ], optional:
      - `all` (default) — skin suites + land component suites
      - `skin` — libskin module suites only
      - `coms` — land HE component suites only (`machine`…`cmds`)
      - `stability` — repeat core subset `rounds` times
      - `leak` — loop talk/json + `machine.status`; fail if RSS grows >32MB
      - `perf` — run each suite with scenario=`perf`
      - `valgrind` — curated suites under `valgrind` memcheck (definite leaks → fail; needs `valgrind` or `VALGRIND=`)
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    test@land.all
    test@land.all[ 1, 5, coms ]
    test@land.all[ 1, 30, leak ]
    test@land.all[ 1, 5, stability ]
    ```

+ `register[ workers, rounds, scenario, slots ]` **run multi-process register API stress suite**
    - workers -------- [ number ], optional, worker processes (omit → 100, max 256)
    - rounds ---------- [ number ], optional, loops per worker (omit → 500)
    - scenario -------- [ string ], optional, `all` or one of `attach`, `rw`, `rw_mixed`, `typed`, `keys`, `lock`, `conv_s`, `conv_o`, `reg_wreg`, `crash`, `grow`, `attach_storm` (omit → `all`; alias `ro_erofs` → `reg_wreg`)
    - slots ----------- [ number ], optional, create-time slot capacity (omit → 512); heap ≈ slots×512 (grow scenario still uses a small map)
    - failed return tfalse (any expectation mismatch)
    - succeed return ttrue (all checks matched)

    Example
    ```shell
    test@land.register[ 20, 50, reg_wreg ]
    test@land.register[ 64, 100, all, 1024 ]
    ```

+ `com[ workers, rounds, scenario ]` **run full-dimension com.c API suite**
    - workers -------- [ number ], optional (omit → 64, max 256; used by forked scenarios)
    - rounds ---------- [ number ], optional (omit → 100)
    - scenario -------- [ string ], optional, `all` or one of:
      `discovery`, `open`, `call_lib`, `call_exe`, `getset`, `string`, `execute`,
      `parity`, `open_storm`, `alias_race`, `exe_pipe`, `edge`, `crash`, `stress`, `perf`
    - failed return tfalse
    - succeed return ttrue

    Example, full suite
    ```shell
    test@land.com
    ```

#### Helper APIs (com suite)

+ `com_ping[]` **return ttrue**
+ `com_echo[ s ]` **echo PARAM1 as talk string**
+ `com_blob[ n ]` **return pattern string of length n (max 1MiB generate; fd2talk caps ~647KiB)**
+ `com_sleep[ ms ]` **sleep then ttrue (crash scenario)**

#### Library API suites (edge / contract / stress / perf)

Each accepts `[ workers, rounds, scenario ]` where scenario is `all` or one of `edge`, `contract`, `stress`, `perf` (unless noted). Defaults typically workers=8, rounds=50. Return ttrue on pass, tfalse on any FAIL.

**Phase 1 — core data/path**

+ `param[...]` — `param_create` / `free` / `import` / `add*` / `string` / `talk` / size; NULL/bounds
+ `path[...]` — `obj_*` / `attr_*`; single-layer alias; illegal paths
+ `talk[...]` — json create/set/get/free; file R/W under temp path
+ `mtalk[...]` — `mxtalk.h` / `m2talk.c` full surface: edge/contract/stress/perf/leak/concurrent (1W+NR mmap)
+ `json[...]` — parse / `tree_to_string` / escape; bad JSON; large-string edges
+ `link[...]` — init/insert/delete/push/pop; light concurrency
+ `config[...]` — get/set family on scratch keys under `test@land` (not production machine)
+ `dbs[...]` — fetch/save family; temporary dbs path

**Phase 2 — component / project**

+ `register_compat[...]` — legacy `register_*` / `reg_*` wrappers; scratch ns `land@ts_compat`
+ `he2com[...]` — `string2he`, `he_free`, `he2string`, `argv2he`, `he_execute` / `string_he_execute`
+ `project[...]` — `project_path` / `list` / `check` / `var_path` / `internal_path` (read-only)
+ `log[...]` — `landlog` / `critical_warn` / macros; must not crash

**Phase 3 — utility / I/O (sandbox + SKIP)**

+ `util_encode[...]` — b64 / md5 / url / hex / mac; empty-string edges
+ `util_time[...]` — uptime / date_desc / stamp
+ `util_dirfile[...]` — temp lock_open / string2file / fd_block
+ `util_socket[...]` — self `unix_listen`/`unix_connect`; `talk2fd`/`fd2talk` loopback
+ `util_system[...]` — `random_long`; partition_* illegal args only
+ `util_signal[...]` — `signal_register` / `noprocess` install+restore
+ `util_network[...]` — readonly netdev/route queries; NULL edges (may fail, must not crash)
+ `util_shell[...]` — safe `execute`/`shell` (`true`/`echo`); SKIP iptables/ifconfig/insmod/rmmod
+ `util_uart[...]` — NULL/illegal path only; SKIP real serial open
+ `serv[...]` — skin `serv_*` / sstart/sstop/… full lifecycle on scratch `test@land.com_sleep`; scenarios: `edge|contract|lifecycle|concurrent|cross|stress|perf|stability`

#### Land component HE suites (edge / contract / stress / perf)

Safe-first HE coverage of `project/land` components (not the skin C library).

+ `machine[...]` — status/cpuinfo/meminfo/psnumber/psinfo/fsinfo/sginfo/esinfo + restart|reboot|default_block/unblock; SKIP restart/reboot/default/release/factory
+ `auth[...]` — list/md5/b64_*/encode/decode; scratch add+group+key_check+modify+match+change+check+delete; SKIP admin/setup
+ `component[...]` — `list` / `alias`; scratch `register`/`unregister` alias; SKIP system unregister
+ `fpk[...]` — list/path/number/project_check/wui_menu; scratch project_add→com/wui/obj/init/uninit/joint_add→pack→install→uninstall
+ `init[...]` — init/joint/uninit list + runtime register/unregister + persisted add/delete; SKIP setup/call/knock
+ `regcom[...]` — list + set/get string/int/boole + dump10/dump100 under `land@regstress`
+ `service[...]` — HE `land@service.*` full lifecycle/concurrent/cross/stress/perf/stability + `land@daemon` config read; SKIP daemon exit
+ `syslog[...]` — setup + path/list/list_*/mask + debug/info/warn/fault + add_/del_; SKIP shut/clear/delete
+ `cmds[...]` — HE smoke (`com_ping` / `machine.status`); `daemon flashid` note; SKIP eline interactive / daemon exit

Example
```shell
test@land.param[ 2, 5 ]
test@land.machine[ 1, 5 ]
test@land.auth[ 1, 5 ]
test@land.serv[ 2, 5 ]
test@land.service[ 2, 4 ]
test@land.all[ 1, 3, coms ]
    test@land.all[ 1, 30, leak ]
    test@land.all[ 1, 1, valgrind ]
```

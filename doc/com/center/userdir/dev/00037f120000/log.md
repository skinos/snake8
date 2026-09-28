## <username>/dev/<macid>/log — Gateway log files

### Overview

Files written by `center@log` for one registered gateway.

- Path: `{device_path}/<username>/dev/<macid>/log/` (`00037f120000` here is an example macid)
- Names: `YYYYMMDD-HHMMSS.log` only
- One file is a sequence of whole lines. Rotation opens a new timestamp file instead of renaming the current one
- Size and count come from `center@log` `log_file_size` / `log_file_max`, unless this user's account `config` sets a usable `log_file_size` (`1..1048576`) or `log_file_max` (`1..100000`)
- The directory is created on the first accepted line. The parent `dev/<macid>` directory must already exist
- userwui lists names only. Download uses `center@api.log_path`; delete uses `center@api.log_delete`

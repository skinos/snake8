# Web Script Development

This guide is a step-by-step walk-through of **Debug → Script**.
The Web UI is already open and you are signed in. Use the English UI.

A **red rectangle** in each picture marks the control or value to use.
Start from an empty **Scripts** / **Files** view and do one action at a time.
If the device already lists other rows, leave them alone. This guide only creates **`demo`**.


## Script rules

Every script file must keep this shape. Only change the functions in the middle.

```bash
#!/bin/bash
. $cheader
# functions here
cend
```

Call other HE lines with `he` (no extra wrapper):

```bash
he log.info[ "message" ]
```

Functions whose names start with `_` do **not** appear as API buttons.
`_service` is a long loop. Start it with `land@service.start`, not by clicking a button.

New scripts are stored at `/mnt/internal/wui/<name>` and registered as `script@<name>`.
Work files in this guide go under `/tmp/file/`. `/tmp` is cleared on reboot.


## 1. Open the Script page

1. On the left sidebar, find **Debug**.
   If the list is long, scroll until **Debug** is visible.
   Click **Debug** to expand it.

![Sidebar: click Debug](./web_dev_script/web_dev_script_01_debug.png)

2. Under **Debug** you should see **Syslog**, **Terminal**, **Inittab**, **Uninittab**, **Jointtab**, **Daemon**, and **Script**.
   Click **Script**.

![Sidebar: click Script](./web_dev_script/web_dev_script_02_script.png)

3. The breadcrumb becomes **Home > Debug > Script**.
   The page has two tables:

   - **Scripts** — object name, description, path, API buttons, Edit / Delete.
   - **Files** — files under `/tmp/file`, Download / Delete.

   Below Files is **Upload File** with **No File** and **Choose**.
   At the bottom of the page is **Refresh**.

![Script page: empty Scripts and Files](./web_dev_script/web_dev_script_03_page.png)


## 2. Example 1 — create, edit, run, download file and log

Goal: write `/tmp/file/demo.txt`, print one log line, click **run**, then download the file and the syslog.

### 2.1 Create the script

1. Stay on **Debug → Script**.
2. Look at the **Scripts** pager (the bar under the Scripts table).
   Click the **purple +**.
   Do not use the Files pager. Only the Scripts pager has **+**.

![Scripts pager: purple plus](./web_dev_script/web_dev_script_04_plus.png)

3. A dialog titled **Enter a script name** opens.
   The input is empty. Buttons are **Cancel** and **Confirm**.

![Dialog: Enter a script name](./web_dev_script/web_dev_script_05_name_dialog.png)

4. Click the input. Type `demo` (letters, digits, `_` or `-` only; no `/`).
5. Click **Confirm**.

![Name demo and Confirm](./web_dev_script/web_dev_script_06_name_filled.png)

6. A toast **Added successfully** appears.
   A new row **script@demo** is in **Scripts**.
   Path is `/mnt/internal/wui/demo`.
   API already shows the template methods (`setup`, `shut`, `list`).
   `_service` is hidden because the name starts with `_`.

![New row script@demo](./web_dev_script/web_dev_script_07_added.png)

### 2.2 Edit and save

1. On the **script@demo** row, **Actions** column, click the **blue pencil** (**Edit**).

![script@demo: Edit pencil](./web_dev_script/web_dev_script_08_edit.png)

2. The editor opens under the Scripts table.
   **Script Name** shows `script@demo`.
   The editor is a CodeMirror box with line numbers.
   The default template starts with `#!/bin/bash` and `. $cheader`, and ends with `cend`.
   Keep those three lines.

![Editor opened with the default template](./web_dev_script/web_dev_script_09_editor.png)

3. Select all text in the editor and replace it with:

```bash
#!/bin/bash
. $cheader

setup()
{
    creturn ttrue
}

shut()
{
    creturn ttrue
}

run()
{
    mkdir -p /tmp/file
    echo "demo_run_file_ok" > /tmp/file/demo.txt
    he log.info[ "demo_run_log_ok" ]
    creturn ttrue
}

cend
```

4. Check **Script Name** is still `script@demo`.
   Under the editor, click the dark **Save** button.

![Edited run() and Save](./web_dev_script/web_dev_script_10_save.png)

If you do not want to write the file, click **Cancel** instead. The editor closes and the file on the device is unchanged.

![Editor Cancel](./web_dev_script/web_dev_script_41_editor_cancel.png)

5. After **Save**, a toast **Saved successfully** appears.
   The editor closes.
   The **script@demo** row now has a green **run** button in **API** (plus `setup` and `shut`).

![After save: run button](./web_dev_script/web_dev_script_11_saved.png)

### 2.3 Click Run

1. On the **script@demo** row, **API** column, click the green **run** button.
2. A toast **Ran successfully: script@demo.run** appears.
   If it fails, a dialog **Failed to run: script@demo.run** is shown.

![run and Ran successfully](./web_dev_script/web_dev_script_12_run.png)

3. The Files table does **not** refresh by itself.
   Scroll to the bottom of the page and click **Refresh**.
   The whole Script page reloads.

![Files: Refresh](./web_dev_script/web_dev_script_13_refresh.png)

### 2.4 Download the file the script created

1. After **Refresh**, scroll to the **Files** table.
2. Find **demo.txt**.
   Path is `/tmp/file/demo.txt`.

![Files: demo.txt](./web_dev_script/web_dev_script_14_file.png)

3. On that row, **Actions**, click the **blue download** icon.
   The browser downloads `demo.txt`.

![Files: Download](./web_dev_script/web_dev_script_15_file_download.png)

4. Open the downloaded file. It must contain:

```text
demo_run_file_ok
```

### 2.5 Download the log

1. Left sidebar: **Debug → Syslog**.

![Sidebar: Syslog](./web_dev_script/web_dev_script_19_syslog_menu.png)

2. Breadcrumb **Home > Debug > Syslog**.
   Keep **Syslog** enabled (switch on).
   **Level** must be **Info** (or more verbose) so `he log.info` is stored.

![Syslog: Level Info](./web_dev_script/web_dev_script_20_syslog_level.png)

3. Under **Local Log** is **Log List**.
   Each row is one log file, with a **Download** button.
4. Click **Download** on the current log file.

![Syslog: Local Log Download](./web_dev_script/web_dev_script_21_syslog_download.png)

5. Open the downloaded file in a text editor.
   Search for `demo_run_log_ok`. That line must be present.


## 3. Upload and delete a file

These controls are on the same **Debug → Script** page, **Files** table.

### 3.1 Upload

1. Open **Debug → Script**.
2. Under the Files table, the row **Upload File** shows **No File** and a blue **Choose** button.

![Upload File / Choose](./web_dev_script/web_dev_script_16_upload.png)

3. Click **Choose**. Pick a file on your PC.
   The page starts the upload at once (no extra Upload button).
   An overlay **Uploading...** is shown.
4. On success: toast **Upload successfully**, and the name appears in Files.
   On failure: **Upload failed** or **File too large** (the file is bigger than 80% of free memory).

### 3.2 Download

Same control as 2.4: the **blue download** icon on the Files row.

### 3.3 Delete a file

1. In **Files**, click the **red trash** icon on that row.

![Files: Delete](./web_dev_script/web_dev_script_17_file_delete.png)

2. Confirm **Are you sure you want to delete demo.txt?**
   Click **Confirm** (not **Cancel**).

![File delete confirm](./web_dev_script/web_dev_script_18_file_delete_confirm.png)

3. Toast **Delete successfully**. The row disappears.

`/tmp/file` is emptied on reboot even if you never click Delete.


## 4. Example 2 — register setup as a boot task

Goal: `setup` writes `/tmp/file/demo.txt` and logs `demo_boot_log_ok`.
Register that `setup` on **Inittab**, reboot, then download the file and the log.

The Script page has no “register at boot” button. Use **Debug → Inittab**.

### 4.1 Change the script and save

1. **Debug → Script**.
2. On **script@demo**, click the **blue pencil** (same as 2.2).
3. Replace the editor text with:

```bash
#!/bin/bash
. $cheader

setup()
{
    mkdir -p /tmp/file
    echo "demo_boot_file_ok" > /tmp/file/demo.txt
    he log.info[ "demo_boot_log_ok" ]
    creturn ttrue
}

shut()
{
    creturn ttrue
}

cend
```

4. Click **Save**. Wait for **Saved successfully**.

You can click **setup** once now to test without reboot. After **Refresh**, **demo.txt** should appear in Files.

### 4.2 Register on Inittab

1. Left sidebar: **Debug → Inittab**.

![Sidebar: Inittab](./web_dev_script/web_dev_script_22_inittab_menu.png)

2. The table caption is **Initialize Table**.
   Columns: **Name (unique)**, **Call**, **Level**, **Actions**.
   Start from an empty table.

![Inittab: empty Initialize Table](./web_dev_script/web_dev_script_23_inittab_page.png)

3. Click the **purple +** on the Inittab pager.

![Inittab: purple plus](./web_dev_script/web_dev_script_24_inittab_plus.png)

4. The **Add Record** dialog opens.
   A note says **Fields marked with * are required**.

![Inittab: empty Add Record](./web_dev_script/web_dev_script_25_inittab_dialog.png)

5. Fill all three fields:

   | Field | Type this |
   |-------|-----------|
   | **Name (unique)** | `demo_boot` |
   | **Call** | `script@demo.setup` |
   | **Level** | `app` (choose it in the dropdown) |

   The Level list on this page is `general`, `app2`, `app`, `extern`, `local`, `manage2`, `manage`, `core2`, `core`.
   There is **no `delay`** item. **`app` is after most system services.**
   The script creates `/tmp/file` itself, so it does not depend on `wui@file`.

6. Click **Submit** (not **Cancel**).

![Inittab Add Record filled and Submit](./web_dev_script/web_dev_script_26_inittab_filled.png)

7. Toast **Added successfully**.
   A row `demo_boot` / `script@demo.setup` / `app` is in the table.

![Inittab: demo_boot row](./web_dev_script/web_dev_script_27_inittab_added.png)

To change a row later: select it, use the pager pencil or the row pencil, then **Submit** again.
To remove it: select the row, pager trash, confirm delete.

### 4.3 Reboot

1. Left sidebar: click **System** to expand it.

![Sidebar: System](./web_dev_script/web_dev_script_36_system_menu.png)

2. Under **System**, click **Device**.

![Sidebar: Device](./web_dev_script/web_dev_script_37_device_menu.png)

3. Open the **Device Manage** tab (the first tab).

![Device Manage tab](./web_dev_script/web_dev_script_38_device_manage.png)

4. In the **Run Time** row, click the yellow **Sys Reboot** button.

![Device: Sys Reboot](./web_dev_script/web_dev_script_39_reboot.png)

5. Confirm **Are you sure you want to restart?**
   Click **Confirm** (not **Cancel**).

![Reboot confirm](./web_dev_script/web_dev_script_40_reboot_confirm.png)

6. The page shows **Restarting…** and asks you to reconnect when the device is back.
7. Wait until the Web UI loads again (often 30–90 seconds on a remote device).
8. Sign in again if the login page is shown.
9. If the Web UI never returns, stop and treat the device as offline.

### 4.4 Check after boot

1. **Debug → Script**, click **Refresh**.
2. In **Files**, download **demo.txt**.
   Content must be `demo_boot_file_ok`.
3. **Debug → Syslog**, download the current log, search `demo_boot_log_ok`.


## 5. Example 3 — run setup on `network/online`

Goal: the same kind of `setup` (log + `/tmp/file/demo.txt`), registered on **Jointtab** for **network/online**.
Reboot, wait until the device is on the Internet, then download the file and the log.

If `demo_boot` is still in Inittab, delete that row first so only the event path runs.

### 5.1 Change the script and save

1. **Debug → Script** → pencil on **script@demo**.
2. Replace with:

```bash
#!/bin/bash
. $cheader

setup()
{
    mkdir -p /tmp/file
    echo "demo_online_file_ok" > /tmp/file/demo.txt
    he log.info[ "demo_online_log_ok" ]
    creturn ttrue
}

shut()
{
    creturn ttrue
}

cend
```

3. **Save**. Toast **Saved successfully**.

### 5.2 Register on Jointtab

1. **Debug → Jointtab**.

![Sidebar: Jointtab](./web_dev_script/web_dev_script_28_jointtab_menu.png)

2. Caption **Joint Table**.
   Same columns as Inittab: **Name (unique)**, **Call**, **Level**.
   Start from an empty table.

![Jointtab: empty Joint Table](./web_dev_script/web_dev_script_29_jointtab_page.png)

3. Click the **purple +**.

![Jointtab: purple plus](./web_dev_script/web_dev_script_30_jointtab_plus.png)

4. The **Add Record** dialog opens.

![Jointtab: empty Add Record](./web_dev_script/web_dev_script_31_jointtab_dialog.png)

5. Fill:

   | Field | Type this |
   |-------|-----------|
   | **Name (unique)** | `demo_online` |
   | **Call** | `script@demo.setup` |
   | **Level** | `network/online` (dropdown) |

   Level is the event name. **network/online** means the default IPv4 route is up.

6. Click **Submit**.

![Jointtab Add Record filled and Submit](./web_dev_script/web_dev_script_32_jointtab_filled.png)

7. Toast **Added successfully**.
   A row `demo_online` / `script@demo.setup` / `network/online` is in the table.

![Jointtab: demo_online row](./web_dev_script/web_dev_script_33_jointtab_added.png)

### 5.3 Reboot and wait for online

1. **System → Device → Device Manage → Sys Reboot**, confirm restart (same as 4.3).

2. After you can open the Web again, the WAN/LTE path has usually already fired `network/online`.
   Wait about 15 seconds more, then continue.
3. **Debug → Script → Refresh**.
   Download **demo.txt**. Content: `demo_online_file_ok`.
4. **Debug → Syslog**, download the log, search `demo_online_log_ok`.


## 6. Example 4 — `_service` started from boot `setup`

Goal: `_service` appends ticks to `/tmp/file/demo.txt` and logs on a timer.
`setup` starts that function as a daemon with `land@service.start`.
Register `setup` on Inittab. After reboot, **Debug → Daemon** must show the service name.

If `demo_online` is still in Jointtab, delete that row first.

### 6.1 Change the script and save

1. **Debug → Script** → pencil on **script@demo**.
2. Replace with:

```bash
#!/bin/bash
. $cheader

setup()
{
    mkdir -p /tmp/file
    echo "demo_svboot_setup_ok" > /tmp/file/demo.txt
    he log.info[ "demo_svboot_setup_ok" ]
    he 'land@service.start[ script@demo, script@demo, _service ]'
    creturn ttrue
}

shut()
{
    creturn ttrue
}

_service()
{
    i=0
    while :
    do
        i=$((i+1))
        echo "demo_svboot_tick $i" >> /tmp/file/demo.txt
        he log.info[ "demo_svboot_tick" ]
        sleep 4
    done
    creturn tfalse
}

cend
```

`land@service.start` has three arguments: **service name**, **object**, **method**.
Use `script@demo` as the service name so Daemon shows that string.
The method must be `_service` (same as the function).

3. Click **Save**.

`_service` still does **not** appear in the API column. That is expected.

![Editor with _service](./web_dev_script/web_dev_script_42_service_editor.png)

### 6.2 Register boot and reboot

1. **Debug → Inittab** → **purple +** (same as 4.2).
2. **Add Record**:

   | Field | Value |
   |-------|--------|
   | **Name (unique)** | `demo_boot` |
   | **Call** | `script@demo.setup` |
   | **Level** | `app` |

3. **Submit**.
4. **System → Device → Sys Reboot**, confirm (section 4.3).
5. Sign in again when the Web is back.

### 6.3 Check Daemon, file, and log

1. **Debug → Daemon**.

![Sidebar: Daemon](./web_dev_script/web_dev_script_34_daemon_menu.png)

2. Caption **Service Live (N)**.
   Columns: **Service Name**, **PID**, **Component**, **Interface**, **Uptime**, **Reset**.
3. Find **script@demo**.
   The table default is 20 rows. If it is not on page 1, click the pager to page 2 (or raise **Display**).
4. A running service has a numeric **PID** greater than 0.
   **Component** is `script@demo`.
   **Interface** is `_service`.

![Daemon: script@demo _service](./web_dev_script/web_dev_script_35_daemon_row.png)

5. **Debug → Script → Refresh**.
   Download **demo.txt**.
   It must start with `demo_svboot_setup_ok` and then several `demo_svboot_tick` lines.
   Wait a few seconds and download again if you only see the setup line.
6. **Debug → Syslog**, download, search `demo_svboot_tick`.


## 7. Example 5 — `_service` started from `network/online`

Same service model as example 4. `setup` is registered on Jointtab **network/online** instead of Inittab.

If `demo_boot` is still in Inittab, delete that row first.

### 7.1 Change the script and save

1. **Debug → Script** → pencil on **script@demo**.
2. Replace with:

```bash
#!/bin/bash
. $cheader

setup()
{
    mkdir -p /tmp/file
    echo "demo_svonline_setup_ok" > /tmp/file/demo.txt
    he log.info[ "demo_svonline_setup_ok" ]
    he 'land@service.start[ script@demo, script@demo, _service ]'
    creturn ttrue
}

shut()
{
    creturn ttrue
}

_service()
{
    i=0
    while :
    do
        i=$((i+1))
        echo "demo_svonline_tick $i" >> /tmp/file/demo.txt
        he log.info[ "demo_svonline_tick" ]
        sleep 4
    done
    creturn tfalse
}

cend
```

3. **Save**.

### 7.2 Register the event and reboot

1. **Debug → Jointtab** → **purple +** (same as 5.2).
2. **Add Record**:

   | Field | Value |
   |-------|--------|
   | **Name (unique)** | `demo_online` |
   | **Call** | `script@demo.setup` |
   | **Level** | `network/online` |

3. **Submit**.
4. **System → Device → Sys Reboot**, confirm.
5. Sign in after the device is online. Wait about 15 seconds.

### 7.3 Check Daemon, file, and log

1. **Debug → Daemon**.
   Find **script@demo**, **PID** > 0, **Interface** `_service` (same as 6.3).
2. **Debug → Script → Refresh**.
   Download **demo.txt**: setup line plus `demo_svonline_tick` lines.
3. **Debug → Syslog**, search `demo_svonline_tick`.


## 8. Delete the script

1. **Debug → Script**.
2. On the **script@demo** row, click the **red trash** icon.

![script@demo: Delete](./web_dev_script/web_dev_script_43_script_delete.png)

3. Confirm **Are you sure you want to delete script@demo?**
   Click **Confirm**.

![Script delete confirm](./web_dev_script/web_dev_script_44_script_delete_confirm.png)

4. Toast **Delete successfully**.
   If that script is open in the editor, the editor closes.

Deleting the script does **not** remove Inittab / Jointtab rows or a running Daemon.
Remove those first (or after) on **Inittab**, **Jointtab**, and **Daemon** as needed.


## 9. Remove boot / event / service leftovers

On the Web:

1. **Debug → Inittab** — select `demo_boot`, pager trash, confirm.
2. **Debug → Jointtab** — delete `demo_online`.
3. **Debug → Daemon** — if `script@demo` is still listed, **Reset** only restarts it.
   To drop the service record, use HE `land@service.delete[ script@demo ]` from **Debug → Terminal** or a shell `he` line.
4. **Debug → Script** — trash `script@demo` if it is still there.
5. **Files** — trash `demo.txt`, or reboot to clear `/tmp/file`.

Equivalent HE (for Terminal / `he`):

```shell
land@init.delete[ demo_boot ]
land@joint.delete[ demo_online ]
land@service.delete[ script@demo ]
wui@script.delete[ script@demo ]
```

# Testing Internet Connectivity via Telnet to a Router on Windows 11 (Example: 192.168.8.1)

This guide explains how to **enable the Telnet client** on **Windows 11**, connect to a router via `telnet 192.168.8.1` in **Windows PowerShell**, log in with **`admin` / `admin`** when the **`D218-320218 login:`** prompt appears, and run **`ping www.google.com`** at the **`$`** prompt to verify whether the router can reach the public Internet.

> **Note**: If the router management address, username, or password differ from this example, adjust accordingly per your device manual.

---

## Step 1: Enable the Telnet Client on Windows 11

Telnet is not installed by default; you need to enable the built-in **Telnet Client** feature first.

### Method A: Enable via Settings (Recommended)

1. Open **Settings** (shortcut `Win + I`).
2. Go to **Apps** -> **Optional Features** (the exact path may vary by Windows version).
3. Click **More Windows Features** (or a link to the classic Windows Features dialog).
4. In the **Windows Features** window, find and check **Telnet Client**.
5. Click **OK** and wait for installation to complete. Restart the computer if prompted.

### Method B: Open the Features Dialog via Run

1. Press `Win + R`, type **`optionalfeatures.exe`**, and press Enter.
2. Check **Telnet Client** in the list, click OK, and wait for installation.

### How to Verify Installation

In **PowerShell** or **Command Prompt**, type:

```text
telnet
```

If the command no longer reports "not recognized" and instead enters a Telnet prompt or shows brief help, the client is ready (type `quit` to exit Telnet).

---

## Step 2: Open Windows PowerShell

1. Click the **Start** button on the taskbar.
2. Type **PowerShell** or **Windows PowerShell** in the search box.
3. Click to open **Windows PowerShell** (for general Telnet usage, normal user privileges are sufficient).

---

## Step 3: Connect to the Router in PowerShell

1. Confirm your PC is connected to the router's LAN via Ethernet or Wi-Fi (this example uses **`192.168.8.1`**).
2. In PowerShell, type the following command and press **Enter**:

```text
telnet 192.168.8.1
```

3. On success, the window will switch to a **black-background character interface** (the Telnet session), which is normal and differs from the usual blue PowerShell background.
4. If there is no response or a connection error, verify: the IP is correct, the router has Telnet/remote management enabled, the firewall is not blocking it, and the PC is on the same subnet as the router.

---

## Step 4: Log In at the Prompt (D218-320218 login:)

After connecting, you should see a login prompt similar to the following (the prefix may differ by model):

```text
D218-320218 login:
```

1. After **`login:`**, type the username: **`admin`** (characters are typically visible as you type).
2. Press **Enter**.

---

## Step 5: Enter the Password and Confirm the `$` Prompt

1. A password prompt will appear, usually **`Password:`** or **`password:`**.
2. Type the password: **`admin`**.
   - **Note**: For security, **no characters are displayed on screen** while typing the password (no `*` and no plaintext). This is normal.
3. Press **Enter** when done.
4. If the credentials are correct, you will enter the router shell and the prompt will change to **`$`** (a `$` followed by a space, awaiting your command).
   - If it shows **`Login incorrect`** or loops back to `login:`, the username or password was wrong. Verify and retry.

---

## Step 6: Run Ping at the `$` Prompt

After the **`$`** prompt, type (note the space):

```text
ping www.google.com
```

Press **Enter**. The router will send probe packets to the public target. Wait several seconds to view the full output.

> On some firmware the command may require arguments, e.g. `ping -c 4 www.google.com`. If `ping www.google.com` alone produces an error, consult your model's CLI documentation. This guide uses the common BusyBox/Linux style to explain how to interpret results.

---

## Step 7: How to Interpret the Results

Text output varies across firmware, but focus on **whether replies are received** and the **packet loss rate**.

### Case A: Ping Succeeds (Internet DNS and link are generally working)

You will typically see output matching one or more of the following:

- Multiple lines of **`64 bytes from ...`** or **`bytes from www.google.com`** with **`icmp_seq=`**, **`ttl=`**, **`time=... ms`** fields.
- A summary line such as: **`X packets transmitted, X received, 0% packet loss`** (transmit and receive counts match, **packet loss is 0%**).

**Example (illustrative):**

```text
PING www.google.com (xxx.xxx.xxx.xxx): 56 data bytes
64 bytes from xxx.xxx.xxx.xxx: seq=0 ttl=52 time=25.123 ms
64 bytes from xxx.xxx.xxx.xxx: seq=1 ttl=52 time=24.891 ms
--- www.google.com ping statistics ---
4 packets transmitted, 4 packets received, 0% packet loss
```

---

### Case B: Ping Fails (Check DNS, routing, upstream network, or firewall policy)

One or more of the following typically indicates the ping did not succeed:

- Summary shows **`100% packet loss`** or **`0 received`**.
- Multiple **`Request timeout`**, **`no answer yet`**, or no **`bytes from`** reply lines.
- **`unknown host`**, **`bad address`**, **`Name or service not known`** (DNS cannot resolve `www.google.com`, also counts as a failure).
- **`Network is unreachable`** or **`Destination Host Unreachable`** (routing or upstream link issue).

**Example (illustrative):**

```text
PING www.google.com (xxx.xxx.xxx.xxx): 56 data bytes
--- www.google.com ping statistics ---
4 packets transmitted, 0 packets received, 100% packet loss
```

---

## Exiting Telnet and Common Issues

- **Exit the router shell**: type **`exit`**, or use the logout command per your device manual (some devices use **`logout`**).
- **Close the Telnet window**: you can also simply close the PowerShell/Telnet window to end the session.
- **Security note**: Telnet is **unencrypted**; use it only on trusted LANs for short troubleshooting sessions. For production environments, prefer SSH or the router's built-in web-based diagnostic tools (if available).

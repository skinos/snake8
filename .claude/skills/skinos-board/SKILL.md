---
name: skinos-board
description: |
  Switch the Snake8/landos SDK active product model (firmware model) via gBOARDID.
  Use when the user says "切换产品型号", "切换固件型号", "换板子", "切到 r600",
  "r600 的 gas", "指定客户", "gBOARDID", "make pid", "帮我把SDK切成某某型号",
  or asks to build/develop for a named product (r600, r607, d218, h721, 720f, …)
  and optional customer under that product, without necessarily naming the platform.
  Also use for host/slave local compile or test: prefer make gBOARDID=slave-… on the
  command line — do NOT make pid / edit the gBOARDID file unless the user wants a
  persistent default switch.
  After switch on swrt5: do NOT run make clean for any gBOARDID change (chip / product /
  customer). Prefer ./mkdel then make. Do NOT use for firmware version string
  gPUBLISH/gVERSION, device upgrade, or deep config/sdk customization
  (those are skinos-sdk / device-upgrade).
---

# Skinos Board / Product Model Switch

Switch the SDK **product model** (固件型号 / 产品型号) by setting `gBOARDID`.

Deeper config layers, `sdk.config`, kernel overlays: **skinos-sdk**.  
Deploy to device: **device-upgrade** (before upgrade/FPK: match device
  `land@machine.status` platform/hardware/custom to `gBOARDID`; platform s-compat
  e.g. wrt↔swrt5).

## Rule 0 — Temporary override vs persistent `make pid`

GNU make **command-line** `gBOARDID=…` already overrides the value from the `gBOARDID` file (`sinclude`). No need to change the file to `?=`.

| Goal | Do this | Touches `gBOARDID` file? |
|------|---------|---------------------------|
| One-shot / host test build (e.g. compile land on **slave** while file stays on r600) | `make obj=<name> gBOARDID=slave-x86-ubuntu2404` (or `ubuntu2004`) | **No** |
| Multi-step host session without rewriting the file | Pass `gBOARDID=slave-…` on **every** `make` / `./mkdel` that must see the board | **No** |
| User asked to **switch** default product / keep SDK on that board | `make pid gBOARDID=<…>` then `make pidinfo` | **Yes** (appends) |

**Agent default for slave / local host testing:**

- Prefer **`make … gBOARDID=slave-x86-ubuntu2404`** (or the slave id already in use on that machine).
- **Do not** run `make pid` and **do not** edit the `gBOARDID` file just to run host tests — that changes the user’s persistent board (often a device product like `swrt5-…-r600`).
- Use `make pid` only when the user explicitly wants to switch the SDK’s default board, or asks to leave the tree on slave afterward.
- To **compile / install / run / stop / uninstall** slave on this SDK server, follow **skinos-slave** (`make dep; make;` → `sdk_install` → `sdk_start`, or `make rebuild`). `make obj=` alone does not install or start the host instance.

```bash
# Good — temporary host build; file unchanged
make obj=land gBOARDID=slave-x86-ubuntu2404
./mkdel && make obj=land gBOARDID=slave-x86-ubuntu2404

# Only when user wants persistent switch
make pid gBOARDID=slave-x86-ubuntu2404
```

Note: `gBOARDID=slave-… make` (environment alone) does **not** override a file line written with `gBOARDID=` — pass it as a **make argument**. `make -e` can force env override; prefer the command-line form above.

## gBOARDID shape

```text
platform-chip-product[-customer][-oem]
```

| Field | Example | Role |
|-------|---------|------|
| platform | `swrt5` | `config/<platform>/` + top-level SDK tree |
| chip | `mt7628` | `config/<platform>/<chip>/` |
| product | `r600` | `config/<platform>/<chip>/<product>/` ← **型号** |
| customer | `gas` / `wlinkr320` / `wrt` … | optional 4th segment (= Makefile `gSCOPE`); dir under the product |
| oem | optional | 5th segment |

**Standard edition (`std`):** if the user does **not** name a customer (`gas` / `wrt` / …), that means the **标准版**. Top-level `Makefile` sets `gSCOPE` to **`std`** when the 4th segment is empty — so use a **three-segment** `gBOARDID` (do **not** append `-std`). A physical `std/` directory under the product is **not** required; config fallthrough uses the product dir itself.

---

## Rule 1 — User names only the product (no platform)

When the user says e.g. **「切到 r600」** / **「产品型号切换成 r600」** and does **not** name the platform:

### 1. Search

Scan **every** platform under `config/`:

```text
config/<platform>/<chip>/<product>/
```

Look for a directory whose name equals the requested product (case-sensitive as in tree, usually lowercase: `r600`, `r607`, `d218`, …).

- `<chip>` is the hardware directory under that platform (`mt7628`, `mt7981`, `rk3568`, …).
- **Do not** treat `config/<platform>/arch/odm/<name>/` or other non-chip paths as the product home. The product must sit at **`config/<platform>/<chip>/<product>/`**.

Collect all hits as candidates: `(platform, chip, product)`.

### 2. Pick platform when multiple hits

If more than one platform has that product, choose in this order:

1. **Platform name ends with digits** preferred (e.g. `swrt5`, `smtk2`, `smtk3`, `wrt5` over names with no trailing number).
2. Among those, **name starts with `s`** preferred (e.g. `swrt5` / `smtk2` over `wrt5`).
3. Among remaining, **larger trailing number wins** (e.g. `swrt5` beats `smtk2` because `5 > 2`).

**Example:** product `r600` exists under both `smtk2` and `swrt5` → choose **`swrt5`** (starts with `s`, trailing number larger).

If still tied (same platform rules, different chips), prefer the path that has `makefile.config` under the product dir; if still unclear, ask the user.

If **no** hit: report that the product was not found under `config/*/＜chip＞/` and list close names; do not invent a `gBOARDID`.

### 3. Switch command

After resolving `platform`, `chip`, `product`, switch with:

```bash
make pid gBOARDID=<platform>-<chip>-<product>
```

Example for r600 → `swrt5` + `mt7628` + `r600`:

```bash
make pid gBOARDID=swrt5-mt7628-r600
```

Then confirm:

```bash
make pidinfo
```

---

## Rule 2 — Product + customer (客户)

The user may further name a **customer** under that product. Customer dirs live at:

```text
config/<platform>/<chip>/<product>/<customer>/
```

Examples under `config/swrt5/mt7628/r600/`: `gas`, `wlinkr320`, `jrzh`, …  
Examples under other products: `wrt`, `pelcod`, …

When the user says e.g. **「切换到 r600 的 gas」** / **「r600 gas」**:

1. Resolve `platform` + `chip` + `product` with **Rule 1** (if platform not given).
2. Verify `config/<platform>/<chip>/<product>/<customer>/` **exists**.
3. Switch with the 4th segment:

```bash
make pid gBOARDID=<platform>-<chip>-<product>-<customer>
```

Example:

```bash
make pid gBOARDID=swrt5-mt7628-r600-gas
```

If the customer directory is **missing**, report available customer dirs under that product; do not invent the segment.

### Default = 标准版 `std`

If the user names **only** the product (no `gas` / customer):

- That **is** the standard edition → Makefile `gSCOPE=std`
- Switch with **three** segments only (Rule 1): `make pid gBOARDID=<platform>-<chip>-<product>`
- Do **not** write `…-std` into `gBOARDID` unless the user explicitly asks for a literal `-std` segment

---

## Rule 3 — Do **not** `make clean` when switching `gBOARDID` (swrt5)

For **`swrt5`**, **any** intra-platform `gBOARDID` switch does **not** need `make clean`:

| Change | Example | `make clean`? |
|--------|---------|----------------|
| Different chip | `swrt5-mt7628-r600` → `swrt5-mt7981-r607` | **No** |
| Same chip, different product | `swrt5-mt7628-r600` → `swrt5-mt7628-d218` | **No** |
| Customer / scope / OEM only | `…-r600` → `…-r600-gas` | **No** |
| Cross-platform | `smtk2-…` → `swrt5-…` | **No** (different `gSDK_DIR`) |

Why (swrt5 / OpenWrt):

- All swrt5 boards share one SDK tree `swrt5/` (`gSDK_DIR=${gTOP_DIR}/${gPLATFORM}`).
- Next build’s **`kernel_dep`** (`config/swrt5/sdk.makefile`) overwrites `swrt5/.config` from the board’s `sdk.config` and syncs the per-target `kernel.config`.
- OpenWrt keeps separate trees per arch / subtarget (e.g. `build_dir/target-mipsel_*/linux-ramips_mt7621` vs `target-aarch64_*/linux-mediatek_filogic`); chips do not share one kernel build dir.
- Top-level `make clean` → `kernel_clean` → OpenWrt `clean` is **expensive** and **not required** for a correct board switch.

Normal switch + build:

```bash
make pidinfo
make pid gBOARDID=<platform>-<chip>-<product>[-<customer>]
make pidinfo
./mkdel && make          # or plain make (dep runs ./mkdel)
```

Use `make clean` only if the **user asks** for a full wipe, or a build fails with clear **stale OpenWrt/kernel-tree** symptoms — never as a default on chip change.

Same default for other OpenWrt-style platforms in this repo (e.g. `wrt5`): do **not** auto-clean on `gBOARDID` switch. Prefer **`./mkdel`** (**skinos-sdk**).

---

## Related

- Board config layout and build: `.claude/skills/skinos-sdk/SKILL.md`
- Flash / test on device: `.claude/skills/device-upgrade/SKILL.md`

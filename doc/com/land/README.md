# landos Project Overview (Documentation Entry Point)

`landos` is a component-based gateway/device management foundation. It breaks device capabilities into components (for example `land@machine`, `land@syslog`, `land@auth`), and manages them through unified **configuration** (JSON) and **method interfaces** (APIs). During system startup, the registered tasks from your project are executed for initialization/coordination/shutdown. In day-to-day use, users interact with components mainly via the `he` tool: query/modify configurations and call component methods to perform real business actions.

This document provides:
1. A high-level overview of project functionality and the overall workflow
2. How to use the project (for new users starting from zero)
3. An index of existing Markdown documents (one-line intro + clickable links)

---

## 1. What this project does

From the structure of `prj.json`, the project is organized as:
- A set of libraries/executables/drivers/components
- A set of system-level registration and boot tasks (`init` / `uninit` / `joint`)
- A management interface accessible via `he` (the `land@*` components + the `he` command)

The main abstraction exposed by the system is the **component**:
- Component name (for example `land@machine`) as the unified management object
- Component configuration described in JSON, which can be queried and updated
- Component methods called via commands, returning a `talk_t` result (string/JSON, or return codes such as `ttrue/tfalse/terror/tpanic`)

---

## 2. Core concepts (quick mental model)

1. **Component Configuration**
   - Each component has a JSON configuration model
   - You can query or modify these configuration fields via `he`

2. **Cache / Register variables**
   - Some components (such as `init/uninit/joint`, and register-related capabilities) materialize configuration into cache files/register variables
   - This creates a time-sequence difference between “what the system reads” and “what is written in configuration” (especially across system boot)

3. **Boot lifecycle**
   - `land@init`: register startup tasks triggered at a specified boot level
   - `land@joint`: execute tasks when joint events occur
   - `land@uninit`: execute tasks during shutdown/exit stages

4. **The `he` command**
   - Query configuration, modify configuration, call component methods, and parse method JSON results
   - Supports multiple formats (full configuration, single field, merged updates, method calls, and returning selected JSON fields)
   - Works locally (terminal) and also through supported communication protocols

---

## 3. How to use the project (from 0 to 1)

### 3.1 Installation / Deployment (FPK)

When you want to deploy a project (or component capabilities) onto a device, you typically package it as an `.fpk` following `fpk.md`, then install it so the system gains the corresponding components and capabilities.

### 3.2 Automatic execution after boot (init / joint / uninit)

At device startup, the system reads the project registration information and executes initialization/coordination/shutdown tasks according to boot level and joint events.

You can view it as a scheduler for component readiness:
- `init`: attach tasks to the system scheduler at the specified stage
- `joint`: event-triggered execution (for example storage/hotplug/network changes)
- `uninit`: cleanup/stop tasks during exit

### 3.3 Manage the gateway with `he` (the most common daily entry)

Start with the `Quick start (30 seconds)` section in `he.md`, for example:
- Query full component configuration
- Query and set single fields
- Call methods and then extract sub-fields from returned JSON

Concrete examples you can look up in `he.md`:
- `land@machine` (query)
- `land@machine:name=...` (modify)
- `land@machine.status` (call a method)

---

## 4. Documentation Index (existing Markdown)

These documents are the main entry points for learning and day-to-day usage (recommended read order):

1. [`ARCHITECTURE.md`](./ARCHITECTURE.md): System overview (components, config vs runtime cache, boot lifecycle).
2. [`TERMINOLOGY.md`](./TERMINOLOGY.md): Shared glossary (talk_t, attribute paths, boot levels, return codes).
3. [`he.md`](./he.md): `he` command format, quick start, and configuration/method usage examples.
4. [`fpk.md`](./fpk.md): FPK packaging and installation concepts, and `prj.json` structure.
5. [`component.md`](./component.md): Component registration/management interfaces and usage.
6. [`machine.md`](./machine.md): Gateway basic information and `land@machine` configuration/method interfaces.
7. [`auth.md`](./auth.md): Authentication and permission configuration/method call examples.
8. [`syslog.md`](./syslog.md): Syslog configuration, log file location policy, and method descriptions (query/clear/call).
9. [`service.md`](./service.md): Service management component interfaces (start/stop/exit/status).
10. [`register.md`](./register.md): Register variable read/write and related method descriptions.
11. [`init.md`](./init.md): Startup task management (register/unregister/list) and `land@init` APIs.
12. [`joint.md`](./joint.md): Joint-event-triggered task management (`land@joint`).
13. [`uninit.md`](./uninit.md): Shutdown/exit-stage task management (`land@uninit`).

---

## 5. Suggested reading order (fastest path)

1. Read [`ARCHITECTURE.md`](./ARCHITECTURE.md) first for the global mental model
2. Read [`TERMINOLOGY.md`](./TERMINOLOGY.md) next to align the shared terms used in docs
3. Read [`he.md`](./he.md) to master the command format and return types
4. Read component documents related to your target business (for example `machine/auth/syslog/service/register`)
5. Finish with `init/joint/uninit` to understand exactly what happens during system startup stages


# Project Development Tool (prj)

## Overview

The `prj` command-line tool is an online programming utility for creating and managing Skinos projects. It provides templates and scaffolding for developing components, Web UI pages, and project configurations that integrate with the Skinos component-based architecture.

## Installation

The `prj` tool is part of the `tmptools` package. After installing `tmptools`, the `prj` command becomes available in the system PATH.

## Usage

```shell
prj <command> <project_name> [arguments...]
```

## Commands

### 1. Create a New Project

Create a new project template with a basic `prj.json` configuration file.

```shell
prj create <project_name>
```

**Parameters:**
- `project_name` - The name of the project to create (required)

**Interactive Prompts:**
- Project introduction - A brief description of the project

**Example:**
```shell
prj create myproject
# Output:
# project myproject introduction: My custom project for device management
# The project directory /app/myproject
```

**Generated Files:**
- `prj.json` - Project configuration file with default fields:
  - `name`: Project name
  - `version`: Default version (e.g., "8.0.0")
  - `author`: Set to "tmptools"
  - `intro`: User-provided introduction

---

### 2. Delete a Project

Remove an existing project and all its contents.

```shell
prj delete <project_name>
```

**Parameters:**
- `project_name` - The name of the project to delete (required)

**Warning:** This operation permanently deletes the project directory and cannot be undone.

**Example:**
```shell
prj delete myproject
```

---

### 3. Add a Shell Component

Create a shell-based component template for the project.

```shell
prj add_com <project_name> <component_name>
```

**Parameters:**
- `project_name` - The target project name (required)
- `component_name` - The name of the component to create (required)

**Behavior:**
- If the project does not exist, it will be created automatically
- Creates a `.ash` file from the `component.ash` template
- Registers the component with the system

**Example:**
```shell
prj add_com myproject sensor
# Output:
# The component file /app/myproject/sensor.ash
```

**Generated Component Structure:**
The template includes the following methods:
- `setup()` - Called during system initialization
- `shut()` - Called during system shutdown
- `service()` - Background service loop
- `list()` - Example method with parameters

---

### 4. Add a Web UI Page

Create a Web UI (WUI) page template for the project.

```shell
prj add_wui <project_name> <wui_name>
```

**Parameters:**
- `project_name` - The target project name (required)
- `wui_name` - The identifier for the WUI page (required)

**Interactive Prompts:**
- Web menu title (Chinese) - Display name in Chinese
- Web menu name (English) - Display name in English

**Behavior:**
- If the project does not exist, it will be created automatically
- Creates an HTML page from the `page.html` template
- Generates language files (`cn.json`, `en.json`)
- Updates `prj.json` with WUI configuration

**Example:**
```shell
prj add_wui myproject settings
# Output:
# web menu title(Chinese): 设置
# web menu name(English): Settings
# The web page file /app/myproject/settings.html
```

**Generated Files:**
- `<wui_name>.html` - Web page with form controls and JavaScript
- `<wui_name>-cn.json` - Chinese language strings
- `<wui_name>-en.json` - English language strings

---

### 5. Register Init Task

Register a component API to be called during system startup.

```shell
prj add_init <project_name> <init_level> <component_api>
```

**Parameters:**
- `project_name` - The target project name (required)
- `init_level` - Boot level (e.g., `app`, `general`, `network`) (required)
- `component_api` - Component API to call (e.g., `myproject@sensor.setup`) (required)

**Boot Levels:**
- `arch` - Hardware setup
- `land` - Platform setup
- `bus` - Bus setup
- `device` - Device setup
- `network` - Network setup
- `manage` - Management frame setup
- `local` - Local interface setup
- `extern` - External connection setup
- `app` - Application setup
- `app2` - Secondary app setup
- `general` - General application setup
- `delay` to `delay5` - Delayed startup

**Example:**
```shell
prj add_init myproject app myproject@sensor.setup
```

---

### 6. Register Uninit Task

Register a component API to be called during system shutdown.

```shell
prj add_uninit <project_name> <uninit_level> <component_api>
```

**Parameters:**
- `project_name` - The target project name (required)
- `uninit_level` - Shutdown level (required)
- `component_api` - Component API to call (required)

**Shutdown Levels:** (reverse order of init)
- `delay5` to `delay` - Delayed shutdown
- `general` - General shutdown
- `app2`, `app` - Application shutdown
- `extern` - External connection shutdown
- `local` - Local interface shutdown
- `manage` - Management frame shutdown
- `network` - Network shutdown
- `device` - Device shutdown
- `bus` - Bus shutdown
- `land` - Platform shutdown
- `arch` - Hardware shutdown

**Example:**
```shell
prj add_uninit myproject app myproject@sensor.shut
```

---

### 7. Register Joint Event Handler

Register a component API to be called when a system event occurs.

```shell
prj add_joint <project_name> <joint_event> <component_api>
```

**Parameters:**
- `project_name` - The target project name (required)
- `joint_event` - Event name (required)
- `component_api` - Component API to call (required)

**Common Joint Events:**

| Event | Description |
|-------|-------------|
| `machine/status` | System status change |
| `date/modify` | System date modified |
| `auth/modify` | Authentication config changed |
| `network/on` | Local interface connected (IPv4) |
| `network/off` | Local interface disconnected (IPv4) |
| `network/up` | Local interface connected (IPv6) |
| `network/down` | Local interface disconnected (IPv6) |
| `network/onextern` | External interface connected (IPv4) |
| `network/offextern` | External interface disconnected (IPv4) |
| `network/online` | Default connection established |
| `network/offline` | Default connection lost |
| `station/appear` | Client connected |
| `station/disappear` | Client disconnected |

**Example:**
```shell
prj add_joint myproject network/online myproject@sensor.online
```

---

### 8. Add Dynamic Object

Create a dynamic component that depends on an existing component.

```shell
prj add_object <project_name> <object_name> <component_name>
```

**Parameters:**
- `project_name` - The target project name (required)
- `object_name` - The object identifier (required)
- `component_name` - The underlying component to use (required)

**Example:**
```shell
prj add_object myproject mysensor sensor
```

---

### 9. Check Project Format

Validate the `prj.json` file format for a project.

```shell
prj check <project_name>
```

**Parameters:**
- `project_name` - The project to validate (required)

**Example:**
```shell
prj check myproject
```

---

### 10. Pack Project

Package the project into an FPK (Firmware Package) file for distribution.

```shell
prj pack <project_name>
```

**Parameters:**
- `project_name` - The project to package (required)

**Output:**
- Creates `<name>-<version>-<hardware>.fpk` in the temporary directory

**Restrictions:**
- Cannot pack projects located under the system project directory (`PROJECT_DIR`)
- Only projects under the application directory (`PROJECT_APP_DIR`) can be packed

**Example:**
```shell
prj pack myproject
# Output:
# The packaging is located in this /tmp/myproject-8.0.0-mt7621.fpk
```

---

## Project Directory Structure

A typical project created with `prj` has the following structure:

```
/app/<project_name>/
├── prj.json              # Project configuration
├── <component>.ash       # Shell component (if created)
├── <wui_name>.html       # Web UI page (if created)
├── <wui_name>-cn.json    # Chinese language file
└── <wui_name>-en.json    # English language file
```

## prj.json Schema

The project configuration file follows this structure:

```json
{
    "name": "project_name",
    "intro": "Project introduction",
    "desc": "Detailed description",
    "type": "root",
    "version": "8.0.0",
    "author": "author_name",
    
    "com": {
        "component_name": "Component description"
    },
    
    "obj": {
        "object_name": "underlying_component"
    },
    
    "init": {
        "boot_level": {
            "component_api": ""
        }
    },
    
    "uninit": {
        "shutdown_level": {
            "component_api": ""
        }
    },
    
    "joint": {
        "event_name": {
            "component_api": ""
        }
    },
    
    "wui": {
        "page_id": {
            "menu": "MenuCategory",
            "cn": "Chinese Title",
            "en": "English Title",
            "page": "page.html",
            "config": "project@component",
            "lang": {
                "cn": "cn.json",
                "en": "en.json"
            }
        }
    }
}
```

## Component Template (component.ash)

The shell component template provides a basic structure:

```bash
#!/bin/bash
. $cheader

setup()
{
    # Initialization code
    creturn ttrue
}

shut()
{
    # Cleanup code
    creturn ttrue
}

service()
{
    # Background service loop
    while :
    do
        # Service logic
        sleep 1
    done
    creturn tfalse
}

list()
{
    ret='{"key":"value"}'
    creturn $ret
}

cend
```

## Web UI Template (page.html)

The HTML template includes:
- Bootstrap-based responsive layout
- Form controls with i18n support
- JavaScript integration with HE commands
- Automatic configuration loading/saving

Key JavaScript variables:
- `comname` - Full component name (e.g., "myproject@sensor")
- `comcfg` - Component configuration object
- `langjson` - Language file path

## Examples

### Complete Project Creation Workflow

```shell
# 1. Create a new project
prj create myapp
# Enter: My Application

# 2. Add a shell component
prj add_com myapp controller

# 3. Add a Web UI page
prj add_wui myapp dashboard
# Enter Chinese: 仪表盘
# Enter English: Dashboard

# 4. Register startup task
prj add_init myapp app myapp@controller.setup

# 5. Register shutdown task
prj add_uninit myapp app myapp@controller.shut

# 6. Register network event handler
prj add_joint myapp network/online myapp@controller.online

# 7. Validate project
prj check myapp

# 8. Package for distribution
prj pack myapp
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| "cannot mkdir" | Check directory permissions |
| "json format error" | Validate JSON syntax in prj.json |
| "no exist" | Project does not exist; use `create` first |
| "path is under PROJECT_DIR" | Move project to PROJECT_APP_DIR before packing |
| Component not registered | Ensure component file has executable permissions |

## See Also

- [README.md](../README.md) - Unified architecture overview and project guide
- [land/fpk.md](../land/fpk.md) - FPK packaging format
- [land/component.md](../land/component.md) - Component development guide

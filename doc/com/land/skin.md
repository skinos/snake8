# libskin API Documentation

> **Readership:** This volume is for **embedded integration** (C-level APIs). Operators who only use **`he`** or a Web UI should start with `he.md` and per-component guides instead.
>
> **Distribution (no land source):** When the `land` project is shipped only as an FPK (`skinos_land` + `libskin.so`) without C sources, **this document plus the installed public headers** (`skin/*.h`, typically under the SDK `doc/dev/include/skin/` or `build/install/include/skin/` after build) are the **authoritative API contract** for writing `com` / `exe` / `cmd`. Keep this file in the SDK/docs release even if `project/land/skin/*.c` is not published.
>
> **Agent / project scaffolding:** See also `.claude/skills/skinos-project/reference-skin-api.md` (quick patterns) and `.claude/skills/skinos-project/SKILL.md`.

## Overview

**libskin** is the platform library behind Skinos components: communication, configuration, logging, services, structured mmap talk (`mxtalk` / m1·m2), unix mmap IPC (`munix`), control RPC over munix+libevent (`mcontrol`), and related facilities. It is built from `project/land` as shared library **`libskin.so`** (`prj.json` → `"lib": { "skin": ... }`) and linked by other skinos packages (`-lskin`).

**Master header:** `#include "skin.h"` pulls in, in order of dependency, `stdhead.h` (standard C/POSIX includes), `skinhead.h` (types, limits, `*_COM` constants), and `skinapi.h` (shortcuts such as `scalls`, `machine_config`). This matches the on-disk layout next to the umbrella header. For a smaller compile surface you may include only what you need (e.g. `talk.h` + `com.h`); the samples elsewhere in Markdown assume the full `skin.h` entry point unless noted.

---

## Table of Contents

1. [Core Data Types](#1-core-data-types)
2. [Communication Data Type( JSON/Talk ) Operations API (talk.h)](#2-jsontalk-operations-api-talkh)  
   - [2.0 Summary](#20-summary)
   - [2.6 Sample program (every `talk.h` function)](#26-sample-program-every-talkh-function)
3. [Parameter Structure API (param.h)](#3-parameter-structure-api-paramh)  
   - [3.0 Summary](#30-summary)
   - [3.4 Sample program (every `param.h` function)](#34-sample-program-every-paramh-function)
4. [Object and Attribute Path API (path.h)](#4-object-and-attribute-path-api-pathh)  
   - [4.0 Summary](#40-summary)
   - [4.4 Sample program (every `path.h` API)](#44-sample-program-every-pathh-api)
5. [Component Communication API (com.h)](#5-component-communication-api-comh)  
   - [5.0 Summary](#50-summary)
   - [5.6 Shell invocation context (com.h)](#56-shell-invocation-context-comh) — includes **`MAIN2API`**
   - [5.7 Sample program (every `com.h` function)](#57-sample-program-every-comh-function)
6. [Configuration of Component Management API (config.h)](#6-configuration-management-api-configh)  
   - [6.0 Summary](#60-summary)
   - [6.4 Sample program (every `config.h` function)](#64-sample-program-every-configh-function)
7. [Database of Component API (dbs.h)](#7-database-api-dbsh)  
   - [7.0 Summary](#70-summary)
   - [7.3 Sample program (every `dbs.h` function)](#73-sample-program-every-dbsh-function)
8. [Registry of Component API (register.h)](#8-registry-api-registerh)  
   - [8.0 Summary](#80-summary)
   - [8.7 Sample program (every `register.h` API)](#87-sample-program-every-registerh-api)
9. [Structured mmap talk API (mxtalk.h)](#9-structured-mmap-talk-api-mxtalkh)  
   - [9.0 Summary](#90-summary)
   - [9.8 Sample program (every `mxtalk.h` API)](#98-sample-program-every-mxtalkh-api)
10. [Unix mmap IPC API (munix.h)](#10-unix-mmap-ipc-api-munixh)  
   - [10.0 Summary](#100-summary)
   - [10.1 Endpoint](#101-endpoint)
   - [10.2 Slots](#102-slots)
   - [10.3 Client post / take](#103-client-post--take)
   - [10.4 Server take / post](#104-server-take--post)
   - [10.5 Server example (echo)](#105-server-example-echo)
   - [10.6 Client — synchronous call](#106-client--synchronous-call)
   - [10.7 Client — `select` asynchronous](#107-client--select-asynchronous)
   - [10.8 Client — libevent asynchronous](#108-client--libevent-asynchronous)
   - [10.9 KEEP vs non-KEEP](#109-keep-vs-non-keep)
   - [10.10 `MUNIX_MMAP_ONLY` vs default alloc](#1010-munix_mmap_only-vs-default-alloc)
   - [10.11 Control RPC (`mcontrol.h`)](#1011-control-rpc-mcontrolh)
11. [Logging API (log.h)](#11-logging-api-logh)  
   - [11.0 Summary](#110-summary)
12. [Service Management API (serv.h)](#12-service-management-api-servh)  
   - [12.0 Summary](#120-summary)
   - [12.6 Sample program (every `serv.h` function)](#126-sample-program-every-servh-function)
13. [Project Information API (project.h)](#13-project-information-api-projecth)  
   - [13.0 Summary](#130-summary)
   - [13.5 Sample program (every `project.h` API)](#135-sample-program-every-projecth-api)
14. [HE Command API (he2com.h)](#14-he-command-api-he2comh)  
   - [14.0 Summary](#140-summary)
   - [14.4 Sample program (every `he2com.h` function)](#144-sample-program-every-he2comh-function)
15. [Linked List API (link.h)](#15-linked-list-api-linkh)
16. [Utility Functions API (utility.h)](#16-utility-functions-api-utilityh)
17. [Skin API Macros (skinapi.h)](#17-skin-api-macros-skinapih)
18. [Predefined Component Constants (skinhead.h)](#18-predefined-component-constants-skinheadh)
19. [Complete Usage Examples](#19-complete-usage-examples)
20. [Compilation and Usage](#20-compilation-and-usage)
21. [Important Notes](#21-important-notes)
22. [Related Documents](#22-related-documents)

---

## 1. Core Data Types

### 1.1 talk_t - Communication Data Type

talk_t is the core data type for inter-component communication in SkinOS, based on JSON implementation.

**Special Return Values:**  
These are sentinel `talk_t` values (small integers cast to pointer type). The same symbol can mean slightly different things depending on whether the API is a **generic component call** or a **service-layer** API — see comments in `talk.h`.

| Constant | Value | Typical context | Meaning |
|----------|-------|-----------------|--------|
| tnull | NULL | Interface: no payload; Service: OK / idle | “No value” vs “everything fine” per API family |
| ttrue | (talk_t)2 | Both | Success (boolean yes) |
| tfalse | (talk_t)3 | Interface: failure; Service: failed, may need restart | Operation failed |
| terror | (talk_t)4 | Interface: error; Service: error, restart not required | Operation error |
| tpanic | (talk_t)5 | Both | Call/dispatch failure (bad component, missing API, etc.) |

Always read the **`com.h` / `serv.h` / `scall*`** documentation for the API you call: do not assume `tnull` is always “error” or always “success” without context.

---

## 2. Communication Data Type( JSON/Talk ) Operations API (talk.h)

### 2.0 Summary

`talk.h` is the public face of the **talk/JSON** model. It includes **`json.h`**: a JSON `talk_t` is a `json_t`, and value kinds (`JSON_STRING`, `JSON_NUMBER`, `JSON_OBJECT`, `JSON_POINTER`, …) come from the `json_value_type` enum in `json.h`.

| Layer | Idea |
|-------|------|
| **X / scalar** | `string2x`, `number2x`, `pointer2x`, `text2x` and matching `x2*` — leaves that are not JSON objects. |
| **AXP** | Name/value **pairs** (`axp_*`); one attribute string + one value (`talk_t`). |
| **JSON object** | Ordered map of AXPs (`json_*`): attach/detach, iterate `json_next` / `json_prev`, typed getters/setters, generic `json_value` / `json_set_value`. |
| **I/O** | `string2json` / `json2string`, `file2json` / `json2file`, `json_save` / `json_load` — **`json2string` is heap memory (`free`)**; **`talk_free`** for `talk_t` graph nodes from the talk API. |
| **Merge** | `json_sync` / `json_patch` return **0/1**; invalid graphs **`assert`** (see §2.3). |

Before using `axp_string()` / `x2number()` / … on an unknown node, confirm shape with **`json_check()`**, **`x_check()`**, or **`axp_value()`** / **`json_value()`**.

---

### 2.1 X Type Operations (Basic Data Types)

#### x_check
```c
boole x_check(talk_t v);
```
**Description:** Check if talk_t is an X type (string/number/pointer)
**Parameters:** v - Value to check
**Returns:** true if X type, false otherwise

#### string2x / number2x / pointer2x / text2x
```c
talk_t string2x(const char *string);
talk_t number2x(int i);
talk_t pointer2x(void *pointer);
talk_t text2x(const char *string);
```
**Description:** Create X type values
**Parameters:**
- string - String value
- i - Integer value
- pointer - Pointer value
**Returns:** Created X type talk_t

#### x2string / x2number / x2pointer / x2text
```c
const char *x2string(talk_t x);
int x2number(talk_t x);
void *x2pointer(talk_t x);
const char *x2text(talk_t x);
```
**Description:** Extract values from X type
**Returns:** Corresponding string/integer/pointer value

**Example:**
```c
talk_t str_x = string2x("hello");
const char *str = x2string(str_x);  // Get string "hello"
talk_free(str_x);

talk_t num_x = number2x(42);
int num = x2number(num_x);  // Get integer 42
talk_free(num_x);
```

### 2.2 AXP Type Operations (Attribute-Value Pairs)

#### axp_check
```c
boole axp_check(talk_t v);
```
**Description:** Check if value is an AXP type

#### axp_create
```c
talk_t axp_create(const char *attr, const char *string, talk_t v);
```
**Description:** Create an AXP object
**Parameters:**
- attr - Attribute name
- string - String value (use v when NULL)
- v - talk_t value
**Returns:** Created AXP object

#### axp_set_name / axp_set_string / axp_set_number / axp_set_pointer / axp_set_json
```c
boole axp_set_name(talk_t axp, const char *attr);
boole axp_set_string(talk_t axp, const char *string);
boole axp_set_number(talk_t axp, int i);
boole axp_set_pointer(talk_t axp, void *pointer);
boole axp_set_json(talk_t axp, talk_t json);
```
**Description:** Set various properties of AXP

#### axp_name / axp_string / axp_number / axp_pointer / axp_json
```c
const char *axp_name(talk_t axp);
const char *axp_string(talk_t axp);
int axp_number(talk_t axp);
void *axp_pointer(talk_t axp);
talk_t axp_json(talk_t axp);
```
**Description:** Get various properties from AXP

**Example:**
```c
talk_t axp = axp_create("name", "device1", NULL);
const char *name = axp_name(axp);      // Get attribute name "name"
const char *val = axp_string(axp);     // Get value "device1"
talk_free(axp);
```

### 2.3 JSON Object Operations

#### json_check
```c
boole json_check(talk_t json);
```
**Description:** Check if value is a JSON object

#### json_create
```c
talk_t json_create(talk_t axp);
```
**Description:** Create a JSON object
**Parameters:** axp - First attribute (can be NULL for empty object)
**Returns:** Created JSON object

#### json_set_string / json_set_number / json_set_pointer / json_set_json
```c
boole json_set_string(talk_t json, const char *attr, const char *string);
boole json_set_number(talk_t json, const char *attr, int i);
boole json_set_pointer(talk_t json, const char *attr, void *pointer);
boole json_set_json(talk_t json, const char *attr, talk_t v);
```
**Description:** Set attribute values in JSON object

#### json_string / json_number / json_pointer / json_json
```c
const char *json_string(talk_t json, const char *attr);
int json_number(talk_t json, const char *attr);
void *json_pointer(talk_t json, const char *attr);
talk_t json_json(talk_t json, const char *attr);
```
**Description:** Get attribute values from JSON object

#### json_cut_axp / json_delete_axp
```c
talk_t json_cut_axp(talk_t json, const char *attr);
boole json_delete_axp(talk_t json, const char *attr);
```
**Description:** Cut/Delete specified attribute from JSON

#### json_attach_axp / json_detach_axp
```c
boole json_attach_axp(talk_t json, talk_t axp);
boole json_detach_axp(talk_t json, talk_t axp);
```
**Description:** Attach/Detach AXP to/from JSON

#### json_next / json_prev
```c
talk_t json_next(talk_t json, talk_t axp);
talk_t json_prev(talk_t json, talk_t axp);
```
**Description:** Iterate through JSON object attributes

#### json_empty / json_size
```c
boole json_empty(talk_t json);
int json_size(talk_t json);
```
**Description:** Test emptiness / count top-level attributes. **`json_size` returns `-1` if `json` is not a valid JSON object** (sets `errno`); otherwise returns `>= 0`.

#### json_value / json_set_value / json_cut_value
```c
talk_t json_value(talk_t json, const char *attr);
boole json_set_value(talk_t json, const char *attr, talk_t v);
talk_t json_cut_value(talk_t json, const char *attr);
```
**Description:** Access an attribute’s value as an arbitrary `talk_t` (nested object, string/number/pointer leaf, etc.). `json_set_value` links `v` into the object; `json_cut_value` removes the value node without freeing it (caller owns it). Prefer `json_string` / `json_number` / … when you know the scalar type.

#### axp_value / axp_set_value / axp_cut_value / axp_delete_value
```c
talk_t axp_value(talk_t axp);
boole axp_set_value(talk_t axp, talk_t v);
talk_t axp_cut_value(talk_t axp);
boole axp_delete_value(talk_t axp);
```
**Description:** Same idea at the AXP level: get/set/cut/delete the value half of a name/value pair. `axp_delete_value` frees the detached value.

#### json_sync / json_patch
```c
int json_sync(talk_t src, talk_t dest);
int json_patch(talk_t src, talk_t dest);
```
**Description:** Deep-merge semantics on JSON objects. Both return **`0`** if nothing changed, **`1`** if `dest` was modified. Invalid `src`/`dest` (fails `json_check`) **abort via `assert()`** in current implementation — not a negative return code. **`json_patch`** honors patch mode from `src`: operator key `"."` / `'|'` / `'='` (`JSON_PATCH_OP`, `JSON_PATCH_SYNC`, `JSON_PATCH_MIRROR` in `talk.h`) controls merge vs mirror (remove keys absent in `src`).

**Example:**
```c
// Create JSON object
talk_t json = json_create(NULL);
json_set_string(json, "name", "router");
json_set_number(json, "port", 8080);

// Get values
const char *name = json_string(json, "name");  // "router"
int port = json_number(json, "port");          // 8080

// Iterate JSON (values may be string, number, nested object, etc. — do not use axp_string() unless the value is a string)
talk_t axp = NULL;
while ((axp = json_next(json, axp)) != NULL) {
    printf("%s = ", axp_name(axp));
    talk_t val = axp_value(axp);
    if (val == NULL) {
        printf("(null)\n");
    } else {
        talk_print(val);  /* prints representation; see talk_print in talk.h */
    }
}

talk_free(json);
```

### 2.4 Serialization/Deserialization

#### string2json / json2string
```c
talk_t string2json(const char *string);
char *json2string(talk_t json);
```
**Description:** Convert between string and JSON

#### file2json / json2file / json_save / json_load
```c
talk_t file2json(const char *path);
int json2file(talk_t json, const char *path);
boole json_save(talk_t json, const char *pathformat, ...);
talk_t json_load(const char *pathformat, ...);
```
**Description:** Read/Write JSON to/from files

**Example:**
```c
// Parse JSON from string
talk_t json = string2json("{\"name\":\"test\",\"value\":123}");

// Convert to string
char *str = json2string(json);
free(str);

// Save to file
json_save(json, "/tmp/config.json");

// Load from file
talk_t loaded = json_load("/tmp/config.json");
talk_free(json);
talk_free(loaded);
```

### 2.5 Other Operations

#### talk_dup / talk_equal / talk_free / talk_print
```c
talk_t talk_dup(talk_t json);
boole talk_equal(talk_t json, talk_t json2);
void talk_free(talk_t json);
int talk_print(talk_t json);
```
**Description:** Duplicate/Compare/Free/Print talk objects

### 2.6 Sample program (every `talk.h` function)

The program below is **educational** (not minimal production code). It calls **every function** declared in `talk.h` at least once. **`talk.h` macros** (`JSON_PATCH_OP`, `JSON_STRING_PREFIX`, `ttrue`, …) are not functions; this sample uses **`JSON_PATCH_OP`** in a comment and builds patch mode via `json_set_string(..., ".", ...)` as in the library implementation.

**Build:** compile and link against libskin (same as other examples), e.g. `#include "skin.h"` so `boole` and `memory_exit` paths resolve.

```c
/*
 * skin.md §2.6 — touches every talk.h API function once.
 * Requires: #include "skin.h" (or equivalent for boole / types).
 */
#include "skin.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int stackvar = 42;

    /* ---- X / scalars: x_check, string2x, number2x, pointer2x, text2x, x2* ---- */
    talk_t sx = string2x("abc");
    if (x_check(sx))
        (void)x2string(sx);
    talk_free(sx);

    talk_t nx = number2x(100);
    if (x_check(nx))
        (void)x2number(nx);
    talk_free(nx);

    talk_t px = pointer2x(&stackvar);
    if (x_check(px))
        (void)x2pointer(px);
    talk_free(px);

    talk_t tx = text2x("literal");
    if (x_check(tx))
        (void)x2text(tx);
    talk_free(tx);

    (void)json_check((talk_t)tnull); /* false; exercises json_check */

    /* ---- AXP: create / check / set_* / name / string / number / pointer / json ---- */
    talk_t nested_for_axp = json_create(NULL);
    json_set_string(nested_for_axp, "nk", "nv");

    talk_t ax = axp_create("label", "textval", NULL);
    if (axp_check(ax)) {
        (void)axp_name(ax);
        (void)axp_string(ax);
        axp_set_name(ax, "renamed");
        axp_set_string(ax, "s2");
        axp_set_number(ax, 10);
        (void)axp_number(ax);
        axp_set_pointer(ax, &stackvar);
        (void)axp_pointer(ax);
        axp_set_json(ax, nested_for_axp); /* links nested_for_axp */
        (void)axp_json(ax);
    }
    talk_free(ax); /* frees linked JSON */

    talk_t ax2 = axp_create("withval", NULL, number2x(5));
    talk_t vv = axp_value(ax2);
    if (vv != NULL && x_check(vv))
        (void)x2number(vv);
    talk_t cutchild = axp_cut_value(ax2);
    if (cutchild != NULL)
        talk_free(cutchild);
    axp_set_value(ax2, string2x("after"));
    axp_delete_value(ax2);
    talk_free(ax2);

    /* ---- JSON: json_create, json_check, json_set_*, json_*, cut/delete, attach/detach ---- */
    talk_t j = json_create(NULL);
    if (!json_check(j))
        return 1;

    json_set_string(j, "a", "1");
    json_set_number(j, "b", 2);
    json_set_pointer(j, "ptr", &stackvar);
    talk_t inner = json_create(NULL);
    json_set_string(inner, "in", "side");
    json_set_json(j, "inner", inner);

    (void)json_string(j, "a");
    (void)json_number(j, "b");
    (void)json_pointer(j, "ptr");
    (void)json_json(j, "inner");

    talk_t axp_a = json_axp(j, "a");
    if (axp_a != NULL && axp_check(axp_a))
        (void)axp_name(axp_a);

    talk_t cut_axp = json_cut_axp(j, "b");
    if (cut_axp != NULL)
        talk_free(cut_axp);

    json_delete_axp(j, "ptr");

    talk_t loose = axp_create("attached", "yes", NULL);
    json_attach_axp(j, loose);
    json_detach_axp(j, loose);
    json_attach_axp(j, loose);

    talk_t jnav = json_create(NULL);
    json_set_string(jnav, "first", "1");
    json_set_string(jnav, "second", "2");
    talk_t n1 = json_next(jnav, NULL);
    if (n1 != NULL) {
        talk_t n2 = json_next(jnav, n1);
        if (n2 != NULL)
            (void)json_prev(jnav, n2);
    }
    talk_free(jnav);

    (void)json_size(j);
    (void)json_size((talk_t)tnull); /* -1, EINVAL */

    talk_t jempty = json_create(NULL);
    json_set_string(jempty, "x", "y");
    (void)json_empty(jempty);
    talk_free(jempty);

    json_set_value(j, "leaf", number2x(99));
    talk_t leafcut = json_cut_value(j, "leaf");
    if (leafcut != NULL)
        talk_free(leafcut);

    talk_t subo = json_create(NULL);
    json_set_string(subo, "z", "1");
    json_set_json(j, "sub", subo);
    talk_t jv = json_value(j, "sub");
    if (jv != NULL && json_check(jv))
        (void)json_string(jv, "z");

    /* json_sync / json_patch (valid graphs only — assert on bad input) */
    talk_t src = string2json("{\"s1\":\"x\",\"s2\":2}");
    talk_t dst = string2json("{\"s1\":\"y\"}");
    (void)json_sync(src, dst);
    talk_free(src);
    talk_free(dst);

    /* Patch op key is JSON_PATCH_OP ("."); first char of value selects mirror/sync/dot behaviour */
    talk_t ps = json_create(NULL);
    json_set_string(ps, JSON_PATCH_OP, "."); /* see talk.h */
    json_set_string(ps, "k", "new");
    talk_t pd = json_create(NULL);
    json_set_string(pd, "k", "old");
    (void)json_patch(ps, pd);
    talk_free(ps);
    talk_free(pd);

    /* ---- Serialization: string2json, json2string, file2json, json2file, json_save, json_load ---- */
    talk_t sj = string2json("{\"demo\":true}");
    char *jstr = json2string(sj);
    if (jstr != NULL)
        free(jstr);

    if (json2file(sj, "/tmp/skin_talk_demo.json") < 0) { /* ignore */ }

    talk_t fj = file2json("/tmp/skin_talk_demo.json");
    if (fj != NULL)
        talk_free(fj);

    if (json_save(sj, "/tmp/skin_talk_demo_saved.json")) { /* ignore */ }

    talk_t ld = json_load("/tmp/skin_talk_demo_saved.json");
    if (ld != NULL)
        talk_free(ld);
    talk_free(sj);

    /* ---- talk_dup, talk_equal, talk_free, talk_print ---- */
    talk_t o1 = string2json("{\"q\":1}");
    talk_t o2 = talk_dup(o1);
    if (talk_equal(o1, o2))
        (void)talk_print(o2);
    talk_free(o1);
    talk_free(o2);

    talk_free(j);
    return 0;
}
```

---

## 3. Parameter Structure API (param.h)

### 3.0 Summary

`param_t` is an **ordered list of options** (at most `PARAM_OPTIONS_MAX` = 10) used when calling components (`ccall` / `scall` / …) and when driving services (`srun`, `sreset`, …). Each slot is either **text** (`PARAM_OPTION_TEXT`) or a **raw pointer** (`PARAM_OPTION_POINTER`); see `param.h`. Typical ways to build it:

| Source | Role |
|--------|------|
| `param_create("a,b,c")` | Parse a comma-separated description; embedded JSON objects are allowed as options (e.g. `eth0,{"mask":"24"},up`). |
| `param_build(json)` | Fill slots from a JSON object whose attributes are named `"1"` … `"10"`. |
| `param_found` + `param_insert*` / `param_add*` | Start from pointers or grow the list; **pointer options are not copied and are not freed by `param_free()`**. |

**Indexing:** `param_string` / `param_talk` / `param_pointer` use **1-based** serial numbers; **`-1`** selects the **last** option. **`param_insert*`** prepends; **`param_add*`** appends. **`param_shift` / `param_unshift`** move the visible window (`start` / `end`). **`param_import` / `param_import2`** merge one or two JSON objects into an existing `param_t`. **`param_combine`** returns a single readable string form (internal buffer; valid until `param_free()`). **`param_free()`** releases the structure and any owned `talk_t` / internal strings; do not use the `param_t` after that.

**Parsing rules (`param_create`):** commas split options only when **not** inside balanced `{` `}` / `[` `]` and when outside unquoted `"` segments (quote parity). Unbalanced braces/brackets/quotes → **`NULL`** with **`EINVAL`**. At most **`PARAM_OPTIONS_MAX`** comma-separated fields; if the input continues past that, the **tail is ignored** without error.

---

### 3.1 Creation and Release

#### param_create
```c
param_t param_create(const char *a);
```
**Description:** Create parameter structure from string
**Parameters:** a - Option description string, e.g. `"opt1,opt2,opt3"`, or **`NULL`** for an **empty** list (no options).
**Returns:** Non-NULL **`param_t`** on success; **`NULL`** with **`EINVAL`** if **`a`** is only whitespace after trim. Parse failures (**unbalanced** `{}` / `[]` / quotes) → **`NULL`**, **`EINVAL`**. Allocation failures call **`memory_exit`**.

#### param_build
```c
param_t param_build(talk_t json);
```
**Description:** Create parameter structure from JSON

#### param_found
```c
param_t param_found(void *pointer, void *pointer2, void *pointer3, void *pointer4);
```
**Description:** Create parameter structure from pointers

#### param_free
```c
void param_free(param_t parameter);
```
**Description:** Free parameter structure

### 3.2 Parameter Operations

#### param_inserts / param_insertt / param_insertp
```c
param_t param_inserts(param_t param, const char *option);
param_t param_insertt(param_t param, const talk_t option);
param_t param_insertp(param_t param, void *option);
```
**Description:** Insert options at the head of parameters

#### param_adds / param_addt / param_addp
```c
param_t param_adds(param_t param, const char *option);
param_t param_addt(param_t param, const talk_t option);
param_t param_addp(param_t param, void *option);
```
**Description:** Add options at the end of parameters

#### param_shift / param_unshift
```c
boole param_shift(param_t parameter, int number);
boole param_unshift(param_t parameter, int number);
```
**Description:** Adjust parameter start position

#### param_import / param_import2
```c
param_t param_import(param_t parameter, talk_t json);
param_t param_import2(param_t parameter, talk_t json, talk_t json2);
```
**Description:** Import JSON into parameters

### 3.3 Get Parameters

#### param_size
```c
int param_size(param_t parameter);
```
**Description:** Get parameter count

#### param_string / param_number / param_talk / param_pointer
```c
const char *param_string(param_t parameter, int serial);
int         param_number(param_t parameter, int serial);
talk_t      param_talk(param_t parameter, int serial);
void       *param_pointer(param_t parameter, int serial);
```
**Description:** Get parameter at **1-based** `serial` (`-1` = last). **`param_number`** parses the option as an integer (`atoi`-style); returns **0** and may set **`errno`** if missing/invalid.

#### param_combine
```c
const char *param_combine(param_t parameter);
```
**Description:** Get string representation of parameters

**Example:**
```c
// Create parameters
param_t p = param_create("eth0,192.168.1.1,24");

// Get parameters
const char *ifname = param_string(p, 1);  // "eth0"
const char *ip = param_string(p, 2);      // "192.168.1.1"
int prefix = param_number(p, 3);          // 24

// Add parameter
p = param_adds(p, "gateway");

param_free(p);
```

### 3.4 Sample program (every `param.h` function)

Below, each **`param_*` function** appears at least once. **`param_import` / `param_import2` take ownership of the passed `talk_t` JSON** (do not `talk_free` them afterward). Pointer options from **`param_found` / `param_insertp` / `param_addp`** are not freed by **`param_free()`**.

```c
#include "skin.h"

static void demo_param_all(void)
{
    int stack = 42;
    talk_t jbuild = string2json("{\"1\":\"opt1\",\"2\":\"opt2\"}");
    talk_t jimp = string2json("{\"z\":1}");
    talk_t j2a = string2json("{\"a\":1}");
    talk_t j2b = string2json("{\"b\":2}");
    talk_t jt = string2x("embedded");
    param_t p;

    p = param_create("a,b,c");
    param_free(p);

    p = param_build(jbuild);
    talk_free(jbuild);
    (void)param_size(p);
    (void)param_string(p, 1);
    (void)param_string(p, -1);
    (void)param_number(p, 1);
    (void)param_talk(p, 1);
    (void)param_combine(p);
    param_free(p);

    p = param_found((void *)&stack, NULL, NULL, NULL);
    p = param_inserts(p, "head");
    p = param_insertt(p, jt);
    talk_free(jt);
    p = param_insertp(p, (void *)0x1234);
    p = param_adds(p, "tail");
    p = param_addt(p, string2json("{\"k\":1}"));
    p = param_addp(p, (void *)0x5678);
    (void)param_shift(p, 1);
    (void)param_unshift(p, 1);
    p = param_import(p, jimp);
    p = param_import2(p, j2a, j2b);
    (void)param_pointer(p, 1);
    (void)param_combine(p);
    param_free(p);
}
```

---

## 4. Object and Attribute Path API (path.h)

### 4.0 Summary

`path.h` defines how **components** and **configuration paths** are represented and navigated.

| Type | Role |
|------|------|
| **`obj_t`** | Parsed **component path**. Parser stores up to **`OBJ_MAX_LEVEL`** = **2** segments (`project@component`). A **single** registered alias is resolved via **`com_path()`** and normalized to two layers. Holds project/component strings, optional `com` / register handles, and buffers; **`obj_create` / `obj_free`**. |
| **`attr_t`** | Parsed **attribute path** inside config or JSON (e.g. `wan/ip`, multi-level with `OBJECT_CONFIG_GAPS` / `/`). **`attr_create` / `attr_free`**, `attr_layer`, `attr_combine`. |
| **JSON helpers** | **`attr_get` / `attr_set` / `attr_cut`** (and `*s` / `*_string` variants) walk a **`talk_t` JSON tree** by attribute path — same logical layout as `config_get` paths. |

Returned `const char *` views from `obj_*` / `attr_*` point into internal buffers; they are invalid after the corresponding **`obj_free` / `attr_free`**.

---

### 4.1 Object Operations (obj_t)

#### obj_create
```c
obj_t obj_create(const char *string);
```
**Description:** Create object from string
**Parameters:** string - Usually `project@component`, or a **single** component alias that **`com_register`** maps to a real path (resolved to project + component).
**Returns:** Object pointer, or **NULL** on invalid input / resolution failure (`errno` set).
**Note:** After successful **`obj_create`**, **`obj_level()`** is **2** and **`obj_prj()`** / **`obj_com()`** are typically both non-NULL.

#### obj_free
```c
void obj_free(obj_t object);
```
**Description:** Free object

#### obj_prj / obj_com / obj_level / obj_layer / obj_name
```c
const char *obj_prj(obj_t object);
const char *obj_com(obj_t object);
int obj_level(obj_t object);
const char *obj_layer(obj_t object, int level);
const char *obj_name(obj_t object);
```
**Description:** Get project name, component name, level, and other info

**Example:**
```c
obj_t o = obj_create("land@machine");
const char *prj = obj_prj(o);    // "land"
const char *com = obj_com(o);    // "machine"
obj_free(o);
```

### 4.2 Attribute Operations (attr_t)

#### attr_create
```c
attr_t attr_create(const char *attribute);
```
**Description:** Create attribute path
**Parameters:** attribute - Attribute path, e.g., "wan/ip" or "config/gateway"
**Returns:** Attribute pointer

#### attr_free
```c
void attr_free(attr_t attribute);
```
**Description:** Free attribute

#### attr_level / attr_layer / attr_combine
```c
int attr_level(attr_t attribute);
const char *attr_layer(attr_t attribute, int level);
const char *attr_combine(attr_t attribute);
```
**Description:** Get attribute level information

### 4.3 Attribute Access

#### attr_get / attr_gets / attr_get_string / attr_gets_string
```c
talk_t attr_get(talk_t json, attr_t attribute);
talk_t attr_gets(talk_t json, const char *attribute, ...);
const char *attr_get_string(char *buffer, int buflen, talk_t json, attr_t attribute);
const char *attr_gets_string(char *buffer, int buflen, talk_t json, const char *attribute, ...);
```
**Description:** Get value from JSON according to attribute path

#### attr_cut / attr_cuts
```c
talk_t attr_cut(talk_t json, attr_t attribute);
talk_t attr_cuts(talk_t json, const char *attribute, ...);
```
**Description:** Cut value from JSON at specified path

#### attr_set / attr_sets / attr_set_string / attr_sets_string
```c
boole attr_set(talk_t json, talk_t v, attr_t attribute);
boole attr_sets(talk_t json, talk_t v, const char *attribute, ...);
boole attr_set_string(talk_t json, const char *v, attr_t attribute);
boole attr_sets_string(talk_t json, const char *v, const char *attribute, ...);
```
**Description:** Set JSON value according to attribute path

**Example:**
```c
// Create attribute path
attr_t a = attr_create("network/wan/ip");

// Set value in JSON
talk_t json = json_create(NULL);
attr_set_string(json, "192.168.1.1", a);

// Get value
const char *ip = attr_get_string(buf, sizeof(buf), json, a);

attr_free(a);
talk_free(json);
```

### 4.4 Sample program (every `path.h` API)

Covers **`obj_*`**, **`attr_*`**, and JSON helpers **`attr_get*` / `attr_set*` / `attr_cut*`** (including `*s` / `*_string` forms). Requires a running SkinOS stack only insofar as **`json_*`** / **`string2json`** are available via `skin.h`.

```c
#include "skin.h"

static void demo_path_all(void)
{
    char buf[256];
    obj_t o = obj_create("land@machine");
    (void)obj_prj(o);
    (void)obj_com(o);
    (void)obj_level(o);
    (void)obj_layer(o, 0);
    (void)obj_name(o);

    attr_t a = attr_create("wan/ip");
    (void)attr_level(a);
    (void)attr_layer(a, 0);
    (void)attr_combine(a);

    talk_t j = json_create(NULL);
    (void)attr_set_string(j, "192.168.1.1", a);
    (void)attr_get_string(buf, sizeof buf, j, a);
    (void)attr_get(j, a);
    (void)attr_gets(j, "%s", "wan/ip");
    (void)attr_gets_string(buf, sizeof buf, j, "%s", "wan/ip");
    (void)attr_set(j, string2x("v"), a);
    (void)attr_sets(j, string2x("v2"), "%s", "wan/ip");
    (void)attr_sets_string(j, "text", "%s", "wan/ip");

    talk_t j2 = string2json("{\"a\":{\"b\":\"c\"}}");
    attr_t ab = attr_create("a/b");
    talk_t cut = attr_cut(j2, ab);
    if (cut > (void *)tpanic && cut && json_check(cut))
        talk_free(cut);
    talk_t j3 = string2json("{\"x\":{\"y\":1}}");
    talk_t cx = attr_cuts(j3, "%s", "x");
    if (cx > (void *)tpanic && cx && json_check(cx))
        talk_free(cx);

    attr_free(ab);
    attr_free(a);
    talk_free(j3);
    talk_free(j2);
    talk_free(j);
    obj_free(o);
}
```

---

## 5. Component Communication API (com.h)

### 5.0 Summary

`com.h` is the **dynamic component loader and RPC layer**: open a component module (`.com` / `.ash` / ko / exe), resolve **`_api`** symbols, and invoke them with **`param_t`** or raw **`talk_t`**.

| Topic | Details |
|-------|---------|
| **Types** | `com_t` handle; **`COM_FILE_LIB` / `COM_FILE_EXECUTE`**; typedefs **`comget_t` / `comset_t` / `comfetch_t` / `comsave_t` / `comapi_t`** / **`eapi_table_t`**. |
| **Discovery** | **`com_list(prefix, project)`**, **`com_register(object, origin, type)`**, **`com_unregister`**, **`com_path(object, buf, len)`** → type code in return value, path in buffer. |
| **Open / symbol** | **`com_open(const char *object)`** only (there is **no** `com_sopen`). **`com_symbol`**, **`com_close`**. Symbol names use **`COM_API_PREFIX`** (`_`), e.g. `_status`. |
| **Existence** | **`com_exist(com_t, api)`** on an open handle; **`com_have(object, api)`** by string path (opens briefly). There is **no** `com_sexist`. |
| **Calls** | **`ccall` / `scall`** + variants: `*t` (JSON), `*s` (format string), `*st`, `*4p`, `*_string`. Results are **`void *`**: JSON (**`talk_free`**) or sentinels — see §1.1. |
| **Runtime get/set** | **`cget` / `sget` / `cset` / `sset`** (+ `*s` / `*_string`). Persist DB is **`dbs_*`** in `dbs.h` — **not** `cfetch`/`csave` (those names are obsolete; do not use). |
| **Exe entry** | **`MAIN2API(table)`** + **`execute_object` / `execute_param` / `execute_api` / `execute_pipe`** (§5.6). |

> **Synced with `project/land/skin/com.h`:** older docs mentioned `com_sopen`, `com_sexist`, `com_project_list`, `com_register_list`, `cfetch`/`csave`. Those symbols are **not** in the public headers — use the APIs in this table instead.

---

### 5.1 Component file kinds (`com_t`)

```c
#define COM_FILE_LIB      1    /* shared library component */
#define COM_FILE_EXECUTE  2    /* executable; reply pipe uses SHELL_COM_PIPE (7) */
#define COMPONENT_MAX     200
```

### 5.2 Component Discovery and Registration

#### com_list
```c
talk_t com_list(const char *prefix, const char *project);
```
**Description:** List components. `prefix` filters driver-style prefixes (e.g. `"usbdrv"`); `project` filters by project id. Either may be **NULL** for “all”.
**Returns:** JSON list — caller **`talk_free`**.

#### com_register / com_unregister
```c
boole com_register(const char *object, const char *origin, char type);
boole com_unregister(const char *object);
```
**Description:** Register/unregister an object name. `type` is **`COM_FILE_LIB`** or **`COM_FILE_EXECUTE`**. `origin` is the backing path/object.

#### com_path
```c
char com_path(const char *object, char *buffer, int buflen);
```
**Description:** Resolve component file path into `buffer`.
**Returns:** **`COM_FILE_LIB`**, **`COM_FILE_EXECUTE`**, **0** if not found, or negative on error.

**Example:**
```c
talk_t list = com_list(NULL, NULL);
talk_free(list);

/* type 0 when origin is another component; COM_FILE_* when origin is a file path */
com_register("myproj@alias", "myproj@real", 0);

char path[PATH_MAX];
char type = com_path("land@machine", path, sizeof(path));
```

### 5.3 Component Open and Close

#### com_open
```c
com_t com_open(const char *object);
```
**Description:** Open/load component by `"project@component"` string. Call **`com_close`** when done.

#### com_symbol
```c
void *com_symbol(com_t com, const char *name);
```
**Description:** Resolve a symbol (usually `"_status"`, `"_setup"`, …).

#### com_close
```c
void com_close(com_t com);
```

#### com_exist / com_have
```c
boole com_exist(com_t com, const char *api);           /* api may be NULL = component only */
boole com_have(const char *object, const char *api); /* string path; opens then checks */
```

**Example:**
```c
com_t c = com_open("land@machine");
if (c) {
    void *fn = com_symbol(c, "_status");
    (void)com_exist(c, "status");
    com_close(c);
}
if (com_have("land@machine", "status")) {
    /* API present */
}
```

### 5.4 Component Calls

#### ccall / scall
```c
void *ccall(obj_t com, const char *api, param_t parameter);
void *scall(const char *com, const char *api, param_t parameter);
```
**Description:** Call component API
**Parameters:**
- com - Component object or string
- api - API name
- parameter - Parameters (can be NULL)
**Returns:**
- JSON object - Success with data
- ttrue - Operation successful
- tfalse - Operation failed
- terror - Operation error
- tpanic - Call error

#### ccallt / scallt / ccall2t / scall2t
```c
void *ccallt(obj_t com, const char *api, talk_t json);
void *scallt(const char *com, const char *api, talk_t json);
void *ccall2t(obj_t com, const char *api, talk_t json, talk_t json2);
void *scall2t(const char *com, const char *api, talk_t json, talk_t json2);
```
**Description:** Call component API with JSON parameters

#### ccallst / scallst
```c
void *ccallst(obj_t com, const char *api, const char *json, talk_t json2);
void *scallst(const char *com, const char *api, const char *json, talk_t json2);
```
**Description:** Call with string+JSON parameters

#### ccall4p / scall4p
```c
void *ccall4p(obj_t obj, const char *api, void *pointer, void *pointer2, void *pointer3, void *pointer4);
void *scall4p(const char *obj, const char *api, void *pointer, void *pointer2, void *pointer3, void *pointer4);
```
**Description:** Call with pointer parameters

#### ccalls / scalls / ccall2s / scall2s
```c
void *ccalls(obj_t com, const char *api, const char *paramformat, ...);
void *scalls(const char *com, const char *api, const char *paramformat, ...);
void *ccall2s(obj_t com, const char *api, const char *option, const char *option2);
void *scall2s(const char *com, const char *api, const char *option, const char *option2);
```
**Description:** Call with formatted string parameters

**Example:**
```c
// Basic call
param_t p = param_create("eth0");
void *r = scall("network@frame", "info", p);
if (r > (void *)tpanic && json_check((talk_t)r)) {
    talk_free((talk_t)r);
}
param_free(p);

// Using string parameters
void *r2 = scalls("network@frame", "set", "%s,%s", "ip", "192.168.1.1");

// Using JSON parameters
talk_t json = string2json("{\"enable\":1}");
void *r3 = scallt("land@machine", "config", json);
talk_free(json);
```

### 5.5 String Result Calls

#### ccall_string / scall_string / ccallt_string / scallt_string / ccalls_string / scalls_string
```c
const char *ccall_string(char *buffer, int buflen, obj_t com, const char *api, param_t parameter);
const char *scall_string(char *buffer, int buflen, const char *com, const char *api, param_t parameter);
const char *ccallt_string(char *buffer, int buflen, obj_t com, const char *api, talk_t json);
const char *scallt_string(char *buffer, int buflen, const char *com, const char *api, talk_t json);
const char *ccalls_string(char *buffer, int buflen, obj_t com, const char *api, const char *paramformat, ...);
const char *scalls_string(char *buffer, int buflen, const char *com, const char *api, const char *paramformat, ...);
```
**Description:** Call component API and get string result

**Example:**
```c
char buf[256];
param_t p = param_create("");
const char *ver = scall_string(buf, sizeof(buf), "land@machine", "version", p);
param_free(p);
```

### 5.6 Shell invocation context (`com.h`)

When a **`COM_FILE_EXECUTE`** component runs as a **child of the shell RPC path**, the parent sets environment variables (**`OBJECT`**, **`PARAM_SIZE`**, **`PARAM1`…`N`**, **`API`**, **`cpipe`**). The child reads them via:

#### execute_object / execute_param / execute_api / execute_pipe
```c
obj_t       execute_object(void);
param_t     execute_param(void);
const char *execute_api(void);
int         execute_pipe(void);
```
**Description:** **`execute_object`** builds **`obj_t`** from **`OBJECT`**. **`execute_param`** builds **`param_t`** from **`PARAM_SIZE`** and **`PARAM1`…`PARAMn`** (via **`param_adds`**; missing env vars become NULL string slots). It returns **NULL** only on allocation failure — an empty call still yields a non-NULL **`param_t`** with zero options when **`PARAM_SIZE`** is absent or zero. **`execute_api`** returns **`getenv("API")`**. **`execute_pipe`** returns **`atoi(cpipe)`** or **`-1`** if unset — this is the fd where **`talk2fd`** should write the JSON reply (**`SHELL_COM_PIPE`** = 7 after **`dup2`** in the child).

**Outside** that child context, **`execute_object`** / **`execute_api`** typically see **NULL** env vars and return **NULL**; **`execute_pipe`** is **`-1`**.

#### MAIN2API / eapi_table_t
```c
typedef struct eapi_table_st {
    const char *name;
    comapi_t    fn;   /* void *(*)(obj_t, param_t) */
} eapi_table_t;

#define MAIN2API(table)  /* expands to int main(argc, argv) */
```
**Description:** Macro that generates **`main()`** for a **`COM_FILE_EXECUTE`** binary. Prefer an **array** of **`eapi_table_t`** (so `sizeof(table)/sizeof(table[0])` works).

**Dispatch order:**
1. If **`execute_object()`** succeeds → shell-spawned context: use **`execute_param` / `execute_api` / `execute_pipe`**, reply with **`talk2fd`**.
2. Else → CLI mode: **`argv2he(argc, argv)`** (`he2com.h`), print result to stdout, then **`he_free`**.

> Prefer **`execute_*`** in the shell-spawned path; CLI fallback uses **`argv2he`**.

**Minimal sketch:**
```c
static void *api_status(obj_t o, param_t p) { (void)o; (void)p; return ttrue; }
static const eapi_table_t g_api[] = {
    { "status", api_status },
};
MAIN2API(g_api)
```

### 5.7 Sample program (every `com.h` function)

Each **`com.h` entry point** below is invoked once. **Real components and a running SkinOS** are required for meaningful results; off-box builds may get **`tpanic` / `NULL` / `false`** — the point is to show **names, argument shapes, and typical `talk_free` handling** for heap JSON returns.

```c
#ifndef PROJECT_ID
#define PROJECT_ID "land"
#endif
#include "skin.h"

static void demo_com_all(void)
{
    char cbuf[1024];
    obj_t o = obj_create("land@machine");
    attr_t ak = attr_create("walk/key");
    attr_t fad = attr_create("/var/tmp/skin_walkthrough.db");
    param_t pm = param_create("a");
    talk_t j1 = string2json("{\"a\":1}");
    talk_t j2 = string2json("{\"b\":2}");
    int x = 1, y = 2, z = 3, w = 4;
    void *rv;
    com_t ch;
    talk_t tl;

    (void)execute_object();
    (void)execute_param();
    (void)execute_api();
    (void)execute_pipe();

    (void)com_register("demo_alias@com", "land@machine", 0); /* type 0 when origin is another component */
    (void)com_unregister("demo_alias@com");
    (void)com_path("land@machine", cbuf, sizeof cbuf);
    tl = com_list(NULL, NULL);
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);

    ch = com_open("land@machine");
    if (ch) {
        (void)com_symbol(ch, "_status");
        (void)com_exist(ch, "status");
        com_close(ch);
    }
    (void)com_have("land@machine", "status");

    rv = ccall(o, "status", pm);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall("land@machine", "status", pm);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccallt(o, "status", j1);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scallt("land@machine", "status", j1);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccallst(o, "status", "{}", j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scallst("land@machine", "status", "{}", j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccall2t(o, "status", j1, j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall2t("land@machine", "status", j1, j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccall4p(o, "status", &x, &y, &z, &w);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall4p("land@machine", "status", &x, &y, &z, &w);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccalls(o, "status", "%s,%s", "a", "b");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scalls("land@machine", "status", "%s,%s", "a", "b");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccall2s(o, "status", "o1", "o2");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall2s("land@machine", "status", "o1", "o2");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    (void)ccall_string(cbuf, sizeof cbuf, o, "status", pm);
    (void)scall_string(cbuf, sizeof cbuf, "land@machine", "status", pm);
    (void)ccallt_string(cbuf, sizeof cbuf, o, "status", j1);
    (void)scallt_string(cbuf, sizeof cbuf, "land@machine", "status", j1);
    (void)ccalls_string(cbuf, sizeof cbuf, o, "status", "%s", "a");
    (void)scalls_string(cbuf, sizeof cbuf, "land@machine", "status", "%s", "a");

    (void)cset(o, j1, ak);
    (void)csets(o, j1, "%s", "walk/key");
    (void)sset("land@machine", j1, ak);
    (void)ssets("land@machine", j1, "%s", "walk/key");
    (void)cset_string(o, "s", ak);
    (void)csets_string(o, "s", "%s", "walk/key");
    (void)sset_string("land@machine", "s", ak);
    (void)ssets_string("land@machine", "s", "%s", "walk/key");

    rv = cget(o, ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = cgets(o, "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sget("land@machine", ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sgets("land@machine", "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    (void)cget_string(cbuf, sizeof cbuf, o, ak);
    (void)cgets_string(cbuf, sizeof cbuf, o, "%s", "walk/key");
    (void)sget_string(cbuf, sizeof cbuf, "land@machine", ak);
    (void)sgets_string(cbuf, sizeof cbuf, "land@machine", "%s", "walk/key");

    /* Persistent DB: use dbs_* in dbs.h (§7). There is no cfetch/csave in com.h. */

    talk_free(j1);
    talk_free(j2);
    param_free(pm);
    attr_free(fad);
    attr_free(ak);
    obj_free(o);
}
```

---

## 6. Configuration Management API (config.h)

### 6.0 Summary

`config.h` reads and writes **runtime configuration** for a component (**not** the same persistence path as `dbs.h`).

| Pattern | Meaning |
|---------|---------|
| **`config_get` / `config_set`** | `obj_t` + **`attr_t`** attribute path. |
| **`*gets` / `*sets` / `*sget` / `*sset`…** | `printf`-style attribute paths or **`const char *com`** instead of `obj_t`. |
| **`*_string`** | Read/write through a caller **`buffer`** / C string without owning a new `talk_t`. |
| **Returns** | **`config_get*`** often returns heap **`talk_t`** → **`talk_free`**; **`tpanic`** on dispatch errors per comments. **`config_set*`** copies **`v`**; caller still owns original `v` unless documented otherwise. On **`config_set*`** success: **`errno == EEXIST`** if content unchanged (no write); **`errno == 0`** if content was written. |
| **List / files** | **`config_list(project)`** — `NULL` project = whole system; **`config_path`** builds paths under a project. |

Use **`dbs_*`** when you need **per-component persistent database files** (`fa` + key), not live config only.

---

### 6.1 Configuration Get

#### config_get / config_gets / config_sget / config_sgets
```c
talk_t config_get(obj_t com, attr_t attr);
talk_t config_gets(obj_t com, const char *attr, ...);
talk_t config_sget(const char *com, attr_t attr);
talk_t config_sgets(const char *com, const char *attr, ...);
```
**Description:** Get configuration value
**Returns:** talk_t value or special return value

#### config_get_string / config_gets_string / config_sgets_string
```c
const char *config_get_string(char *buffer, int buflen, obj_t com, attr_t attr);
const char *config_gets_string(char *buffer, int buflen, obj_t com, const char *attr, ...);
const char *config_sgets_string(char *buffer, int buflen, const char *com, const char *attr, ...);
```
**Description:** Get configuration value as string

### 6.2 Configuration Set

#### config_set / config_sets / config_sset / config_ssets
```c
boole config_set(obj_t com, talk_t v, attr_t attr);
boole config_sets(obj_t com, talk_t v, const char *attr, ...);
boole config_sset(const char *com, talk_t v, attr_t attr);
boole config_ssets(const char *com, talk_t v, const char *attr, ...);
```
**Description:** Set configuration value. All variants share the same success/`errno` semantics (wrappers preserve `errno` from `config_set`).
**Returns:**
- `true` — succeed. Check **`errno`**:
  - **`EEXIST`** — content unchanged (no file write); safe to skip restart/`_shut`/`_setup`
  - **`0`** — content written to config file
- `false` — failed; **`errno`** is the real error

#### config_set_string / config_sset_string / config_ssets_string
```c
boole config_set_string(obj_t com, const char *string, attr_t attr);
boole config_sset_string(const char *com, const char *string, attr_t attr);
boole config_ssets_string(const char *com, const char *string, const char *attr, ...);
```
**Description:** Set configuration value as string. Same return/`errno` semantics as `config_set` above.
### 6.3 Configuration List and Path

#### config_list
```c
talk_t config_list(const char *project);
```
**Description:** Get configuration list
**Parameters:** project - Project name (NULL for entire system)
**Returns:** JSON format configuration list

#### config_path
```c
const char *config_path(char *buffer, int buflen, const char *project, const char *filename, ...);
```
**Description:** Get configuration file path

### 6.4 Sample program (every `config.h` function)

```c
#include "skin.h"

static void demo_config_all(void)
{
    char buf[512];
    obj_t o = obj_create("land@machine");
    attr_t a = attr_create("walk/key");
    talk_t v = string2x("v");
    talk_t g;

    g = config_get(o, a);
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    g = config_gets(o, "%s", "walk/key");
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    g = config_sget("land@machine", a);
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    g = config_sgets("land@machine", "%s", "walk/key");
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);

    (void)config_get_string(buf, sizeof buf, o, a);
    (void)config_gets_string(buf, sizeof buf, o, "%s", "walk/key");
    (void)config_sgets_string(buf, sizeof buf, "land@machine", "%s", "walk/key");

    (void)config_set(o, v, a);
    /* true + errno==EEXIST: unchanged; true + errno==0: written */
    (void)config_sets(o, v, "%s", "walk/key");
    (void)config_sset("land@machine", v, a);
    (void)config_ssets("land@machine", v, "%s", "walk/key");
    (void)config_set_string(o, "s", a);
    (void)config_sset_string("land@machine", "s", a);
    (void)config_ssets_string("land@machine", "s", "%s", "walk/key");

    g = config_list(NULL);
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    (void)config_path(buf, sizeof buf, "land", "demo.cfg");

    talk_free(v);
    attr_free(a);
    obj_free(o);
}
```

---

## 7. Database API (dbs.h)

### 7.0 Summary

`dbs.h` handles **persistent key/value storage** scoped by component and **database file** (namespace path).

| Concept | Role |
|---------|------|
| **`fa` (file attr)** | Which logical DB file / table space (e.g. path-like string). |
| **`attr`** | Key path **inside** that file (same style as `attr_t` paths). |
| **`*fetch` / `*save`** | Read/write **`talk_t`** values; **`*_string`** variants use a caller buffer. |
| **`dbs_table`** | Invoke a named **table API** (`list`, `query`, …) with **`param_t`**. |
| **`dbs_path`** | Resolve filesystem path for a component + `fa`. |

Naming mirrors **`config_*`**: **`dbs_fetch` + obj** vs **`dbs_sfetch` + string com**, `*s` for varargs path segments. This is **durable storage**; **`config_*`** is oriented toward **current configuration** semantics.

**Whole-file vs key path:** In **`dbs_fetch`** (and the same pattern in save paths), if **`attr` is `NULL`** or **`attr_level(attr) <= 0`**, the implementation loads/saves the **entire** JSON file as one value (via **`file2json`** / full-document write). Otherwise it walks **`attr`** layers inside the parsed file JSON.

---

### 7.1 Data Fetch

#### dbs_fetch / dbs_sfetch / dbs_fetchs / dbs_sfetchs
```c
talk_t dbs_fetch(obj_t com, attr_t fa, attr_t attr);
talk_t dbs_sfetch(const char *com, const char *fa, attr_t attr);
talk_t dbs_fetchs(obj_t com, attr_t fa, const char *attr, ...);
talk_t dbs_sfetchs(const char *com, const char *fa, const char *attr, ...);
```
**Description:** Fetch data from database
**Parameters:**
- fa - Logical DB file / namespace (combined with component to form the on-disk path under **`PROJECT_DBS_DIR`**).
- attr - Key path inside the file, or **`NULL`** / empty-level **`attr_t`** for **whole-file** JSON (see §7.0).

#### dbs_fetch_string / dbs_fetchs_string / dbs_sfetchs_string
```c
const char *dbs_fetch_string(char *buffer, int buflen, obj_t com, attr_t fa, attr_t attr);
const char *dbs_fetchs_string(char *buffer, int buflen, obj_t com, attr_t fa, const char *attr, ...);
const char *dbs_sfetchs_string(char *buffer, int buflen, const char *com, const char *fa, const char *attr, ...);
```
**Description:** Fetch database data as string

### 7.2 Data Save

#### dbs_save / dbs_ssave / dbs_saves / dbs_ssaves
```c
boole dbs_save(obj_t com, attr_t fa, talk_t value, attr_t attr);
boole dbs_ssave(const char *com, const char *fa, talk_t value, attr_t attr);
boole dbs_saves(obj_t com, attr_t fa, talk_t v, const char *attr, ...);
boole dbs_ssaves(const char *com, const char *fa, talk_t value, const char *attr, ...);
```
**Description:** Save data to database

#### dbs_save_string / dbs_ssave_string / dbs_ssaves_string
```c
boole dbs_save_string(obj_t com, attr_t fa, const char *value, attr_t attr);
boole dbs_ssave_string(const char *com, const char *fa, const char *value, attr_t attr);
boole dbs_ssaves_string(const char *com, const char *fa, const char *value, const char *attr, ...);
```
**Description:** Save string data to database

**Example:**
```c
// Save data
talk_t data = string2json("{\"ip\":\"1.1.1.1\"}");
dbs_ssave("network@frame", "/mnt/dbs/net.db", data, attr_create("wan"));
talk_free(data);

// Fetch data
talk_t loaded = (talk_t)dbs_sfetch("network@frame", "/mnt/dbs/net.db", attr_create("wan"));
if (loaded > (void *)tpanic && json_check(loaded)) {
    talk_free(loaded);
}
```

### 7.3 Sample program (every `dbs.h` function)

```c
#include "skin.h"

static void demo_dbs_all(void)
{
    char buf[512];
    obj_t o = obj_create("land@machine");
    attr_t fa = attr_create("/var/tmp/skin_dbs_walk.db");
    attr_t key = attr_create("row/k");
    talk_t val = string2json("{\"n\":1}");
    talk_t t;

    t = dbs_fetch(o, fa, key);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = dbs_fetchs(o, fa, "%s", "row/k");
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = dbs_sfetch("land@machine", "/var/tmp/skin_dbs_walk.db", key);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = dbs_sfetchs("land@machine", "/var/tmp/skin_dbs_walk.db", "%s", "row/k");
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);

    (void)dbs_fetch_string(buf, sizeof buf, o, fa, key);
    (void)dbs_fetchs_string(buf, sizeof buf, o, fa, "%s", "row/k");
    (void)dbs_sfetchs_string(buf, sizeof buf, "land@machine", "/var/tmp/skin_dbs_walk.db", "%s", "row/k");

    (void)dbs_save(o, fa, val, key);
    (void)dbs_saves(o, fa, val, "%s", "row/k");
    (void)dbs_ssave("land@machine", "/var/tmp/skin_dbs_walk.db", val, key);
    (void)dbs_ssaves("land@machine", "/var/tmp/skin_dbs_walk.db", val, "%s", "row/k");
    (void)dbs_save_string(o, fa, "x", key);
    (void)dbs_ssave_string("land@machine", "/var/tmp/skin_dbs_walk.db", "x", key);
    (void)dbs_ssaves_string("land@machine", "/var/tmp/skin_dbs_walk.db", "x", "%s", "row/k");

    t = dbs_table(o, fa, "list", NULL);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    (void)dbs_path(buf, sizeof buf, o, fa);

    talk_free(val);
    attr_free(key);
    attr_free(fa);
    obj_free(o);
}
```

---

## 8. Registry API (register.h)

### 8.0 Summary

`register.h` exposes a **mmap-backed key/value store** per object name (e.g. **`land@syslog`**, system files **`MACHINE_REGFILE`** / **`COM_REGFILE`** in `skinhead.h`): hash index, variable-length heap with freelist, binary-safe, shared across processes.

| API | Role |
|-----|------|
| **`reg_attach` / `reg_detach`** | Cached open (`reg_t`); stays in process cache forever (`reg_detach` no-op). Name **with `@`** → RDWR; **without `@`** → read-only (`put`/`del`/`lock` → `EROFS`) |
| **`wreg_attach` / `wreg_detach`** | Write open: any namespace, always RDWR, **not** cached; must `wreg_detach` to close. Handle works with `reg_put`/`reg_get`/… |
| **`reg_put` / `reg_put_noblock` / `reg_get` / `reg_len` / `reg_cap` / `reg_del` / `reg_del_noblock`** | Blob set/get/delete (`put`/`del` wait on `reg_lock`; `*_noblock` → EBUSY) |
| **`reg_ptr` / `reg_val`** | Zero-copy mmap pointers (`ptr` writable / `val` const). Fixed mmap window — heap auto-grow does not invalidate. RO `ptr` → `EROFS`; use `wreg_attach` + `reg_put` |
| **`reg_lock` / `reg_lock_noblock` / `reg_unlock`** / **`reg_slock` / `reg_olock` / …** | Cooperative fcntl slot lock; lock APIs return value ptr (`NULL`+errno on fail); `s*`/`o*` leave attach ref until matching unlock |
| **`reg_put_int` / `reg_get_int` / `reg_put_str` / …** | Typed helpers |
| **`reg_keys` / `reg_skeys` / `reg_okeys`** | JSON name→size map (**`talk_free`**) |
| **`reg_sput` / `reg_sget` / `reg_sput_int` / …** | Object-string convenience (`reg_attach`+put; sys / no `@` → RO → `EROFS`; write sys via `wreg`) |
| **`reg_oput` / `reg_oget` / `reg_oput_int` / …** | `obj_t` convenience (`obj_name`; NULL → default object) |

Defaults (create only, when args ≤0): **`REG_DEFAULT_SYS_SLOTS` (1024)** if name has no `@`, else **`REG_DEFAULT_OBJ_SLOTS` (128)**; **`max_heap`** defaults to `max_slots * REG_DEFAULT_HEAP_PER_SLOT` (512). Heap starts at **`REG_DEFAULT_HEAP` (16KiB)** and may auto-grow (under flock, no remap) up to `max_heap`. Key names must be shorter than **`REG_NAME_MAX` (32)**. Layout is internal (`register.h`: `REG_VERSION` 2, sparse create).

Legacy `register_*` / old `reg_*` wrappers are declared at the bottom of **`register.h`** and implemented in **`register_compat.c`**; prefer `reg_oput` / `reg_sput` / `reg_oget` / ….

(Unrelated but often used with the same codebase: **`utility.h`** declares **`directory_subsize` / `directory_sum`** without implementations — see §16.5.)

---

### 8.1 Namespace

```c
reg_t reg_attach(const char *object, int max_slots, int max_heap);
void  reg_detach(reg_t r);   /* no-op for cached attaches */
reg_t wreg_attach(const char *object, int max_slots, int max_heap);
void  wreg_detach(reg_t r);  /* closes uncached write handle */
```

**Description:** `reg_attach` / `wreg_attach` open register file for `object` (`NULL`/empty → **`MACHINE_REGFILE`** / `"machine"`). `reg_attach` is process-cached and permanent; names containing `@` are writable, names without `@` (including **`MACHINE_REGFILE`**, **`COM_REGFILE`**) are read-only mmap (`PROT_READ`). Writers use `wreg_attach` … `reg_put` … `wreg_detach` (no cache). `max_slots` / `max_heap` ≤0 use the defaults above when creating; ignored if a valid v2 image already exists.

---

### 8.2 Blob and pointer

```c
void       *reg_put(reg_t r, const char *name, const void *data, int size, int capacity); /* wait if locked; capacity<=0 → default slack; success → value ptr */
void       *reg_put_noblock(reg_t r, const char *name, const void *data, int size, int capacity); /* EBUSY if locked */
int         reg_get(reg_t r, const char *name, void *buf, int buflen);
int         reg_len(reg_t r, const char *name);
int         reg_cap(reg_t r, const char *name);
boole       reg_del(reg_t r, const char *name);
boole       reg_del_noblock(reg_t r, const char *name); /* EBUSY if locked */
void       *reg_ptr(reg_t r, const char *name, int *size_out); /* writable ptr; RO → EROFS */
const void *reg_val(reg_t r, const char *name, int *size_out); /* RO zero-copy */
void       *reg_lock(reg_t r, const char *name);
void       *reg_lock_noblock(reg_t r, const char *name);
boole       reg_unlock(reg_t r, const char *name);
void       *reg_slock(const char *object, const char *name);
void       *reg_slock_noblock(const char *object, const char *name);
boole       reg_sunlock(const char *object, const char *name);
void       *reg_olock(obj_t this, const char *name);
void       *reg_olock_noblock(obj_t this, const char *name);
boole       reg_ounlock(obj_t this, const char *name);
```

**Description:** Put/get by value; `reg_put*` return the mmap value pointer on success (`NULL` + errno on fail). `reg_lock*` / `reg_slock*` / `reg_olock*` likewise return the mmap value pointer on success (`NULL` + errno on fail; pointer valid while the lock is held / map lives). `reg_get` returns stored size. Pointers stay valid for the process while the map lives (fixed mmap window; heap may grow in-file up to `heap_cap` without remap). Key length ≥ `REG_NAME_MAX` → `EINVAL`. On read-only attaches (no `@`), mutations return `EROFS`; use `wreg_attach` + `reg_put`. `put`/`get`/`del`/`attach` take an internal whole-file `flock` for multi-process mutual exclusion. Mutations use publish-last ordering (no freelist split/coalesce) so a kill mid-op must not break hash/slot/freelist — at worst the in-flight op is lost or a chunk/slot leaks. `reg_put` / `reg_del` / `reg_lock` wait if another process holds the cooperative slot lock; `reg_put_noblock` / `reg_del_noblock` / `reg_lock_noblock` return `EBUSY` instead.
**Example:**
```c
reg_t r = reg_attach("land@machine", 0, 0);
reg_put_int(r, "counter", 42);
{
    int n = 0;
    const char *host = (const char *)reg_val(r, "hostname", &n);
    (void)host;
}
/* reg_detach not required for cached attach */

/* System / no-'@' namespace write: */
reg_t w = wreg_attach("machine", 0, 0);
reg_put_str(w, "hostname", "router1");
wreg_detach(w);
```

---

### 8.3 Typed helpers and list

```c
void       *reg_put_int(reg_t r, const char *name, int v);          /* capacity = sizeof(int) */
int         reg_get_int(reg_t r, const char *name, int def);
void       *reg_put_str(reg_t r, const char *name, const char *s);   /* capacity<=0 default slack; NULL s → "" */
#define     reg_put_string  reg_put_str
const char *reg_get_str(reg_t r, const char *name);                  /* zero-copy */
void       *reg_put_boole(reg_t r, const char *name, boole v);       /* capacity = sizeof(boole) */
boole       reg_get_boole(reg_t r, const char *name, boole def);
talk_t      reg_keys(reg_t r);
talk_t      reg_skeys(const char *object);
talk_t      reg_okeys(obj_t this);

void       *reg_sput(const char *object, const char *name, const void *data, int size, int capacity);
const void *reg_sget(const char *object, const char *name, int *size_out);
void       *reg_sput_int(const char *object, const char *name, int v);
int         reg_sget_int(const char *object, const char *name, int def);
void       *reg_sput_str(const char *object, const char *name, const char *s);
#define     reg_sput_string reg_sput_str
const char *reg_sget_str(const char *object, const char *name);
void       *reg_sput_boole(const char *object, const char *name, boole v);
boole       reg_sget_boole(const char *object, const char *name, boole def);

void       *reg_oput(obj_t this, const char *name, const void *data, int size, int capacity);
const void *reg_oget(obj_t this, const char *name, int *size_out);
void       *reg_oput_int(obj_t this, const char *name, int v);
int         reg_oget_int(obj_t this, const char *name, int def);
void       *reg_oput_str(obj_t this, const char *name, const char *s);
#define     reg_oput_string reg_oput_str
const char *reg_oget_str(obj_t this, const char *name);
void       *reg_oput_boole(obj_t this, const char *name, boole v);
boole       reg_oget_boole(obj_t this, const char *name, boole def);
```

**Description:** `reg_s*` attach by object string; `reg_o*` use `obj_name(this)` (`NULL` → default object). `reg_sput` / `reg_oput` take `capacity` like `reg_put` (`<=0` → default slack). Typed `*_int` / `*_boole` use `sizeof(type)`; `*_str` uses default slack. Sys (no `@`) attach is RO — `sput`/`oput` → `EROFS`; write with `wreg_attach` + `reg_put*`.
---

## 9. Structured mmap talk API (mxtalk.h)

### 9.0 Summary

`mxtalk.h` is a **mmap-backed structured key/value store** with talk-like setters/getters. Create with **`m1talk_create*`** (depth 1, flat) or **`m2talk_create*`** (depth 2, root + one OBJECT layer). All ops share the same **`mxtalk_*`** surface. Handles are nullable pointers: **root = map base**; children point at L1/L2 slots. Offsets in the map are relative to the map base except **`MX_VOID`** (writer-local absolute pointers).

| API | Role |
|-----|------|
| **`m1talk_create` / `m1talk_create_file` / `m1talk_map_size` / `m1talk_create_mem`** | Depth **1**: L1 STRING\|INT\|VOID only; no L2 / OBJECT; mem init for munix/mcontrol slots |
| **`m2talk_create` / `m2talk_create_file` / `m2talk_map_size` / `m2talk_create_mem`** | Depth **2**: L1 + shared L2 pool; `mxtalk_json_create` creates OBJECT children; mem init for munix/mcontrol slots |
| **`mxtalk_attach` / `mxtalk_wrap` / `mxtalk_detach`** | Open existing file / wrap memory; release owner |
| **`mxtalk_data` / `mxtalk_size`** | Raw map pointer and byte size |
| **`mxtalk_begin` / `mxtalk_end`** | Seqlock writer fence (nested `set_*` is also nest-safe) |
| **`mxtalk_json_create` / `mxtalk_json`** | Create/lookup OBJECT (depth 2 only; m1 → `EPERM`) |
| **`mxtalk_set_string` / `_cap` / `mxtalk_string` / `stringp`** | STRING cells (heap) |
| **`mxtalk_set_int` / `mxtalk_int` / `mxtalk_intp`** | INT64 cells (in-slot) |
| **`mxtalk_set_pointer` / `mxtalk_pointer`** | VOID cells (absolute ptr, writer-local) |
| **`mxtalk_name` / `type` / `exist` / `string_len` / `string_cap`** | Introspect |
| **`mxtalk_next` / `mxtalk_each`** | Walk children of root or OBJECT |
| **`mxtalk_delete` / `mxtalk_remove`** | Delete node / remove by name |
| **`mxtalk_to_talk` / `talk_to_mxtalk`** | Optional bridge to heap `talk_t` (skips VOID) |

**Defaults** when create args `<=0`: `max_l1=64`, `max_l2=32`, `max_l2_pool=max_l1×4` (min `max_l2`), `name_max=MX_DEFAULT_NAME_MAX` (32), `max_heap=MX_DEFAULT_HEAP` (64 KiB). Heap is **fully reserved** in `map_size` at create (no grow). STRING `cap<=0` follows register-style sizing (`REG_DEFAULT_VAR_SIZE` / round up). Full slot/heap → `NULL` + **`ENOSPC`**.

**Concurrency:** single writer + multi reader (one map-wide `hdr.seq` seqlock). The writer process may call the same getters under `begin`/`end` (reentrant; no self-deadlock). **All** structure/lookup readers wait out an in-progress write (odd `seq`) then re-check: `mxtalk_string` / `stringp` / `json` / `int` / `exist` / `string_len` / `string_cap` / `name` / `type` / `next` / `intp` / `pointer` / `to_talk` (no `EAGAIN` for that reason). `mxtalk_each` is **step-consistent** (each `next` is quiet), not a frozen full-walk snapshot — a completed write between steps may prepend keys you will not see until a later walk. `*_create_file` / `mxtalk_attach(path,1)` take exclusive non-blocking **`flock`**; second writer → **`EBUSY`** (create locks before resizing so a failed second create does not wipe an in-use map). File **create always rebuilds** the map after the lock; to open an existing map for write use **`mxtalk_attach(path,1)`**. `mxtalk_attach(path,0)` is shared RO (no writer flock). `mxtalk_wrap(..., writable)` does **not** take `flock` — caller must ensure single-writer if the memory is shared.

**Naming notes:** cell type **`MX_VOID`** is accessed via **`mxtalk_set_pointer` / `mxtalk_pointer`** (talk-like “pointer”; process-local, skipped by `mxtalk_to_talk`). In-place buffers use **`mxtalk_stringp`** vs **`mxtalk_intp`** (same idea, different suffix). **`mxtalk_type(root)`** is always **`MX_OBJECT`** (including depth-1 roots, which have no L2). Illegal names (empty / `>= name_max`): writers → **`EINVAL`**; readers that only look up often → **`ENOENT`** / “missing”. **`mxtalk_begin` / `mxtalk_end`** on a read-only map are silent no-ops (no `EROFS`).

**Lifetime (caller-owned, like `malloc`/`free`):** Child `mxtalk_t` handles and pointers from `mxtalk_string` / `stringp` / `intp` / `mxtalk_pointer` are **interior pointers** into the map. After `mxtalk_detach`, or after the writer `remove`/`delete`/type-change/STRING grow that recycles that slot or heap block, those pointers are **dangling** — the same class of bug as keeping a pointer after `free`. The library does not stamp generations on handles. Do not cache OBJECT handles across deletes; re-lookup with `mxtalk_json` when needed. Copy string bytes out if they must outlive the next mutating publish or detach. In-place edits via `stringp` / `intp` belong inside `mxtalk_begin` / `mxtalk_end`. Prefer not to `delete`/`remove` the current node inside `mxtalk_each` without saving `mxtalk_next` first (same-process); freed slots keep `next_in_*` so a racing reader can often still step forward.

**Layouts:**

```text
depth 1: [hdr][L1 hash buckets][L1 slots][heap]
depth 2: [hdr][L1 buckets][L1 slots][L2 buckets][L2 pool][heap]
```

Same magic/`MX_VERSION`; `hdr.depth` distinguishes m1 vs m2 maps.

---

### 9.1 Type and constant macros

```c
#define MX_MAGIC            0x4d32544bu  /* 'M2TK' */
#define MX_VERSION          2u
#define MX_NIL              0xFFFFFFFFu
#define MX_DEFAULT_NAME_MAX 32
#define MX_DEFAULT_HEAP     (64u * 1024u)

#define MX_FREE    0
#define MX_STRING  1
#define MX_INT     2
#define MX_OBJECT  3
#define MX_VOID    4

typedef struct mxtalk_st *mxtalk_t;

#define mxtalk_each(var, parent) \
    for ( (var) = mxtalk_next( (parent), NULL ); \
          (var) != NULL; \
          (var) = mxtalk_next( (parent), (var) ) )
```

**Description:** `mxtalk_type()` returns one of `MX_*`. Use `mxtalk_each` to walk all attributes under a parent (root or OBJECT).

---

### 9.2 Create

```c
mxtalk_t m1talk_create(int max_l1, int name_max, int max_heap);
mxtalk_t m1talk_create_file(const char *path, int max_l1, int name_max, int max_heap);
uint32_t m1talk_map_size(int max_l1, int name_max, int max_heap);
mxtalk_t m1talk_create_mem(void *mem, uint32_t size, int max_l1, int name_max, int max_heap);

mxtalk_t m2talk_create(int max_l1, int max_l2, int max_l2_pool,
    int name_max, int max_heap);
mxtalk_t m2talk_create_file(const char *path, int max_l1, int max_l2,
    int max_l2_pool, int name_max, int max_heap);
uint32_t m2talk_map_size(int max_l1, int max_l2, int max_l2_pool,
    int name_max, int max_heap);
mxtalk_t m2talk_create_mem(void *mem, uint32_t size, int max_l1, int max_l2,
    int max_l2_pool, int name_max, int max_heap);
```

**Description:** Anonymous create uses `MAP_ANONYMOUS`. File create takes the writer flock then `ftruncate`s to `map_size` and **reinitializes** the map (always rebuild; use `mxtalk_attach` to open an existing file). `max_l2` caps attributes **per OBJECT**; `max_l2_pool` is the **global** L2 slot pool. **`map_size`** returns bytes needed after cap normalize (`0` + errno on error). **`create_mem`** initializes a caller buffer (`MX_OWN_WRAP`; `size` must be `>= map_size`) — used by `mcontrol_salloc_m*talk` / `alloc_m*talk` inside munix slots; `mxtalk_detach` drops the wrap owner only (does not free the buffer). Failures: `EINVAL` (caps), `EBUSY` (second writer on file), `ENOMEM`, etc. Returns root handle or `NULL`.

**Example:**
```c
mxtalk_t flat;
mxtalk_t nested;
mxtalk_t file_root;

flat = m1talk_create(64, 32, 64 * 1024);
nested = m2talk_create(64, 32, 256, 32, 64 * 1024);
file_root = m2talk_create_file("/tmp/demo.m2.map", 128, 32, 512, 32, 128 * 1024);
if (flat == NULL || nested == NULL || file_root == NULL) {
    /* check errno: EINVAL / EBUSY / … */
}
```

---

### 9.3 Lifecycle

```c
mxtalk_t mxtalk_attach(const char *path, int writable);
mxtalk_t mxtalk_wrap(void *mem, uint32_t size, int writable);
void     mxtalk_detach(mxtalk_t t);
void    *mxtalk_data(mxtalk_t t);
uint32_t mxtalk_size(mxtalk_t t);
```

**Description:** `attach` opens an existing map file (`writable!=0` → RDWR + writer flock; `0` → RO). `wrap` adopts caller memory (must already be a valid map; magic/version checked; **no `flock`** — shared writable wrap is caller’s mutex). `detach` closes fd / unmaps owned maps. `data` / `size` return map base and `hdr.map_size` (root only meaningful for size of whole map).

**Example:**
```c
mxtalk_t w;
mxtalk_t r;
void *base;
uint32_t sz;

w = m1talk_create_file("/tmp/demo.m1.map", 32, 24, 8192);
mxtalk_set_string(w, "host", "gw1");
base = mxtalk_data(w);
sz = mxtalk_size(w);
mxtalk_detach(w);

r = mxtalk_attach("/tmp/demo.m1.map", 0);
if (r != NULL) {
    const char *s;

    s = mxtalk_string(r, "host");
    (void)s;
    mxtalk_detach(r);
}
```

---

### 9.4 Writer coherency

```c
void mxtalk_begin(mxtalk_t t);
void mxtalk_end(mxtalk_t t);
```

**Description:** Seqlock fence for multi-field updates. Public `set_*` already nest `begin`/`end` internally; use explicit `begin`/`end` when writing several related keys as one publish unit, or when patching string buffers returned by `mxtalk_stringp` / `mxtalk_intp`. On a read-only map, `begin`/`end` are **silent no-ops** (they do not set `EROFS`).

**Example:**
```c
mxtalk_begin(root);
mxtalk_set_int(root, "tick", 7);
mxtalk_set_string(root, "tick_s", "s7");
mxtalk_end(root);
```

---

### 9.5 Set / get

```c
mxtalk_t     mxtalk_json_create(mxtalk_t t, const char *name);
mxtalk_t     mxtalk_json(mxtalk_t t, const char *name);

char        *mxtalk_set_string(mxtalk_t t, const char *name, const char *val);
char        *mxtalk_set_string_cap(mxtalk_t t, const char *name, const char *val, int cap);
const char  *mxtalk_string(mxtalk_t t, const char *name);
char        *mxtalk_stringp(mxtalk_t t, const char *name);

int64_t     *mxtalk_set_int(mxtalk_t t, const char *name, int64_t v);
int64_t      mxtalk_int(mxtalk_t t, const char *name, int64_t def);
int64_t     *mxtalk_intp(mxtalk_t t, const char *name);

boole        mxtalk_set_pointer(mxtalk_t t, const char *name, void *ptr);
void        *mxtalk_pointer(mxtalk_t t, const char *name);
```

**Description:** Parent `t` is root or (on m2) an OBJECT. Setting a new type replaces the previous cell. `mxtalk_json_create` fails on depth-1 maps and under an existing OBJECT (`EPERM`). `set_string` / `set_int` / `set_pointer` return pointers into the map (`NULL` + errno on fail: `EROFS`, `ENOSPC`, `EINVAL`, …). `mxtalk_int` returns `def` if missing/wrong type (does not require errno). Pointer getters (`mxtalk_string` / `mxtalk_json` / `mxtalk_pointer` / `mxtalk_stringp` / `mxtalk_intp`) return `NULL` with **`ENOENT`** (missing key), **`EINVAL`** (bad arg, wrong parent, or wrong cell type), or **`EROFS`** (writable-only on RO map); they **wait** if a writer holds the seqlock. Success does not modify errno. `stringp` / `intp` are for in-place mutation inside `begin`/`end`.

**Example:**
```c
mxtalk_t obj;
char *sp;
int64_t *ip;
void *marker;

sp = mxtalk_set_string(root, "s1", "hello");
ip = mxtalk_set_int(root, "n1", -42);
marker = (void *)(uintptr_t)0xabcdu;
(void)mxtalk_set_pointer(root, "p1", marker);

obj = mxtalk_json_create(root, "child");   /* m2 only */
if (obj != NULL) {
    mxtalk_set_string(obj, "inner", "x");
    mxtalk_set_int(obj, "inum", 3);
}
```

---

### 9.6 Walk / introspect / delete

```c
const char  *mxtalk_name(mxtalk_t t);
int          mxtalk_type(mxtalk_t t);
boole        mxtalk_exist(mxtalk_t t, const char *name);
int          mxtalk_string_len(mxtalk_t t, const char *name);
int          mxtalk_string_cap(mxtalk_t t, const char *name);

mxtalk_t     mxtalk_next(mxtalk_t parent, mxtalk_t cur);
boole        mxtalk_delete(mxtalk_t node);
boole        mxtalk_remove(mxtalk_t parent, const char *name);
```

**Description:** `name` / `type` on a child handle (also wait on `seq`). `mxtalk_type(root)` is always `MX_OBJECT`. `string_len` is stored size including NUL; `string_cap` is heap capacity. `next(parent,NULL)` starts iteration; `delete` frees a node handle’s slot by index under the writer fence (no name re-lookup); `remove` looks up by name under parent. Deleting an OBJECT clears its L2 attrs. Avoid deleting the current `each` cursor without saving the next handle first.

**Example:**
```c
mxtalk_t cur;

mxtalk_each(cur, root) {
    printf("%s type=%d\n", mxtalk_name(cur), mxtalk_type(cur));
}
(void)mxtalk_remove(root, "s1");
```

---

### 9.7 talk_t bridge

```c
talk_t mxtalk_to_talk(mxtalk_t t);
boole  talk_to_mxtalk(mxtalk_t t, const char *name, talk_t src);
```

**Description:** `mxtalk_to_talk` builds a heap `talk_t` from root or OBJECT (skips `MX_VOID`; caller `talk_free`). `talk_to_mxtalk` runs the import under one writer `begin`/`end` (nested `set_*` stay inside that publish). If `name` is NULL/empty, import leaves into root (`t` must be root); if `name` is set, create/lookup OBJECT then import (depth 2 only — on m1 → `EPERM`). Nested objects under an OBJECT are rejected. Mid-import failure can leave a partial map (not transactional). Skin `json_set_number` stores decimal text and uses `int`; mxtalk `MX_INT` is `int64_t` — round-trips are lossy / may land as `MX_STRING`.

**Example:**
```c
talk_t jt;
talk_t src;

jt = mxtalk_to_talk(root);
if (jt != NULL) {
    talk_free(jt);
}
src = json_create(NULL);
json_set_string(src, "from", "talk");
(void)talk_to_mxtalk(root, "imp", src);  /* m2: creates OBJECT "imp" */
talk_free(src);
/* m1 flat import into root: talk_to_mxtalk(root, NULL, flat_src); */
```

---

### 9.8 Sample program (every `mxtalk.h` API)

```c
#include "skin/skin.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main(void)
{
    mxtalk_t root;
    mxtalk_t obj;
    mxtalk_t cur;
    mxtalk_t ro;
    char *sp;
    const char *cs;
    int64_t *ip;
    int64_t iv;
    void *vp;
    void *marker;
    talk_t jt;
    talk_t src;
    void *base;
    uint32_t map_sz;
    const char *path = "/tmp/mxtalk_doc_demo.map";

    /* —— m1 create / flat ops —— */
    root = m1talk_create(32, 24, 16384);
    if (root == NULL) {
        return 1;
    }
    if (mxtalk_json_create(root, "nope") != NULL) {
        /* depth 1: must fail with EPERM */
        mxtalk_detach(root);
        return 1;
    }
    sp = mxtalk_set_string(root, "s1", "hello");
    sp = mxtalk_set_string_cap(root, "s2", "ab", 16);
    ip = mxtalk_set_int(root, "n1", 99);
    marker = (void *)(uintptr_t)0x1111u;
    (void)mxtalk_set_pointer(root, "p1", marker);
    cs = mxtalk_string(root, "s1");
    sp = mxtalk_stringp(root, "s1");
    iv = mxtalk_int(root, "n1", 0);
    ip = mxtalk_intp(root, "n1");
    vp = mxtalk_pointer(root, "p1");
    (void)mxtalk_exist(root, "s1");
    (void)mxtalk_string_len(root, "s1");
    (void)mxtalk_string_cap(root, "s2");
    mxtalk_each(cur, root) {
        (void)mxtalk_name(cur);
        (void)mxtalk_type(cur);
    }
    jt = mxtalk_to_talk(root);
    if (jt != NULL) {
        talk_free(jt);
    }
    src = json_create(NULL);
    json_set_string(src, "a", "b");
    (void)talk_to_mxtalk(root, NULL, src);   /* flat import OK on m1 */
    talk_free(src);
    (void)mxtalk_remove(root, "s2");
    mxtalk_detach(root);

    /* —— m2 create_file / OBJECT / attach / wrap / begin-end —— */
    unlink(path);
    root = m2talk_create_file(path, 64, 16, 128, 32, 32768);
    if (root == NULL) {
        return 1;
    }
    base = mxtalk_data(root);
    map_sz = mxtalk_size(root);
    mxtalk_begin(root);
    mxtalk_set_string(root, "tick_s", "s0");
    mxtalk_set_int(root, "tick", 0);
    mxtalk_end(root);
    obj = mxtalk_json_create(root, "box");
    if (obj != NULL) {
        mxtalk_set_string(obj, "a", "x");
        mxtalk_set_int(obj, "b", 1);
        (void)mxtalk_json(root, "box");
        mxtalk_each(cur, obj) {
            (void)mxtalk_name(cur);
        }
        (void)mxtalk_delete(obj);   /* or mxtalk_remove(root, "box") */
    }
    src = json_create(NULL);
    json_set_string(src, "from", "talk");
    (void)talk_to_mxtalk(root, "imp", src);
    talk_free(src);
    mxtalk_detach(root);

    ro = mxtalk_attach(path, 0);
    if (ro != NULL) {
        (void)mxtalk_string(ro, "tick_s");
        /* RO write fails with EROFS */
        mxtalk_detach(ro);
    }
    /* optional: mxtalk_wrap(base, map_sz, 1) when owning raw memory */
    (void)base;
    (void)map_sz;
    (void)sp;
    (void)cs;
    (void)ip;
    (void)iv;
    (void)vp;
    unlink(path);
    return 0;
}
```

---

## 10. Unix mmap IPC API (munix.h)

### 10.0 Summary

`munix.h` combines **AF_UNIX SOCK_DGRAM signaling** with **shared mmap slot pools** for payloads. The server owns two maps (`in` = client→server requests, `out` = server→client replies). Paths come from `project_var_path`: unix `…/<name>.unix`, maps `…/<name>.mmap.in` / `…/<name>.mmap.out`.

**Handle is the unix socket `fd`** (`listen`/`connect` return it; **must** close with `munix_close`). **One fd = one serialized request/reply channel.** Parallel in-flight RPCs need multiple endpoints.

**Opaque handle:** `munix_slot_t` is a pointer to a private struct — free with `munix_slot_free`.

**Peer:** `munix_client_st` is caller storage (like `recvfrom`); `munix_client_t` is `munix_client_st *`. `sunix_take` writes **`addr`/`addrlen`/`corr`** (preserves **`data`**); `sunix_post` echoes `corr` on the reply; pass `&peer` to take/post; no alloc/free.

**fd / slot lifetime:** use only `munix_close(fd)`. Do **not** `close(fd)` or `dup()` the fd into munix APIs — `close` alone leaves the internal fd→ctx table and maps/paths unclean; a later open that reuses the number can hit the wrong endpoint. **`munix_slot_free` all owned slots before `munix_close`** — a slot handle keeps a pointer into the endpoint; free-after-close is use-after-free.

| API | Role |
|-----|------|
| **`munix_listen` / `munix_connect` / `munix_close`** | Endpoint; returned `fd` for `poll` / `select` / libevent |
| **`munix_set_data` / `munix_get_data`** | Caller cookie on endpoint (`void*`; munix never frees) |
| **`munix_slot_alloc` / `sunix_slot_alloc`** | Client request slot / server reply slot |
| **`munix_slot_data` / `munix_slot_cap` / `munix_slot_free`** | Payload pointer / usable size / return to pool |
| **`munix_slot_set_data` / `munix_slot_get_data`** | Caller cookie on slot handle (`void*`; munix never frees) |
| **`munix_client_set_data` / `munix_client_get_data`** | Caller cookie on peer (`munix_client_t`; `sunix_take` preserves) |
| **`munix_post` / `munix_take`** | Client send / receive |
| **`sunix_take` / `sunix_post`** | Server receive / send |
| **`MUNIX_POST_KEEP`** | Keep slot handle after post (peer must not pool-free) |
| **`MUNIX_MMAP_ONLY`** | Alloc from mmap only (no unix `GRANT_REQ` / wait) |

**Conventions:**

- **key**: required NUL-terminated string (may be `""`); soft max **1024** bytes in the implementation. Pointer from take is valid until the **next successful** take on that endpoint (failed take / alloc drain / discarded mismatch do not overwrite it). Short control/meta can go in **key only** (`slot=NULL`) without a payload slot.
- **slot**: may be `NULL` on **post** for **key-only** traffic (`MUNIX_POST_KEEP` + `NULL` → `EINVAL`). On **take**, the out-pointer is required (`NULL` → `EINVAL`); `*slot` may be `NULL` (key-only reply). Wire length is always `munix_slot_cap` (full allocated/usable buffer), not a separate app length — frame inside the buffer yourself (`\0`, m1talk, …), or use key-only when the payload would be tiny. **`munix_slot_alloc` / `sunix_slot_alloc` require `len > 0`** (`len==0` → `NULL`/`EINVAL`); do not use `alloc(0)` for key-only.
- **`timeout_ms`**: `0` = non-block (`EAGAIN`), `>0` = wait ms (`ETIMEDOUT`), `<0` = forever. Ignored when `MUNIX_MMAP_ONLY`. Client `munix_take` may wait; **`sunix_take` is always non-blocking** (server is 1:N — do not block the listen loop; `poll`/`select` first). **After `POLLIN`, drain like non-blocking `recvfrom`**: loop take until `NULL`/`EAGAIN`. One successful take is one `POST`; `GRANT_REQ` / `HELLO` / junk are consumed inside take and do not return to the caller. GRANT waiters are woken when take hits `EAGAIN` (socket empty) or when an in-map slot is freed.
- **Client order**: one in-flight RPC per connect fd — `alloc → post → take` (then free as needed). Wire **`corr`** (monotonic `uint64`) tags each request `POST`; the reply must echo it. While a reply is pending, `munix_take` delivers only that corr (mismatched POSTs are discarded and **out** slots returned). Idle take (`reply_pending` clear) delivers any valid POST. Starting a new **`alloc` abandons** any prior unread reply by clearing `reply_pending` (one-shot; unread = failed/dropped). mmap-hit does **not** drain the fd — leftover datagrams stay until a later `take` / drain / `munix_close`. `post` while a reply is still pending → `EBUSY` (take it, or alloc to abandon). Parallel RPCs → multiple `connect` fds. `GRANT_REQ`/`GRANT` use the same corr field for alloc-wait matching. **`munix_slot_alloc` tries the mmap freelist first**, except a same-len pending GRANT skips mmap and only drains/claims; on miss it may `GRANT_REQ` + drain (claim GRANT into a slot, or discard leftover POSTs). Event loop: while waiting for GRANT, POLLIN must retry **`munix_slot_alloc`**, not `munix_take` (take returns unused GRANTs to the pool; matching `corr` clears `grant_pending_req`).
- **Server restart**: client `take`/`alloc`/`post` → `ESTALE` once the session is marked stale; `munix_close` + `munix_connect` and retry the whole RPC.
- **Caller contracts (not enforced by the API)**: only `munix_close` (never bare `close`/`dup` into munix); free owned **slots** **before** `munix_close`; KEEP needs cooperative peers (see §10.1 / §10.9); **one fd is not thread-safe**; **multi-fd × one thread per fd is OK** (process×map `plock->gate` serializes map lock/unlock); parallel RPCs use multiple `connect` fds; do not use across `fork` without reconnect.
- **mmap pool lock (v7):** header `lock` is `MU_LOCK_PACK(pid, epoch)` on a word matching the ABI (`uint64` on LP64: pid32|epoch32; `uint32` on 32-bit: pid16|epoch16, native CAS; keep `pid_max` ≤ 65536 on 32-bit). (`0` = free; legacy TAS `pid==1` still stealable). A separate **`dirty`** bit is set while a holder may be mid-update and cleared only on unlock. **Steal** (dead/legacy owner — `kill(0)` only after 65535 contended `sched_yield`s so live holders are not probed every spin; **pid reuse** where this process does not hold, or **held+epoch mismatch** across threads) or **`dirty`** forces dead-owner reclaim + freelist rebuild. Process-local hold state (`gate`+`held`+`epoch`) is keyed by map file identity (`dev`+`ino`) and shared by all fds on that map — `gate` allows one-thread-per-connect-fd; `held`+`epoch` detect pid reuse. Slot ownership is **`(owner_pid, owner_birth)`** (starttime; self cached once); pressure reclaim treats mismatched birth as dead. Steal/dirty repair under the map lock uses **`kill`-only** liveness (no `/proc`); full `(pid, birth)` checks run on **pressure** `mu_reclaim_dead` **outside** the lock (snapshot → classify → re-lock), and only for slots whose reserved stamp is at least ~2s old. **Hybrid concurrency:** the map lock covers **freelist + heap** (`alloc` / `free` / reclaim rebuild). Per-slot **`state`** for `post` (`ALLOC|HELD→POSTED`) and take claim (`POSTED→HELD`) is **lock-free CAS** (no map lock on that hot path); freelist return always **CAS → `FREE`** then `gen++` so it cannot race a concurrent post/take. **Heap:** first 8 bytes are permanent pad so chunk offset `0` stays the freelist-empty / alloc-fail sentinel.
- **fd close**: only `munix_close(fd)` — never bare `close(fd)` / `dup()` into munix APIs (see §10.1).
- **slot vs close**: `munix_slot_free` owned handles **before** `munix_close`; free-after-close is UAF (see §10.1 / §10.2).
- **`munix_slot_t`**: opaque pointer. **`munix_client_st`**: caller-owned peer storage (`addr`/`addrlen`/`corr`/`data`); **`munix_client_t`**: pointer to it. `sunix_take` fills `addr`/`addrlen`/`corr` (preserves `data`); `sunix_post` echoes `corr`; pass `&peer` to take/post.
- **`munix_set_data` / `munix_get_data`**: optional caller cookie on the endpoint (`void*`). Useful when an event loop only has `fd`. Munix never frees the pointer; clear or replace before `munix_close` if you own the object.
- **`munix_slot_set_data` / `munix_slot_get_data`**: optional caller cookie on a **slot handle** (not the mmap payload from `munix_slot_data`). Default `NULL` at handle create; `munix_slot_free` does not free the cookie.
- **`munix_client_set_data` / `munix_client_get_data`**: optional caller cookie on peer storage; `sunix_take` does not clear it.
- **Safe point for KEEP reuse**: after you `munix_take` the reply, the peer has finished with the previous request slot.
- **Roles**: use client APIs only on `connect` fds and server APIs only on `listen` fds (wrong role → fail / `EINVAL`).

**Roles:**

```text
Client: connect → alloc → fill → post → take → (slot_free if needed)
Server: listen  → poll(fd) → sunix_take until EAGAIN → (optional sunix_slot_alloc) → sunix_post
```

---

### 10.1 Endpoint

```c
int  munix_listen(const char *name,
    int in_slots, size_t in_heap,
    int out_slots, size_t out_heap);
int  munix_connect(const char *name);
void munix_close(int fd);
void munix_set_data(int fd, void *data); /* caller cookie; never freed by munix */
void *munix_get_data(int fd);
```

**Description:** `listen` creates the unix socket and both mmap files (new `map_gen` each listen). **Pool args:** `in_slots` / `out_slots` must be **`≥ 1`**; values **above 512 are clamped to 512**. `in_heap` / `out_heap` must be **`≥ 64`** and fit in `uint32_t`; the resulting map size (`header + slot table + heap`) must also fit in `uint32_t` or `listen` fails with `EINVAL` (rejects combinations that would wrap). `connect` attaches maps, reads `map_gen`, binds a **per-endpoint** client datagram path (`…/<name>.unix-<pid>-<fd>` so same-process multi-connect does not collide), then connects to the server unix (**does not** send HELLO). Both return the unix socket `fd` (`>=0`) or `-1` on failure. The fd is non-blocking and suitable for event loops; pass the same fd into the other munix APIs.

**Cookie (`munix_set_data` / `munix_get_data`):** attach a private pointer for wrappers / libevent callbacks that only see `fd`. Default is `NULL`. Munix never frees it; `munix_close` drops the endpoint only.

```c
munix_set_data(cli, my_session);
/* in poll callback: */
sess = munix_get_data(fd);
```

**`munix_close` vs `close`:**

| Do | Do not |
|----|--------|
| `munix_close(fd)` to destroy the endpoint | `close(fd)` on a munix fd |
| `poll` / `select` / libevent on the same `fd` | `dup(fd)` then pass the copy into munix APIs |
| `munix_slot_free` owned slots, then `munix_close` | `munix_slot_free` after / across `munix_close` |

Why:

- Bare `close(fd)`: munix keeps an internal **fd → endpoint** table plus mmap/path state. `close` only drops the kernel socket; the table entry and maps remain. When that fd number is reused later, munix APIs can hit the **wrong** endpoint (`EBUSY` on register, or silent mis-routing). No kernel hook can fix this — callers must use `munix_close`.
- Free-after-close: each `munix_slot_t` points back at the endpoint. `munix_close` frees that endpoint; a later `munix_slot_free` / `munix_slot_data` is **use-after-free**. Munix does not track live slot handles, so this is a caller ordering rule (same class of contract as `close` vs `munix_close`).

**Example:**

```c
int srv;
int cli;

srv = munix_listen("echo_demo", 16, 64 * 1024, 16, 64 * 1024);
cli = munix_connect("echo_demo");
if (srv < 0 || cli < 0) {
    /* path / map / bind failure */
}
```

---

### 10.2 Slots

```c
#define MUNIX_MMAP_ONLY  0x2

typedef struct munix_slot_st *munix_slot_t; /* opaque */

void        *munix_slot_data(munix_slot_t slot);
size_t       munix_slot_cap(munix_slot_t slot);
void         munix_slot_free(munix_slot_t slot);
void         munix_slot_set_data(munix_slot_t slot, void *data);
void        *munix_slot_get_data(munix_slot_t slot);

munix_slot_t munix_slot_alloc(int fd, size_t len, int timeout_ms, int flags);
munix_slot_t sunix_slot_alloc(int fd, size_t len);
```

**Description:**

- **`munix_slot_data`**: mmap payload pointer. **`munix_slot_set_data` / `get_data`**: separate caller cookie on the handle (default `NULL`; munix never frees it; not cleared by `munix_slot_free` beyond destroying the handle).
- Client `munix_slot_alloc` takes from the **in** map. **`len` must be `> 0`** (`len==0` → `NULL`/`EINVAL`); sizes that would overflow internal `uint32` chunk math are also rejected (`EINVAL`). No-payload traffic uses `slot=NULL` on post, not `alloc(0)`. Default alloc **tries mmap freelist first**; mmap-hit returns a slot and **clears** any pending GRANT wait (does not drain the fd). Only on miss may it send `GRANT_REQ` and wait per `timeout_ms` (fd drain claims a matching GRANT into a slot immediately, or discards leftover POSTs). Non-block (`timeout_ms=0`): after `GRANT_REQ`, `EAGAIN` until readable — next `alloc` with the same `len` **skips mmap** (does not race the freelist), does **not** resend, and only drains/claims. No GRANT → keep `EAGAIN` (like `recvfrom`); the caller times out or `munix_close` + reconnects. Block wait uses `timeout_ms` (`ETIMEDOUT` clears pending so a later alloc may send a fresh REQ). **`MUNIX_MMAP_ONLY`**: only local mmap (never `GRANT_REQ` / drain / wait on the fd, even if a GRANT_REQ is already pending); mmap-hit still returns a slot and clears that pending; full → immediate `EAGAIN` (pending unchanged) — preferred in `select`/libevent hot paths. Event loop: POLLIN while a GRANT is pending must retry **`munix_slot_alloc`** (same `len`), not `munix_take`.
- **`GRANT_REQ` retry:** after `ETIMEDOUT`, the next `munix_slot_alloc` sends a fresh `GRANT_REQ`. If `len` changes while a request is still pending, munix resends with the new size. Non-blocking (`timeout_ms=0`) may keep a pending flag so a tight poll loop does not spam the server until success or a timed wait expires. Server queues `GRANT_REQ` FIFO (O(1) tail append, grant head) during take and wakes waiters when take returns `EAGAIN` (drain the fd in the same poll round) or when an in-map slot is freed. Take `EAGAIN` with waiters: one in-map `mu_reclaim_dead` (age ≥ ~2s, then UNCLAIMED without `/proc` or dead owner via `/proc`) then GRANT already-free slots. In-map `munix_slot_free` only wakes (take just scanned). Wake peeks free slots and remaining heap against the **head waiter's `need_len`**: if either cannot satisfy that size, stop (do not walk waiters). If the head cannot fit while a **smaller** waiter is behind, the head is rotated to the tail (true HOL); same-or-larger needs behind the head are not rotated. Send-buffer full (`EAGAIN`) stops the wake until a later free; dead-peer send errors drop that waiter and continue. AF_UNIX SOCK_DGRAM `ECONNREFUSED` / `ECONNRESET` from a dead peer are skipped inside take/alloc drain (same as junk frames) so they do not starve `POLLIN`; client waits also `poll` `POLLERR`.
- Server `sunix_slot_alloc` takes from the **out** map (**`len > 0`** same as client); never waits (`NULL`/`EAGAIN` when full). Under out-pool pressure it may reclaim dead owners and aged **non-KEEP `POSTED`** replies (see §10.9).
- `munix_slot_cap` is the usable size for this handle (after alloc: write budget; after take: valid payload size to read).
- Free when you own the handle: after take; after failed post; after successful post **without** KEEP; KEEP owner frees when done with the channel. Always free owned slots **before** `munix_close` — free-after-close is use-after-free.

**Example (default alloc vs `MUNIX_MMAP_ONLY`):**

```c
munix_slot_t a;
munix_slot_t b;

/* may wait / GRANT when pool busy */
a = munix_slot_alloc(cli, 64, 1000, 0);

/* event-loop style: never block on GRANT */
b = munix_slot_alloc(cli, 64, 0, MUNIX_MMAP_ONLY);
if (b == NULL && errno == EAGAIN) {
    /* try later or fail this RPC */
}
```

---

### 10.3 Client post / take

```c
#define MUNIX_POST_KEEP  0x1

int         munix_post(int fd, const char *key, munix_slot_t slot, int flags);
const char *munix_take(int fd, munix_slot_t *slot, int timeout_ms);
```

**`munix_post`:** `key` is required (not NULL; may be `""`). `slot` may be NULL (key-only). **`MUNIX_POST_KEEP` + `slot==NULL` → `EINVAL`**. `flags=0`: success **consumes** the handle (do not free/use again). **`MUNIX_POST_KEEP`**: handle stays valid; peer must not pool-free; reuse after you have taken the reply. One in-flight RPC per fd: `post` assigns a wire **`corr`**. **Send is non-blocking (`sendto`-style)**: socket send buffer full → `-1`/`EAGAIN` and the slot handle is **not** consumed (state rolled back) — wait for writable (`poll`/`EV_WRITE`) and retry the same post. Session already stale → `-1`/`ESTALE`. Wrong endpoint / wrong send-side map for `slot` → `-1`/`EINVAL`. Reply still pending → `-1`/`EBUSY` (take the reply, or `alloc` to abandon that round).

**`munix_take`:** out-pointer `slot` is required (`NULL` → `EINVAL`); `*slot` may be `NULL` (key-only). `timeout_ms` `< 0` block, `0` `EAGAIN`, `> 0` `ETIMEDOUT`. While a reply is pending, delivers only the last post’s matching `corr`; mismatched POSTs are discarded (**out** slots returned). Idle take (`reply_pending` clear) delivers any valid POST. Fail → `NULL` + errno (`EAGAIN` / `ETIMEDOUT` / `ESTALE` / `EPROTO` / `ENOMEM` / …). **Corrupt / truncated datagrams are skipped**; `EPROTO` is reserved for slot/state protocol breaks after a valid matching `POST` frame (e.g. gen mismatch, late aged reclaim). If building the slot handle fails with **`ENOMEM`**, or wire `payload_len` exceeds the slot **`payload_cap`** (`EPROTO`): non-KEEP reply is freelisted (message dropped); **KEEP** leaves the slot `POSTED` so the **sender’s** handle stays valid — the datagram is already consumed, so **this take cannot be retried** (further takes wait for a new reply). Timeout does **not** clear `reply_pending`. GRANT datagrams on take are always returned to the pool (not claimed). Matching `corr` **clears** `grant_pending_req` so a later alloc may send a fresh `GRANT_REQ`; wrong/late GRANT keeps the wait. Claim is only in `munix_slot_alloc`.

---

### 10.4 Server take / post

```c
typedef struct munix_client_struct {
    struct sockaddr_un addr;
    socklen_t addrlen;
    void *data; /* caller cookie; sunix_take does not clear; munix never frees */
    uint64_t corr; /* request corr from last sunix_take; sunix_post echoes it */
} munix_client_st;
typedef munix_client_st *munix_client_t;

const char *sunix_take(int fd, munix_slot_t *slot, munix_client_t client);
int         sunix_post(int fd, const char *key, munix_slot_t slot,
                munix_client_t client, int flags);

void  munix_client_set_data(munix_client_t client, void *data);
void *munix_client_get_data(munix_client_t client);
```

**Description:** `sunix_take` is **always non-blocking** (`NULL`/`EAGAIN` if idle) by design: one server fd serves many clients, so the listen loop must not block inside take — use `poll`/`select`/libevent, then **drain like non-blocking `recvfrom`** (loop take until `EAGAIN`). One success is one `POST`; `GRANT_REQ` is queued inside take and waiters are woken on that `EAGAIN` (and on in-map `slot_free`). Out-pointer `slot` and `client` are required (`NULL` → `EINVAL`); `*slot` may be NULL (key-only). **Corrupt / truncated datagrams are skipped** (same as client take). On success: key (may be `""`), `*slot` may be NULL, and `client` gets peer **`addr`/`addrlen`/`corr`** (**recvfrom style** — **`data` is preserved**; caller owns the `munix_client_st`; safe to keep/copy across later takes). On failure, the caller's storage is **unchanged** — only trust `addr`/`addrlen`/`corr` after success. `sunix_post` requires a `munix_client_t` (pointer to that storage or any saved copy) and **echoes `client->corr`** on the reply wire; `slot` may be NULL; `flags` same KEEP rules as client post (`KEEP` + `NULL` → `EINVAL`). Slot must belong to this listen fd’s **out** map (`EINVAL` otherwise). **`sunix_post` is non-blocking (`sendto`-style)** — full send buffer → `-1`/`EAGAIN`, handle kept for retry when writable (do not block the listen loop waiting inside post). Optional: drain take first, copy `key` + `munix_client_st`, free the in slot, queue the reply, `sunix_post` on `POLLOUT` (§10.5.1). The listen fd is never marked `session_stale` (that flag is client-only), so `sunix_post` does not return `ESTALE` in practice. Library take paths write only `addr`/`addrlen`/`corr` (never wipe `data`); callers may zero-init or `munix_client_set_data` before first use if they rely on the cookie.

---

### 10.5 Server example (echo)

```c
#include "skin/skin.h"
#include <poll.h>
#include <string.h>

static int run_server(const char *name)
{
    int mx;
    struct pollfd pfd;
    munix_slot_t in_slot;
    munix_slot_t out_slot;
    munix_client_st client;
    const char *key;
    void *src;
    void *dst;
    size_t n;

    mx = munix_listen(name, 16, 64 * 1024, 16, 64 * 1024);
    if (mx < 0) {
        return 1;
    }
    pfd.fd = mx;
    pfd.events = POLLIN;
    for (;;) {
        if (poll(&pfd, 1, 1000) <= 0) {
            continue;
        }
        for (;;) {
            key = sunix_take(mx, &in_slot, &client);
            if (key == NULL) {
                break;
            }
            out_slot = NULL;
            if (in_slot != NULL) {
                n = munix_slot_cap(in_slot);
                out_slot = sunix_slot_alloc(mx, n);
                if (out_slot != NULL) {
                    src = munix_slot_data(in_slot);
                    dst = munix_slot_data(out_slot);
                    memcpy(dst, src, n);
                }
                munix_slot_free(in_slot);
            }
            /* out full → key-only reply still OK; failed post keeps ownership */
            if (sunix_post(mx, key, out_slot, &client, 0) != 0) {
                if (out_slot != NULL) {
                    munix_slot_free(out_slot);
                }
            }
        }
    }
    /* munix_close(mx); */
}
```

§10.5 posts in the take loop (enough when replies are cheap). To drain receive first and send when the fd is writable — same as non-blocking `recvfrom` then `sendto` on `POLLOUT` — copy `key` and `munix_client_st` (the next successful take overwrites them), move payload to an out slot, **free the in slot before queueing**, then `sunix_post` until `EAGAIN` and wait `POLLOUT`. Do not keep in slots on the send queue (that pins the in map and delays GRANT).

### 10.5.1 Server example (drain take, send queue on POLLOUT)

```c
#include "skin/skin.h"
#include <errno.h>
#include <poll.h>
#include <string.h>

#define MU_EX_Q 32
#define MU_EX_KEY 1025

static int run_server_queued(const char *name)
{
    int mx;
    struct pollfd pfd;
    struct {
        munix_client_st client;
        munix_slot_t slot;
        char key[MU_EX_KEY];
    } q[MU_EX_Q];
    int qhead;
    int qn;
    int qi;
    int err;
    munix_slot_t in_slot;
    munix_slot_t out_slot;
    munix_client_st client;
    const char *key;
    void *src;
    void *dst;
    size_t n;
    size_t klen;

    mx = munix_listen(name, 16, 64 * 1024, 16, 64 * 1024);
    if (mx < 0) {
        return 1;
    }
    qhead = 0;
    qn = 0;
    pfd.fd = mx;
    for (;;) {
        pfd.events = POLLIN;
        if (qn > 0) {
            pfd.events = POLLIN | POLLOUT;
        }
        if (poll(&pfd, 1, 1000) <= 0) {
            continue;
        }

        if ((pfd.revents & POLLIN) != 0) {
            for (;;) {
                key = sunix_take(mx, &in_slot, &client);
                if (key == NULL) {
                    break;
                }
                out_slot = NULL;
                if (in_slot != NULL) {
                    n = munix_slot_cap(in_slot);
                    out_slot = sunix_slot_alloc(mx, n);
                    if (out_slot != NULL) {
                        src = munix_slot_data(in_slot);
                        dst = munix_slot_data(out_slot);
                        memcpy(dst, src, n);
                    }
                    munix_slot_free(in_slot);
                }
                if (qn >= MU_EX_Q) {
                    if (out_slot != NULL) {
                        munix_slot_free(out_slot);
                    }
                    continue;
                }
                qi = qhead + qn;
                if (qi >= MU_EX_Q) {
                    qi -= MU_EX_Q;
                }
                q[qi].client = client;
                q[qi].slot = out_slot;
                klen = strlen(key);
                if (klen > sizeof(q[qi].key) - 1) {
                    klen = sizeof(q[qi].key) - 1;
                }
                memcpy(q[qi].key, key, klen);
                q[qi].key[klen] = '\0';
                qn++;
            }
        }

        while (qn > 0) {
            qi = qhead;
            if (sunix_post(mx, q[qi].key, q[qi].slot, &q[qi].client, 0) != 0) {
                err = errno;
                if (err == EAGAIN || err == EWOULDBLOCK) {
                    break;
                }
                if (q[qi].slot != NULL) {
                    munix_slot_free(q[qi].slot);
                }
            }
            qhead++;
            if (qhead >= MU_EX_Q) {
                qhead = 0;
            }
            qn--;
        }
    }
    /* munix_close(mx); */
}
```

---

### 10.6 Client — synchronous call

```c
static int rpc_sync(int mx, const char *api, const void *req, size_t req_len)
{
    munix_slot_t slot;
    munix_slot_t reply;
    const char *key;
    void *p;
    size_t cap;

    slot = munix_slot_alloc(mx, req_len, 2000, 0);
    if (slot == NULL) {
        return -1;
    }
    p = munix_slot_data(slot);
    cap = munix_slot_cap(slot);
    if (req_len > cap) {
        munix_slot_free(slot);
        return -1;
    }
    memcpy(p, req, req_len);
    if (munix_post(mx, api, slot, 0) != 0) {
        munix_slot_free(slot);
        return -1;
    }
    /* non-KEEP: slot handle already consumed */
    key = munix_take(mx, &reply, 5000);
    if (key == NULL) {
        return -1; /* ETIMEDOUT / ESTALE / … */
    }
    if (reply != NULL) {
        /* use munix_slot_data(reply), munix_slot_cap(reply) */
        munix_slot_free(reply);
    }
    return 0;
}
```

---

### 10.7 Client — `select` asynchronous

Pre-loan a KEEP slot (or use `MUNIX_MMAP_ONLY` and fail-fast). Loop only waits for replies.

```c
#include <sys/select.h>
#include <errno.h>

enum { ST_IDLE = 0, ST_WAIT_REPLY };

struct rpc_sel {
    int mx;
    munix_slot_t req; /* KEEP */
    int state;
};

static int rpc_sel_open(struct rpc_sel *r, const char *name, size_t need)
{
    r->mx = munix_connect(name);
    if (r->mx < 0) {
        return -1;
    }
    r->req = munix_slot_alloc(r->mx, need, -1, 0);
    if (r->req == NULL) {
        munix_close(r->mx);
        r->mx = -1;
        return -1;
    }
    r->state = ST_IDLE;
    return 0;
}

static int rpc_sel_begin(struct rpc_sel *r, const char *api)
{
    void *p;
    size_t n;

    if (r->state != ST_IDLE) {
        errno = EBUSY;
        return -1;
    }
    p = munix_slot_data(r->req);
    n = munix_slot_cap(r->req);
    /* fill p[0..n) */
    (void)p;
    (void)n;
    if (munix_post(r->mx, api, r->req, MUNIX_POST_KEEP) != 0) {
        return -1;
    }
    r->state = ST_WAIT_REPLY;
    return 0;
}

static int rpc_sel_on_readable(struct rpc_sel *r)
{
    munix_slot_t reply;
    const char *key;

    if (r->state != ST_WAIT_REPLY) {
        return 0;
    }
    key = munix_take(r->mx, &reply, 0);
    if (key == NULL) {
        if (errno == EAGAIN) {
            return 0;
        }
        return -1; /* ESTALE → close + reopen */
    }
    if (reply != NULL) {
        munix_slot_free(reply);
    }
    r->state = ST_IDLE;
    return 0;
}

/* main loop sketch */
static void rpc_sel_loop(struct rpc_sel *r)
{
    fd_set rfds;
    int fd;
    int n;

    fd = r->mx;
    (void)rpc_sel_begin(r, "api");
    for (;;) {
        if (r->state == ST_IDLE) {
            break; /* or begin next call */
        }
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        n = select(fd + 1, &rfds, NULL, NULL, NULL);
        if (n < 0 && errno == EINTR) {
            continue;
        }
        if (n > 0 && FD_ISSET(fd, &rfds)) {
            if (rpc_sel_on_readable(r) != 0) {
                break;
            }
        }
    }
}
```

Without KEEP, each shot uses `munix_slot_alloc(mx, n, 0, MUNIX_MMAP_ONLY)` in `begin`; on `EAGAIN` skip/ defer — no GRANT-wait state. If default alloc (`flags=0`, `timeout_ms=0`) is used instead, POLLIN must retry **`munix_slot_alloc`**, not `munix_take`.

---

### 10.8 Client — libevent asynchronous

```c
#include <event2/event.h>
#include <errno.h>

struct rpc_ev {
    int mx;
    munix_slot_t req;
    struct event *ev;
    int waiting;
};

static void rpc_ev_readable(evutil_socket_t fd, short what, void *arg)
{
    struct rpc_ev *r;
    munix_slot_t reply;
    const char *key;

    (void)fd;
    (void)what;
    r = (struct rpc_ev *)arg;
    if (!r->waiting) {
        return;
    }
    key = munix_take(r->mx, &reply, 0);
    if (key == NULL) {
        if (errno == EAGAIN) {
            return;
        }
        /* ESTALE: munix_close, reconnect, re-arm event */
        return;
    }
    if (reply != NULL) {
        munix_slot_free(reply);
    }
    r->waiting = 0;
}

static int rpc_ev_open(struct rpc_ev *r, struct event_base *base, const char *name, size_t need)
{
    int fd;

    r->mx = munix_connect(name);
    if (r->mx < 0) {
        return -1;
    }
    r->req = munix_slot_alloc(r->mx, need, -1, 0);
    if (r->req == NULL) {
        munix_close(r->mx);
        return -1;
    }
    fd = r->mx;
    r->ev = event_new(base, fd, EV_READ | EV_PERSIST, rpc_ev_readable, r);
    event_add(r->ev, NULL);
    r->waiting = 0;
    return 0;
}

static int rpc_ev_call(struct rpc_ev *r, const char *api)
{
    void *p;

    if (r->waiting) {
        errno = EBUSY;
        return -1;
    }
    p = munix_slot_data(r->req);
    (void)p; /* fill */
    if (munix_post(r->mx, api, r->req, MUNIX_POST_KEEP) != 0) {
        return -1;
    }
    r->waiting = 1;
    return 0;
}
```

---

### 10.9 KEEP vs non-KEEP

**Non-KEEP (typical one-shot RPC):**

```c
slot = munix_slot_alloc(mx, 64, 1000, 0);
/* fill */
munix_post(mx, "echo", slot, 0);   /* handle consumed on success */
key = munix_take(mx, &reply, 5000);
if (reply != NULL) {
    munix_slot_free(reply);
}
```

**KEEP (reuse one request buffer; safe after take):**

```c
slot = munix_slot_alloc(mx, 64, -1, 0);
for (;;) {
    /* fill slot */
    munix_post(mx, "echo", slot, MUNIX_POST_KEEP);
    key = munix_take(mx, &reply, -1);
    if (reply != NULL) {
        munix_slot_free(reply); /* peer side of KEEP request: drop handle only */
    }
    /* now safe to rewrite slot and post again */
}
munix_slot_free(slot); /* owner returns request slot to pool */
```

Server KEEP for reply slots is the same idea via `sunix_post(..., MUNIX_POST_KEEP)`.

**KEEP + process death / reclaim:**

- Shared slot marks `KEEP`; peer drop still sets `peer_done` (for cooperative live paths).
- `owner_pid` + `owner_birth` (starttime) for KEEP stays the **sender**; non-KEEP take still hands ownership to the receiver.
- Under **pool pressure**, any slot whose **owner is dead or whose pid was reused** (live pid, mismatched birth) is reclaimed — **including KEEP**, without requiring `peer_done`. Pressure reclaim snapshots owners then classifies **outside** the map lock (avoids holding the lock across `/proc`); slots with no reserved stamp or younger than ~2s are skipped (no `/proc`); steal/dirty repair under the lock uses **`kill`-only**.
- While the KEEP **owner is alive**, peers must not pool-free; reuse remains safe after take.
- If the peer drops and the owner is already dead, the peer freelists immediately.
- Take-path handle **`ENOMEM`** / bad wire length (`payload_len` > slot `payload_cap` → `EPROTO`): non-KEEP freelists the `POSTED` slot; **KEEP** leaves `POSTED` (sender handle remains valid). The unix datagram is already gone — **receiver take cannot retry that message**; KEEP only protects sender ownership / reclaim.
- **Orphan GRANT:** after a successful GRANT send the slot is `UNCLAIMED` until the client claims it in mmap. The same aged `mu_reclaim_dead` pass freelists `ALLOC`+`UNCLAIMED` older than ~2s (no `/proc`; 29-bit modular second stamp in `reserved`). Server in-map scan is on take `EAGAIN` when waiters remain, not on each in-map free. Late claim after reclaim is ignored (`slot_gen` / state check).
- **Orphan out POSTED (non-KEEP):** reply posted but never taken — under out-pool pressure, `POSTED` older than ~2s is freelisted (same stamp encoding). **KEEP replies** are not aged while the owner lives; **dead** KEEP owners are reclaimed by dead-owner pressure reclaim (not by POSTED age alone).
- Server death + new `listen` still uses `map_gen` / `ESTALE` (reconnect). KEEP is not a substitute for that.
- Map allocator (v6): slot-first + chunk `owner_slot`/`owner_gen`; freelist is rebuildable from the bump layout; age stamps use 29-bit seconds; map lock is `(pid, epoch)` in an ABI-sized word + **`dirty`** with mandatory reclaim+rebuild on steal/dirty; slot **state** for post/take is CAS (see §10.0 hybrid); slot owner is `(pid, starttime)`.

---

### 10.10 `MUNIX_MMAP_ONLY` vs default alloc

| | `flags = 0` | `MUNIX_MMAP_ONLY` |
|---|---|---|
| mmap freelist empty | may `GRANT_REQ` + wait (`timeout_ms`) | immediate `EAGAIN` (no fd) |
| sync client / startup loan | good default | optional |
| `select` / libevent hot path | avoid (POLLIN must retry **alloc**, not take) | preferred |

```c
/* sync / startup: allow GRANT wait */
slot = munix_slot_alloc(mx, 128, 3000, 0);

/* async hot path: never wait on unix for a slot */
slot = munix_slot_alloc(mx, 128, 0, MUNIX_MMAP_ONLY);
if (slot == NULL) {
    /* EAGAIN: pool busy — retry later or drop */
}
```

---

### 10.11 Control RPC (`mcontrol.h`)

Thin **RPC/control** over **munix + libevent + optional mxtalk**.

| Side | Flow |
|------|------|
| Server | `mcontrol_listen` → `mcontrol_fn` → sync post or `bind` + later `mcontrol_reply` |
| Client | `mcontrol_connect` → `alloc*` → fill → `mcontrol_call` → `mcontrol_free(rep)` → `mcontrol_close` |

Pool args `0` → default **8 × 64 KiB**. Munix rules in §10 still apply. This layer does **not** add a second corr or a second in-flight RPC on the same connect fd.

#### API

```c
typedef const char *(*mcontrol_fn)(int fd,
    const char *key, void *in, size_t in_len,
    munix_client_t client, void **out);

int mcontrol_listen(struct event_base *base, const char *object, mcontrol_fn control,
    int in_slots, size_t in_heap, int out_slots, size_t out_heap);

/* server out-slot alloc (non-blocking sunix) */
void        *mcontrol_salloc(int fd, size_t len);
mxtalk_t     mcontrol_salloc_m1talk(int fd, int max_l1, int name_max, int max_heap);
mxtalk_t     mcontrol_salloc_m2talk(int fd,
    int max_l1, int max_l2, int max_l2_pool, int name_max, int max_heap);

munix_client_t mcontrol_bind(void *p, munix_client_t client); /* heap copy; re-bind replaces */
munix_client_t mcontrol_getbind(void *p); /* bound peer, or NULL — probe only */
int            mcontrol_reply(const char *key, void *p);
void           mcontrol_free(void *p);   /* always safe; no-op if unknown / already consumed */
void           mcontrol_close(int fd);   /* sweep registry then munix_close */

/* client */
int          mcontrol_connect(const char *object);
void        *mcontrol_alloc(int fd, size_t len, int timeout_ms);
mxtalk_t     mcontrol_alloc_m1talk(int fd, int timeout_ms,
    int max_l1, int name_max, int max_heap);
mxtalk_t     mcontrol_alloc_m2talk(int fd, int timeout_ms,
    int max_l1, int max_l2, int max_l2_pool, int name_max, int max_heap);
const char  *mcontrol_call(int fd, const char *key, void *req, void **rep, int timeout_ms);

/* deprecated for sync *out — use *out = p; rare escape hatch to munix_slot_t */
munix_slot_t mcontrol_slot(void *p);
```

**Parameter order (`alloc*`):**

| API | Arguments |
|-----|-----------|
| `mcontrol_alloc` | `(fd, len, timeout_ms)` — bare buffer needs `len` |
| `mcontrol_alloc_m1talk` / `_m2talk` | `(fd, timeout_ms, geometry…)` — map size from geometry; **no** `len`, so `timeout` is 2nd |
| `mcontrol_salloc*` (server) | no timeout (non-blocking `sunix_slot_alloc`) |

#### Errors (`errno`)

Fail → `NULL` / `-1` with **`errno`**. Munix details remain in §10.3–10.5; this table is the **mcontrol** surface. **Business failure** on a completed RPC is the reply **key** (and/or body fields), **not** `errno`.

**`mcontrol_salloc` / `salloc_m1talk` / `salloc_m2talk`** (server; always non-blocking):

| errno | When |
|-------|------|
| `EINVAL` | `fd` is not this process’s listen/`mcontrol` ctx; bad `len` / geometry at munix or mxtalk; slot payload unusable |
| `EAGAIN` | out pool cannot satisfy now (no free slot **or** heap cannot fit `len` — munix often uses the same code for both) |
| `ENOMEM` | slot obtained but local handle / registry add failed |

No **`ETIMEDOUT`** on `salloc*` (no wait). On `slot == NULL`, mcontrol usually **preserves** munix `errno`. `salloc_m*talk`: `m*talk_map_size` / `create_mem` / `reg_add` failure may leave the helper’s `errno` (`EINVAL`, `ENOMEM`, …); not every branch rewrites it.

**`mcontrol_alloc` / `alloc_m1talk` / `alloc_m2talk`** (client; `timeout_ms` like `munix_slot_alloc`):

| errno | When |
|-------|------|
| `EINVAL` | not a connect/`mcontrol` ctx; `len==0` / overflow / bad geometry (munix or mxtalk) |
| `EAGAIN` | non-block (`timeout_ms==0`) or mmap-only: freelist/GRANT not ready; slot-empty vs heap-too-small often both `EAGAIN` |
| `ETIMEDOUT` | `timeout_ms > 0` and wait expired (clears pending GRANT wait per §10) |
| `ENOMEM` | slot obtained but register / create_mem path failed |

`alloc` after a timed-out `call` also **abandons** `reply_pending` (§10.3 / recovery below). Same note as `salloc*` for `alloc_m*talk` create/reg errno.

**`mcontrol_call`** (`post` then `take`; most codes are munix passthrough):

| Stage | errno | When / ownership |
|-------|-------|------------------|
| mcontrol | `EINVAL` | `key==NULL`; bad connect ctx; `req` not a registered `alloc*` pointer for this `fd` |
| post | `EBUSY` | previous reply still pending (often prior take timeout) — **this post did not run**; `req` still valid |
| post | `EAGAIN` | send buffer full — slot **not** consumed; retry same `req` when writable |
| post | `ESTALE` | session stale |
| post | `EINVAL` | bad key/slot/endpoint for munix |
| take | `ETIMEDOUT` | `timeout_ms > 0`, no matching reply in time — **does not** clear `reply_pending`; `req` already consumed if post succeeded |
| take | `EAGAIN` | `timeout_ms == 0` and no reply yet; same pending rule |
| take | `ESTALE` / `EPROTO` / `ENOMEM` | stale session / protocol slot break after a matching frame / reply slot register failed (§10.5) |

`timeout_ms` on `call` applies to **take only** (`<0` block). Successful `call` with a non-success **business** key still returns that key string — check the key/body, not `errno`.

**Other mcontrol surfaces (short):** `mcontrol_connect` / `listen` fail → munix or `ENOMEM`/`EINVAL`. Sync/`reply` send path: queue full → drop with **`ENOBUFS`**. See ownership table and recovery below for `EBUSY` / `ETIMEDOUT` / `EAGAIN`.

#### Payload model

- Same pointer kind everywhere: `in`, `*out`, client `req` / `*rep`, `salloc*` / `alloc*` → bare buffer **or** mxtalk map root.
- No body → `in == NULL`, `in_len == 0`.
- **`in_len` is munix slot cap** (usable size), **not** “bytes the peer wrote”. Frame inside the buffer yourself (`\0`, length prefix, m1/m2, …).
- Inbound m1/m2: `(mxtalk_t)in` for read, or `mxtalk_wrap(in, (uint32_t)in_len, 0)` then `mxtalk_detach` before return.
- Do **not** keep `in` after `control` returns (inbound slot is freed).
- Sync body: `*out = p` where `p` came from `mcontrol_salloc*` / `salloc_m1talk` / `salloc_m2talk`. **Do not** `*out = mcontrol_slot(p)`.

#### Munix mapping (corr, one-flight, EBUSY, drain / send)

- **`corr`:** client `mcontrol_call` = one `post` then `take` on that connect fd. Server sync post reuses the `peer` from `sunix_take`. Async: `mcontrol_bind` copies the whole `munix_client_st` (incl. `corr`); `mcontrol_reply` echoes it. Do not hand-build a peer and omit `corr`.
- **One in-flight RPC per connect fd.** Parallel RPCs → more `mcontrol_connect` fds. Listen fd is 1:N clients.
- **`EBUSY`:** post while a reply is still pending (typical: previous take timed out; §10.3). Clear with `mcontrol_alloc` (abandon) or `mcontrol_close` + reconnect. `errno==EBUSY` ⇒ **this** post did not run; `req` still valid until `mcontrol_free`.
- **`timeout_ms` on `mcontrol_call`:** take wait only (`<0` block, `0` → `EAGAIN`, `>0` → `ETIMEDOUT`). Post `EAGAIN` fails immediately. Timeout on take does **not** clear `reply_pending`.
- **Server drain:** `EV_READ|EV_PERSIST`; `on_read` loops `sunix_take` until `EAGAIN` (§10.5).
- **Send-when-writable:** sync/`reply` `sunix_post`; `EAGAIN` → queue → `EV_WRITE`. Queue full → drop reply (`ENOBUFS`).

#### Client recovery convention (EBUSY / ETIMEDOUT)

Business callers should **not** busy-loop `mcontrol_call` after a failed take or a busy post. Recommended wrapper policy:

1. **`mcontrol_call` returns `NULL`** and `errno` is **`EBUSY`**, **`ETIMEDOUT`**, or **`EAGAIN`** (when `timeout_ms == 0`): treat as recoverable transport state for **this** connect fd.
2. **Always** `mcontrol_free(req)` if you still hold a request pointer (safe no-op if post already consumed it).
3. **Abandon or reconnect** before retrying:
   - **Abandon (keep fd):** `tmp = mcontrol_alloc(fd, small_len, short_timeout);` then `mcontrol_free(tmp);` — alloc clears `reply_pending` (§10.3). If alloc fails, fall through to reconnect.
   - **Reconnect:** `mcontrol_close(fd);` `fd = mcontrol_connect(object);`
4. **Retry** the original RPC with **backoff** (e.g. sleep / exponential delay, capped). Bound the retry count; then surface the error to the caller.
5. Parallel work → **separate** `mcontrol_connect` fds (still one in-flight per fd), sized against server `in_slots` / `out_slots`.

Sketch (app glue — not a libskin API):

```c
/* after: rkey = mcontrol_call(fd, key, req, &rep, ms); mcontrol_free(req); */
if (rkey == NULL) {
    err = errno;
    if (err == EBUSY || err == ETIMEDOUT || err == EAGAIN) {
        tmp = mcontrol_alloc(fd, 32, 100);
        if (tmp != NULL) {
            mcontrol_free(tmp);          /* abandon pending round */
        } else {
            mcontrol_close(fd);
            fd = mcontrol_connect(object); /* or fail out */
        }
        /* backoff, then retry alloc+call once/N times */
    }
}
```

#### Server contract

| Intent | Pattern |
|--------|---------|
| Sync, key only | `return "ttrue"` (or any key); leave `*out` NULL |
| Sync, with body | `p = mcontrol_salloc*(…)`, fill, `*out = p`, `return ""` (or key) |
| Async | `p = salloc*`, `mcontrol_bind(p, client)`, **do not** set `*out`, `return NULL`; later `mcontrol_reply(key, p)` |
| Drop | `return NULL` **without** `bind` (usually no `salloc` either) |

**`return NULL` — async vs drop:**

`NULL` only means “`on_read` will **not** sync-post this turn”. Whether a reply comes later depends **only** on `bind`:

- **Async:** `bind` first, then `return NULL`. Optional probe: `mcontrol_getbind(p) != NULL`. Later `mcontrol_reply`.
- **Drop:** never `bind`. Typical: no alloc, just `return NULL`. Client take times out.
- Forgetting `bind` on an intended async path **silently becomes drop**.
- `getbind` is a **probe**, not the definition of drop (drop often has no `p` at all).

**Illegal `*out`:**

`*out` must be a **registered** `salloc*` payload for **this** listen fd. If `*out != NULL` but `reg_find` fails (malloc, stack, foreign pointer, already freed), the library **drops the body** and still posts **key only**. Prefer leaving `*out` NULL over guessing.

Other notes: mis-bind + sync `*out` → on_read strips heap peer and still sync-posts. Reply `key` / `rkey` must **not** point into `in` (inbound slot freed before post). Optional cookie: `munix_client_set_data(peer, biz)` — free `biz` yourself before `reply`/`free` (munix never frees cookies).

#### Client `req` / `rep` ownership

`mcontrol_call` = post then take.

| Event | Dereference `req`? | `mcontrol_free(req)`? |
|-------|--------------------|------------------------|
| post **failed** (`EBUSY` / `EAGAIN` / `EINVAL` / …) | yes, until free | **yes** (still registered) |
| post **succeeded** (even if take fails: `ETIMEDOUT` / `EAGAIN` / `ESTALE`) | **no** (dangling; mmap may be reused) | **yes, always safe** — no-op if already consumed |
| success with `*rep` | n/a | `mcontrol_free(rep)` when done |

Recommended pattern: **always** `mcontrol_free(req)` after `mcontrol_call` returns (success or failure); never read/write `req` after a successful post. `rkey` is munix-internal until the next successful take on that fd — copy it if you need it later. `rep == NULL` in the call discards any reply body immediately.

#### Server example (sync + async, key / void* / m1 / m2)

```c
#include "skin/skin.h"

/* queue_or_timer / later() are app glue — not part of mcontrol */

static const char *station_control(int fd, const char *key,
    void *in, size_t in_len, munix_client_t client, void **out)
{
    void *p;
    mxtalk_t tin;
    mxtalk_t tout;
    munix_client_t peer;
    const char *s;
    size_t n;

    if (key == NULL) {
        return terror_string;
    }

    /* ---- sync: key only ---- */
    if (strcmp(key, "ping") == 0) {
        (void)fd; (void)in; (void)in_len; (void)client; (void)out;
        return "ttrue";
    }

    /* ---- drop: NULL, no bind (no alloc) ---- */
    if (strcmp(key, "drop") == 0) {
        (void)fd; (void)in; (void)in_len; (void)client; (void)out;
        return NULL;
    }

    /* ---- sync: bare void* echo ---- */
    if (strcmp(key, "echo") == 0) {
        (void)client;
        n = 64;
        if (in != NULL && in_len > 0) {
            n = in_len;
            if (n > 256) {
                n = 256;
            }
        }
        p = mcontrol_salloc(fd, n);
        if (p == NULL) {
            return terror_string;
        }
        if (in != NULL && in_len > 0) {
            memcpy(p, in, n); /* in_len is cap — frame if you need exact length */
        }
        *out = p; /* must be salloc pointer; not mcontrol_slot(p) */
        return "";
    }

    /* ---- sync: m1talk in / m1talk out ---- */
    if (strcmp(key, "m1") == 0) {
        (void)client;
        if (in != NULL && in_len > 0) {
            tin = (mxtalk_t)in; /* or mxtalk_wrap(in, (uint32_t)in_len, 0) */
            s = mxtalk_string(tin, "ifname");
            (void)s;
        }
        tout = mcontrol_salloc_m1talk(fd, 32, 32, 4 * 1024);
        if (tout == NULL) {
            return terror_string;
        }
        mxtalk_set_string(tout, "ifname", "br0");
        *out = tout;
        return "";
    }

    /* ---- sync: m2talk in / m2talk out ---- */
    if (strcmp(key, "m2") == 0) {
        (void)client;
        if (in != NULL && in_len > 0) {
            tin = (mxtalk_t)in;
            s = mxtalk_string(tin, "name");
            (void)s;
        }
        tout = mcontrol_salloc_m2talk(fd, 16, 32, 32, 32, 8 * 1024);
        if (tout == NULL) {
            return terror_string;
        }
        mxtalk_set_string(tout, "name", "wan");
        *out = tout;
        return "";
    }

    /* ---- async: bind + return NULL; reply later ---- */
    if (strcmp(key, "async") == 0) {
        (void)in; (void)in_len; (void)out;
        p = mcontrol_salloc(fd, 64);
        if (p == NULL) {
            return terror_string;
        }
        peer = mcontrol_bind(p, client);
        if (peer == NULL) {
            mcontrol_free(p);
            return terror_string;
        }
        /* optional: munix_client_set_data(peer, biz); free biz before reply/free */
        (void)peer;
        queue_or_timer(p); /* app schedules later(p) */
        return NULL; /* bound → async; forgetting bind → silent drop */
    }

    return terror_string;
}

static void later(void *p)
{
    /* key-only async reply; or fill p then mcontrol_reply("", p) */
    mcontrol_reply("ttrue", p);
}

/* in _service: */
fd = mcontrol_listen(base, "client@station", station_control,
    8, 64 * 1024, 8, 64 * 1024);
```

#### Client examples (key / void* / m1 / m2)

```c
#include "skin/skin.h"

/* ---- key only ---- */
fd = mcontrol_connect("client@station");
rkey = mcontrol_call(fd, "ping", NULL, NULL, 3000);
if (rkey == NULL || strcmp(rkey, "ttrue") != 0) {
    /* handle error */
}
mcontrol_close(fd);

/* ---- bare void* ---- */
fd = mcontrol_connect("client@station");
req = mcontrol_alloc(fd, 64, 1000);          /* (fd, len, timeout) */
if (req == NULL) {
    mcontrol_close(fd);
    return -1;
}
snprintf((char *)req, 64, "hello");
rep = NULL;
rkey = mcontrol_call(fd, "echo", req, &rep, 3000);
mcontrol_free(req); /* always safe: real free or no-op if post consumed */
if (rkey == NULL) {
    mcontrol_close(fd);
    return -1;
}
/* do not dereference req here if post succeeded */
if (rep != NULL) {
    /* use (char *)rep … */
    mcontrol_free(rep);
}
mcontrol_close(fd);

/* ---- m1talk ---- */
fd = mcontrol_connect("client@station");
req = mcontrol_alloc_m1talk(fd, 1000, 32, 32, 4 * 1024); /* (fd, timeout, geometry) */
if (req == NULL) {
    mcontrol_close(fd);
    return -1;
}
mxtalk_set_string((mxtalk_t)req, "ifname", "eth0");
rep = NULL;
rkey = mcontrol_call(fd, "m1", req, &rep, 3000);
mcontrol_free(req);
if (rkey == NULL) {
    mcontrol_close(fd);
    return -1;
}
if (rep != NULL) {
    s = mxtalk_string((mxtalk_t)rep, "ifname");
    (void)s;
    mcontrol_free(rep);
}
mcontrol_close(fd);

/* ---- m2talk ---- */
fd = mcontrol_connect("client@station");
req = mcontrol_alloc_m2talk(fd, 1000, 16, 32, 32, 32, 8 * 1024);
if (req == NULL) {
    mcontrol_close(fd);
    return -1;
}
mxtalk_set_string((mxtalk_t)req, "name", "lan");
rep = NULL;
rkey = mcontrol_call(fd, "m2", req, &rep, 3000);
mcontrol_free(req);
if (rkey == NULL) {
    mcontrol_close(fd);
    return -1;
}
if (rep != NULL) {
    s = mxtalk_string((mxtalk_t)rep, "name");
    (void)s;
    mcontrol_free(rep);
}
mcontrol_close(fd);
```

---

## 11. Logging API (log.h)

### 11.0 Summary

`log.h` defines **severity levels**, **output options**, and **subsystem type/subtype** constants that are packed into a single **`unsigned int flags`** passed to **`landlog()`**. The runtime splits `flags` into level, options, type, and subtype, then filters against **`register`** keys such as **`log_mask`** / **`log_options`** before formatting and writing (TUI, syslog, file, etc.).

| Entry point | Role |
|-------------|------|
| **`landlog(flags, file, line, fmt, …)`** | Low-level logger; usually invoked via **`journal(flags, …)`** or the predefined macros (`default_info`, `network_debug`, `shell_fault`, …). |
| **Macros** | Families include **default_***, **shell_***, **land_***, **auth_***, **network_***, … — each expands to a fully composed `flags` word plus `__FILE__` / `__LINE__`. |

**Composing custom `flags`:** OR **level** + **options** + **(type shifted by `LANDLOG_TYPE_OFFSET`)** + **(subtype shifted by `LANDLOG_SUBTYPE_OFFSET`)** — see **§11.3.1** and the `LANDLOG_*_OFFSET` / `*_MASK` macros in `log.h`. **`LANDLOG_ERRNO`** appends `strerror(errno)` when set.

---

### 11.1 Log Levels

```c
#define LANDLOG_FAULT     (1)    // Fault
#define LANDLOG_WARN      (2)    // Warning
#define LANDLOG_INFO      (4)    // Information
#define LANDLOG_DEBUG     (8)    // Debug
#define LANDLOG_VERBOSE   (16)   // Verbose
```

### 11.2 Log Options

```c
#define LANDLOG_TUI       (1<<8)   // Output to TUI
#define LANDLOG_SYSLOG    (2<<8)   // Output to system log
#define LANDLOG_FILE      (4<<8)   // Output to file
#define LANDLOG_TRACE     (8<<8)   // Output trace info
#define LANDLOG_ERRNO     (16<<8)  // Include errno
```

### 11.3 Log Types

| Type | Description |
|------|-------------|
| LANDLOG_DEFAULT | Default type |
| LANDLOG_LAND | Land system |
| LANDLOG_ARCH | Hardware architecture |
| LANDLOG_NETWORK | Network |
| LANDLOG_IFNAME | Interface name |
| LANDLOG_AGENT | Agent |
| LANDLOG_CENTER | Center services |
| LANDLOG_WUI | Web UI |
| LANDLOG_CLIENT | Client |
| LANDLOG_MODEM | Modem |
| LANDLOG_UART | UART |
| LANDLOG_VPN | VPN |

Names in this table are **type / subsystem identifiers** (`LANDLOG_LAND`, `LANDLOG_NETWORK`, …). In real `flags` values they occupy the **type** bit field (see below), not the low byte alone.

### 11.3.1 Composing `flags` for `landlog()`

`landlog()` treats `flags` as a bit layout in the implementation:

| Field | Bits (conceptually) | Macros |
|-------|---------------------|--------|
| Level | low byte | `LANDLOG_FAULT`, `LANDLOG_WARN`, `LANDLOG_INFO`, `LANDLOG_DEBUG`, `LANDLOG_VERBOSE` (`LANDLOG_LEVEL_MASK`, `LANDLOG_LEVEL_OFFSET`) |
| Options | next byte | `LANDLOG_TUI`, `LANDLOG_SYSLOG`, `LANDLOG_FILE`, `LANDLOG_TRACE`, `LANDLOG_ERRNO` (`LANDLOG_OPTION_MASK`, `LANDLOG_OPTION_OFFSET`) |
| Type | next byte | e.g. `(LANDLOG_LAND << LANDLOG_TYPE_OFFSET)` (`LANDLOG_TYPE_MASK`, `LANDLOG_TYPE_OFFSET`) |
| Subtype | high byte | e.g. `(LANDLOG_LAND_DEFAULT << LANDLOG_SUBTYPE_OFFSET)` (`LANDLOG_SUBTYPE_MASK`, `LANDLOG_SUBTYPE_OFFSET`) |

The predefined macros such as `default_info(...)` expand to a full combination, for example  
`(LANDLOG_DEFAULT << LANDLOG_TYPE_OFFSET) | (LANDLOG_DEFAULT_NONE << LANDLOG_SUBTYPE_OFFSET) | LANDLOG_INFO`.  
When building custom flags, OR **shifted** type/subtype constants with level and options instead of using raw `LANDLOG_LAND` alone as the entire `flags` word.

### 11.4 Log Functions

#### landlog
```c
void landlog(unsigned int flags, const char *filename, int line, const char *format, ...);
```
**Description:** Internal logging function

#### journal
```c
#define journal(flags, ...) landlog((flags), (__FILE__), (__LINE__), __VA_ARGS__)
```
**Description:** Log with identifier

#### critical_warn
```c
void critical_warn(const char *format, ...);
```
**Description:** Dedicated critical-path warning (not the `*_warn` macros). Use for rare, high-severity operational alerts; see `log.h`.

### 11.5 Predefined Log Macros

**Default Logs:**
- default_verbose / default_debug / default_info / default_warn / default_warning / default_fault / default_faulting

**Shell Logs:**
- shell_verbose / shell_debug / shell_info / shell_warn / shell_warning / shell_fault / shell_faulting

**Land Logs:**
- land_verbose / land_debug / land_info / land_warn / land_warning / land_fault / land_faulting

**Auth Logs:**
- auth_verbose / auth_debug / auth_info / auth_warn / auth_warning / auth_fault / auth_faulting

**Network Logs:**
- network_verbose / network_debug / network_info / network_warn / network_warning / network_fault / network_faulting

**Example:**
```c
// Log information
land_info("System started successfully");

// Log debug
network_debug("Interface %s is up", "eth0");

// Log warning (with errno)
land_warning("Failed to open file: %s", filename);

// Use journal for custom type
journal((LANDLOG_NETWORK<<16)|(LANDLOG_NETWORK_DEFAULT<<24)|LANDLOG_INFO, 
        "Custom log entry");
```

---

## 12. Service Management API (serv.h)

### 12.0 Summary

Service APIs delegate to the **service management component** (see `SERVICE_COM` / `land@service` in `skinhead.h`). They schedule or control **named** background workers that invoke a component API (`com` + `api` + parameters). On **`srun` / `crun` / …**, the **`delay`** argument is a **seconds** count before the service is actually started (see `serv.h`).

**Naming:** **`s*`** variants take the component as a **`const char *`** (e.g. `"land@machine"`); **`c*`** variants take an **`obj_t`**.

**Operation families:**

| Family | Typical entry points | Behavior (per `serv.h`) |
|--------|----------------------|-------------------------|
| Run | `srun`, `srunt`, `srun2t`, `sruns`, … | Register and start; **stops and deletes** an existing service with the **same name** first. |
| Reset | `sreset`, `sresett`, `sreset2t`, `sresets`, … | If not registered, register; if already present, **restart**. |
| Start | `sstart`, `sstartt`, `sstart2t`, `sstartst`, `sstarts`, … | **Start only if not already running** (idempotent start). |
| Stop / remove | `sdelete`, `sstop`, `soff`, `soffdel` | Delete; stop; **pause** (stay registered); pause **and** delete registration. |
| Query | `spid`, `sinfo`, `sdump`, `slist` | PID, JSON info / dump, full list — caller **`talk_free()`** on returned `talk_t` where applicable. |

**Low-level:** `serv_call(cmd, v, timeout)` sends a command to the daemon. The **`v`** argument is **ownership-transferred** (freed inside the implementation). Returns JSON (`talk_free` when done), **`ttrue`** (ok, no payload), **`terror`**, or **`tpanic`** (timeout / IPC failure); see `serv.h` for details.

---

### 12.1 Service Register and Run

#### srun / crun / srunt / crunt / srun2t / crun2t / sruns / cruns
```c
boole srun(int delay, const char *com, const char *api, param_t param, const char *nameformat, ...);
boole crun(int delay, obj_t com, const char *api, param_t param, const char *nameformat, ...);
boole srunt(int delay, const char *com, const char *api, talk_t json, const char *nameformat, ...);
boole crunt(int delay, obj_t com, const char *api, talk_t json, const char *nameformat, ...);
boole srun2t(int delay, const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...);
boole crun2t(int delay, obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...);
boole sruns(int delay, const char *name, const char *com, const char *api, const char *paramformat, ...);
boole cruns(int delay, const char *name, obj_t com, const char *api, const char *paramformat, ...);
```
**Description:** Register and run service (stops same-name service first)
**Parameters:**
- delay - Delay in seconds
- com - Component
- api - API name
- param/json - Parameters
- nameformat - Service name format

### 12.2 Service Reset

#### sreset / creset / sresett / cresett / sreset2t / creset2t / sresets / cresets
```c
boole sreset(const char *com, const char *api, param_t param, const char *nameformat, ...);
boole creset(obj_t com, const char *api, param_t param, const char *nameformat, ...);
```
**Description:** Reset or start service (registers if not exists)

### 12.3 Service Start

#### sstart / cstart / sstartt / cstartt / sstart2t / cstart2t / sstarts / cstarts
```c
boole sstart(const char *com, const char *api, param_t param, const char *nameformat, ...);
boole cstart(obj_t com, const char *api, param_t param, const char *nameformat, ...);
```
**Description:** Start service

### 12.4 Service Control

#### sdelete / sstop / soff / soffdel
```c
boole sdelete(const char *nameformat, ...);
boole sstop(const char *nameformat, ...);
boole soff(const char *nameformat, ...);
boole soffdel(const char *nameformat, ...);
```
**Description:** Delete/Stop/Off/Off and delete service

### 12.5 Service Query

#### spid / sinfo / sdump / slist
```c
pid_t spid(const char *nameformat, ...);
talk_t sinfo(const char *nameformat, ...);
talk_t sdump(const char *nameformat, ...);
talk_t slist(void);
```
**Description:** Get service PID/info/detailed info/list

**Example:**
```c
// Run service (delay 5 seconds)
sruns(5, "wan_monitor", "network@wan", "monitor", "eth0");

// Stop service
sstop("wan_monitor");

// Delete service
sdelete("wan_monitor");

// Get service PID
pid_t pid = spid("wan_monitor");

// Get all service list
talk_t list = slist();
talk_free(list);
```

### 12.6 Sample program (every `serv.h` function)

**`serv_call`'s `v` argument is ownership-transferred** (freed inside the implementation). **`slist()`** passes **`NULL`**. All other calls need a **running service daemon** to succeed.

```c
#include "skin.h"

static void demo_serv_all(void)
{
    obj_t o = obj_create("land@machine");
    param_t pm = param_create("a");
    talk_t j = string2json("{\"k\":1}");
    talk_t j2 = string2json("{\"k\":2}");
    talk_t r;

    r = serv_call("list", NULL, 1);
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);
    r = serv_call("dump", string2json("{\"name\":\"x\"}"), 1);
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);

    (void)srun(0, "land@machine", "status", pm, "demo_svc");
    (void)crun(0, o, "status", pm, "demo_svc");
    (void)srunt(0, "land@machine", "status", j, "demo_svc");
    (void)crunt(0, o, "status", j, "demo_svc");
    (void)srun2t(0, "land@machine", "status", j, j2, "demo_svc");
    (void)crun2t(0, o, "status", j, j2, "demo_svc");
    (void)sruns(0, "demo_named", "land@machine", "status", "%s", "a");
    (void)cruns(0, "demo_named", o, "status", "%s", "a");

    (void)sreset("land@machine", "status", pm, "demo_svc");
    (void)creset(o, "status", pm, "demo_svc");
    (void)sresett("land@machine", "status", j, "demo_svc");
    (void)cresett(o, "status", j, "demo_svc");
    (void)sreset2t("land@machine", "status", j, j2, "demo_svc");
    (void)creset2t(o, "status", j, j2, "demo_svc");
    (void)sresets("demo_named", "land@machine", "status", "%s", "a");
    (void)cresets("demo_named", o, "status", "%s", "a");

    (void)sstart("land@machine", "status", pm, "demo_svc");
    (void)cstart(o, "status", pm, "demo_svc");
    (void)sstartt("land@machine", "status", j, "demo_svc");
    (void)cstartt(o, "status", j, "demo_svc");
    (void)sstart2t("land@machine", "status", j, j2, "demo_svc");
    (void)cstart2t(o, "status", j, j2, "demo_svc");
    (void)sstartst("land@machine", "status", "{}", j2, "demo_svc");
    (void)cstartst(o, "status", "{}", j2, "demo_svc");
    (void)sstarts("demo_named", "land@machine", "status", "%s", "a");
    (void)cstarts("demo_named", o, "status", "%s", "a");

    (void)sdelete("demo_svc");
    (void)sstop("demo_svc");
    (void)soff("demo_svc");
    (void)soffdel("demo_svc");
    (void)spid("demo_svc");

    r = sinfo("demo_svc");
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);
    r = sdump("demo_svc");
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);
    r = slist();
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);

    talk_free(j2);
    talk_free(j);
    param_free(pm);
    obj_free(o);
}
```

---

## 13. Project Information API (project.h)

### 13.0 Summary

`project.h` manages the **multi-project filesystem layout** (`PROJECT_INFOFILE` / **`prj.json`**, default version **`PROJECT_DEFAULT_VERSION`**).

| Area | APIs |
|------|------|
| **Discovery** | **`project_scan`** (refresh), **`project_list`** (cache), **`project_dirty`**, **`project_check`**. |
| **Paths** | **`project_path`**, **`project_storage`**, **`project_osc_path`**, **`project_var_path`**, **`project_internal_path`** — plus **`project2path`**, **`exe2path`**, … macros using **`PROJECT_ID`**. |
| **Bootstrapping** | **`project_add_init` / `project_add_uninit` / `project_add_joint` / `project_add_object`** register entries in project metadata. |
| **i18n** | **`project_i18n`**, **`project_i18n_get`**. |

Most functions return paths into **`buffer`** or heap **`talk_t`** lists — **`talk_free`** when applicable; see each prototype in `project.h`.

---

### 13.1 Project Scan and List

#### project_scan / project_list / project_dirty
```c
talk_t project_scan(void);
talk_t project_list(void);
void project_dirty(void);
```
**Description:** Scan/List/Mark project list as dirty

#### project_check
```c
boole project_check(const char *name, const char *prjpath);
```
**Description:** Check project JSON format

### 13.2 Project Paths

#### project_path / project2path
```c
const char *project_path(char *buffer, int buflen, const char *name);
#define project2path(buffer, buflen) project_path(buffer, buflen, PROJECT_ID)
```
**Description:** Get project directory

#### project_storage / project2storage
```c
const char *project_storage(char *buffer, int buflen, const char *name, const char *type);
#define project2storage(buffer, buflen, type) project_storage(buffer, buflen, PROJECT_ID, type)
```
**Description:** Get project storage directory

#### project_osc_path / osc2path / project_var_path / var2path / project_internal_path / internal2path
```c
const char *project_osc_path(char *buffer, int buflen, const char *name, const char *execute, ...);
#define exe2path(buffer, buflen, ...) project_osc_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
const char *project_var_path(char *buffer, int buflen, const char *name, const char *variable, ...);
#define var2path(buffer, buflen, ...) project_var_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
const char *project_internal_path(char *buffer, int buflen, const char *name, const char *variable, ...);
#define internal2path(buffer, buflen, ...) project_internal_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
```
**Description:** Get executable/variable/internal file paths

### 13.3 Project Configuration

#### project_add_init / project_add_uninit / project_add_joint / project_add_object
```c
boole project_add_init(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_uninit(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_joint(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_object(const char *name, const char *prjpath, const char *object, const char *com);
```
**Description:** Add init/uninit/event/object operations

### 13.4 Internationalization

#### project_i18n / project_i18n_get
```c
talk_t project_i18n(const char *project, const char *prefix);
const char *project_i18n_get(talk_t lang, const char *text);
```
**Description:** Get language JSON / Get translated text

**Example:**
```c
// Get project list
talk_t projects = project_list();
talk_free(projects);

// Get current project path
char path[PATH_MAX];
project2path(path, sizeof(path));

// Get executable path
exe2path(path, sizeof(path), "myapp");
```

### 13.5 Sample program (every `project.h` API)

Uses **`PROJECT_ID`** for the **`project2path` / `exe2path` / …** macros — define it before **`#include "skin.h"`** if your build does not already (for example **`-DPROJECT_ID=land`**).

```c
#ifndef PROJECT_ID
#define PROJECT_ID "land"
#endif
#include "skin.h"

static void demo_project_all(void)
{
    char buf[PATH_MAX];
    talk_t t;
    talk_t lang;

    t = project_scan();
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = project_list();
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    project_dirty();
    (void)project_check("land", "/PRJ");

    (void)project_path(buf, sizeof buf, "land");
    (void)project2path(buf, sizeof buf);
    (void)project_storage(buf, sizeof buf, "land", "config");
    (void)project2storage(buf, sizeof buf, "config");
    (void)project_osc_path(buf, sizeof buf, "land", "openvpn");
    (void)osc2path(buf, sizeof buf, "openvpn");
    (void)ko2path(buf, sizeof buf, "drv.ko");
    (void)misc2path(buf, sizeof buf, "m.bin");
    (void)project_var_path(buf, sizeof buf, "land", "x");
    (void)var2path(buf, sizeof buf, "x");
    (void)project_internal_path(buf, sizeof buf, "land", "i");
    (void)internal2path(buf, sizeof buf, "i");

    (void)project_add_init("land", "/PRJ", "app", "land@machine.init");
    (void)project_add_uninit("land", "/PRJ", "app", "land@machine.stop");
    (void)project_add_joint("land", "/PRJ", "ev", "land@machine.on");
    (void)project_add_object("land", "/PRJ", "machine", "land@machine");

    lang = project_i18n("land", "net");
    if (lang > (void *)tpanic && lang && json_check(lang)) {
        (void)project_i18n_get(lang, "k");
        talk_free(lang);
    }
}
```

---

## 14. HE Command API (he2com.h)

### 14.0 Summary

`he2com.h` parses and executes **HE** strings — the CLI/config language mapping to **get/set/call** on components (similar to shell `project@component:attr=value` or `project@component.method[param]`).

| Piece | Role |
|-------|------|
| **Types** | **`HE_GET` / `HE_SET` / `HE_OR` / `HE_CALL` / `HE_DBS_*`** — operation kind stored in **`he_t`’s `flags` field**. |
| **`he_t`** | Parsed command: object, file attr, method, **`param_t`**, value JSON, buffers — **`he_free`**. |
| **Parse** | **`argv2he`**, **`string2he`**, **`json2he`**; **serialize** **`he2json`**, **`he2string`**. |
| **Execute** | **`he_execute`**, **`string_he_execute`**, **`json_he_execute`**, **`talk_he_command`** (batch array); **`line_he_command`** for terminal-oriented use. |

Results are usually **`talk_t`** JSON (**`talk_free`** if not a sentinel) or **`ttrue` / `tfalse` / `NULL` / `terror` / `tpanic`** per `he2com.h`. Ownership: anything attached to **`he_t`** is freed by **`he_free`**; execution APIs document whether returned JSON is new.

---

### 14.1 HE Command Types

```c
#define HE_GET       0      // Get configuration value
#define HE_SET       1      // Set configuration value
#define HE_OR        2      // Modify configuration value
#define HE_CALL      3      // API call
#define HE_DBS_GET   4      // Get database value
#define HE_DBS_SET   5      // Set database value
#define HE_DBS_OR    6      // Modify database value
#define HE_DBS_CALL  7      // Database API call
```

### 14.2 HE Structure Operations

#### argv2he / string2he / json2he
```c
he_t argv2he(int argc, const char **argv);
he_t string2he(const char *cmd);
he_t json2he(talk_t cmd);
```
**Description:** Parse HE from **`main` argv** (used by **`MAIN2API`** CLI path), a command string, or JSON. Caller **`he_free`**.
#### he2json / he2string
```c
talk_t he2json(he_t h);
const char *he2string(he_t h);
```
**Description:** Convert HE structure to JSON/string

#### he_free
```c
void he_free(he_t h);
```
**Description:** Free HE structure

### 14.3 HE Command Execution

#### he_execute / string_he_execute / line_he_command / json_he_execute / talk_he_command
```c
talk_t he_execute(he_t h);
talk_t string_he_execute(const char *cmd);
int line_he_command(const char *cmd);
talk_t json_he_execute(talk_t cmd);
talk_t talk_he_command(talk_t cmd);
```
**Description:** Execute HE command

**Example:**
```c
// Execute string HE command
talk_t result = string_he_execute("land@machine.status");
if (result > (void *)tpanic && json_check(result)) {
    talk_free(result);
}

// Execute with parameters
result = string_he_execute("network@frame.set{\"ip\":\"192.168.1.1\"}");

// Line command (prints to terminal)
int rc = line_he_command("land@machine.version");
```

### 14.4 Sample program (every `he2com.h` function)

The **`HE_*`** symbols are **integer constants** (see §12.1); the first line forces them to appear in the snippet. **`json2he()`** expects at least **`"obj"`**; for a call shape, add **`"op"`** with the method name (see **`he2com.h`** / HE JSON shape).

```c
#include "skin.h"

static void demo_he2com_all(void)
{
    he_t h;
    talk_t t;
    talk_t cmdj;
    talk_t hj;
    talk_t batch;

    (void)(HE_GET | HE_SET | HE_OR | HE_CALL | HE_DBS_GET | HE_DBS_SET | HE_DBS_OR | HE_DBS_CALL);

    {
        const char *av[] = { "demo", "land@machine.status" };
        h = argv2he(2, av);
        if (h) he_free(h);
    }

    h = string2he("land@machine.status");
    t = he2json(h);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    (void)he2string(h);
    he_free(h);

    cmdj = string2json("{\"obj\":\"land@machine\",\"op\":\"status\"}");
    hj = json2he(cmdj);
    talk_free(cmdj);
    if (hj) {
        t = he2json(hj);
        if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
        (void)he2string(hj);
        t = he_execute(hj);
        if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
        he_free(hj);
    }

    t = string_he_execute("land@machine.status");
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);

    (void)line_he_command("land@machine.version");

    cmdj = string2json("{\"obj\":\"land@machine\",\"op\":\"status\"}");
    t = json_he_execute(cmdj);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    talk_free(cmdj);

    batch = string2json("{\"r\":{\"obj\":\"land@machine\",\"op\":\"status\"}}");
    t = talk_he_command(batch);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    talk_free(batch);
}
```

---

## 15. Linked List API (link.h)

### 15.1 Macro Definitions

#### link_entry
```c
#define link_entry(node, type, member) ((type*)((char*)(node) - (unsigned long)(&((type*)0)->member)))
```
**Description:** Get actual data structure from linked list node

#### link_each
```c
#define link_each(var, head) for((var) = (*head); NULL != (var); (var = (var)->next))
```
**Description:** Iterate through linked list

### 15.2 Linked List Operations

#### link_init
```c
boole link_init(link_t node);
```
**Description:** Initialize linked list node

#### link_child
```c
link_t link_child(lhead_t head);
```
**Description:** Get first node of linked list

#### link_size
```c
int link_size(lhead_t head);
```
**Description:** Get node count in linked list

#### link_delete / link_remove
```c
boole link_delete(lhead_t head, link_t elm);
link_t link_remove(lhead_t head, link_t elm);
```
**Description:** Delete node from linked list

#### link_push / link_pop
```c
boole link_push(lhead_t head, link_t elm);
link_t link_pop(lhead_t head);
```
**Description:** Push/Pop node at end of linked list

#### link_insert / link_pull
```c
boole link_insert(lhead_t head, link_t elm, link_t next);
link_t link_pull(lhead_t head);
```
**Description:** Insert/Pull node at specified position

**Example:**
```c
// Define structure containing linked list node
struct my_struct {
    int data;
    link_struct link;
};

// Initialize list head
link_struct *head = NULL;

// Create node
struct my_struct *node = malloc(sizeof(struct my_struct));
node->data = 42;
link_init(&node->link);

// Add to list
link_push(&head, &node->link);

// Iterate list
link_t pos;
link_each(pos, &head) {
    struct my_struct *entry = link_entry(pos, struct my_struct, link);
    printf("%d\n", entry->data);
}
```

---

## 16. Utility Functions API (utility.h)

This chapter lists the most common entry points. **`utility.h` declares many more helpers** (network, shell, sockets, UART, time, etc.); refer to the header for full prototypes.

**Conventions (typical patterns, not a guarantee for every symbol):**

- Many functions indicate failure with **negative integers** or **`false`** and set **`errno`** where a system call failed; check the declaration’s comment in `utility.h`.
- Allocating helpers (`md5_encode`, `b64_encode`, `url_encode`, …) usually return **heap-allocated** buffers; **caller frees** unless the header states otherwise.
- **Thread safety:** utilities are not uniformly re-entrant; treat global process state, static buffers (if any), and subprocess/shell helpers as **non-thread-safe** unless documented.

### 16.1 String Processing

#### char2char / low2upp / upp2low
```c
void char2char(char *src, char a, char b);
void low2upp(char *str);
void upp2low(char *str);
```
**Description:** **`char2char`** replaces every **`a`** with **`b`** in-place in **`src`** (NUL-terminated). **`low2upp`** / **`upp2low`** convert the **entire** string in place using **`toupper` / `tolower`** on **`(unsigned char)`** bytes. **`NULL` `src` / `str`** → **`EINVAL`** and no-op (as documented for the encoding helpers).

### 16.2 Encoding/Decoding

#### md5_encode / b64_encode / b64_decode
```c
char *md5_encode(const char *s, int len);
char *b64_encode(const char *s, int len);
char *b64_decode(const char *s, int *len);
```
**Description:** MD5 digest as **hex string**; Base64 encode/decode. **`md5_encode`**: **`NULL`** / invalid length → **`NULL`** with **`EINVAL`**; allocation failure → **`NULL`** with **`ENOMEM`**. Caller frees returned strings.

#### url_encode / url_decode
```c
char *url_encode(char const *s, int len, int *new_length);
int url_decode(char *str, int len);
```
**Description:** URL encode/decode

#### simple_encode / simple_decode
```c
char *simple_encode(const char *message, const char *tok);
char *simple_decode(const char *message, const char *tok);
```
**Description:** **AES-128-CBC** (key/IV derived from **`tok`** and fixed salt in the implementation), then **Base64** for the wire form — not XOR. On failure returns **`NULL`** with **`errno`** set (**`EINVAL`**, **`ENOMEM`**, etc., per the encoding helpers).

#### string2hex / hex2string / hex2printf
```c
void string2hex(const char *src, char *dest, int len);
void hex2string(const char *src, char *dest, int len);
void hex2printf(const char *src, char *dest, int len);
```
**Description:** String and hex conversion

### 16.3 MAC Address Processing

#### string2mac / mac2string / mac2int / mac2serial / mac2add / macrang
```c
boole string2mac(const char *macbuf, hp_mac_t mac);
boole mac2string(hp_mac_t mac, char *macbuf);
unsigned int mac2int(hp_mac_t mac);
boole mac2serial(hp_mac_t mac, char *macbuf);
void mac2add(hp_mac_t mac, int i);
boole macrang(hp_mac_t mac, hp_mac_t start, hp_mac_t end, int mod);
```
**Description:** MAC address conversions

### 16.4 Signal Handling

#### signal_noprocess / signal_register
```c
void signal_noprocess(int signo);
sighandler_t signal_register(int signo, sighandler_t func, int sa_flags);
```
**Description:** No-op signal handler/Register signal handler

### 16.5 Directory Operations

#### directory_size / directory_subsize / directory_sum / directory_subsum
```c
int directory_size(const char *dir);
int directory_subsize(const char *dir);
int directory_sum(const char *dir);
int directory_subsum(const char *dir);
```
**Description:** Get directory size or entry count. **`directory_subsize` and `directory_sum` are declared in the header but not implemented in this tree** (linking a call will fail); use `directory_size` / `directory_subsum` or add implementations if you need them.

### 16.6 File Locking

#### fd_lock / fd_unlock / fd_lock_pid
```c
boole fd_lock(int fd, boole ex, int start, int whence, int len, int wait);
boole fd_unlock(int fd, boole ex, int start, int whence, int len);
pid_t fd_lock_pid(int fd, boole ex, int start, int whence, int len);
```
**Description:** File region lock/unlock/get lock owner

#### fd_block / fd_nonblock
```c
boole fd_block(int fd);
boole fd_nonblock(int fd);
```
**Description:** Set file blocking/non-blocking mode

#### lock_open / lock_close
```c
int lock_open(const char *filename, int flags, int mode, int block);
int lock_close(int fd);
```
**Description:** Open/Close file with lock

### 16.7 File Read/Write

#### string2file / string3file / file2string
```c
int string2file(const char *filename, const char *format, ...);
int string3file(const char *filename, const char *format, ...);
const char *file2string(const char *filename, char *buffer, int bufsize);
```
**Description:** Write string to file/Append to file/Read from file

#### number2file / file2number
```c
int number2file(const char *filename, int number);
int file2number(const char *filename);
```
**Description:** Write/Read number to/from file

### 16.8 Time Functions

#### time_stamp / uptime_int / uptime_string / uptime_desc / livetime_desc / date_desc / date_set / date_adjust
```c
long long time_stamp(void);
unsigned long uptime_int(void);
const char *uptime_string(char *buffer, int buflen);
const char *uptime_desc(char *buffer, int buflen);
const char *livetime_desc(unsigned int ontime, char *buffer, int buflen);
const char *date_desc(char *buffer, int buflen);
boole date_set(time_t seconds, const char* zone);
time_t date_adjust(time_t seconds, const char* zone);
```
**Description:** Timestamp/Uptime/Date operations

### 16.9 System Commands

#### shell / execute / silent_execute / shell_injection_check
```c
int   shell(const char *format, ...);
int   execute(int timeout, boole silent, const char *format, ...);
#define silent_execute(...) execute(0, 1, __VA_ARGS__)
boole shell_injection_check(const char *cmd);
```
**Description:** **`shell`** formats the command into a **`LINE_MAX`** buffer, rejects **empty** / **oversized** commands and patterns unsafe for **`/bin/sh -c`** (e.g. `` ` ``, `|`, `;`, `<`, `$(` / `${`, stray `&` except `>&`), then calls **`system(3)`** — returns its status, **`errno`** preserved from the last relevant failure. **`execute`** **`fork`**s and runs **`execvp`** on the **first whitespace-separated token** as argv\[0\] (up to **19** more tokens); optional **`timeout`** seconds and **`SIGALRM`**; on timeout sends **`SIGKILL`**. Return value is the child’s **`WEXITSTATUS`** when exited cleanly, or **`-1`** with **`errno`** on fork/exec/wait/signal errors or non-normal exit. **`silent_execute`** is **`execute(0, 1, …)`** (stdio to **`/dev/null`** in the child when supported). **`shell_injection_check`** returns **`true`** if the string contains injection-prone characters (same class of checks used before **`system`**).

#### killpid
```c
boole killpid(pid_t pid, int timeout);
```
**Description:** Force kill process

#### ifconfig / iptables / ip6tables / ebtables
```c
int ifconfig(const char *format, ...);
int iptables(const char *format, ...);
int ip6tables(const char *format, ...);
int ebtables(const char *format, ...);
```
**Description:** Execute network configuration commands (with lock)

#### insmod / rmmod / lsmod
```c
int insmod(const char *module);
int rmmod(const char *module);
boole lsmod(const char *module);
```
**Description:** **`insmod`** uses **`modprobe`** via **`shell`** when the module name is **not** already listed in **`/proc/modules`**; if already loaded returns **`-1`** with **`EEXIST`**. **`rmmod`** runs **`rmmod`** only when the module **appears** in **`/proc/modules`**; if not loaded returns **`-1`** with **`EINVAL`**. Return values otherwise follow **`shell`**. **`lsmod`** checks presence in **`/proc/modules`**.

### 16.10 Network Tools

#### ip2subnet / netmask2cidr / netmask2num
```c
const char *ip2subnet(const char *ip, const char *mask, char *subnet, int len);
const char *netmask2cidr(const char *netmask, char *buf, int buflen);
unsigned int netmask2num(const char *mask);
```
**Description:** IP subnet calculations

#### netdev_flags / netdev_info / netdev_flew
```c
int netdev_flags(const char *card, short flag);
int netdev_info(const char *card, char *ip, int ip_len, char *pppip, int pppip_len, char *mask, int mask_len, char *mac, int mac_len);
int netdev_flew(const char *card, unsigned long long *rx_bytes, unsigned long long *rx_packets, unsigned long long *rx_errs, unsigned long long *rx_drops, unsigned long long *tx_bytes, unsigned long long *tx_packets, unsigned long long *tx_errs, unsigned long long *tx_drops);
```
**Description:** Network interface flags/info/traffic statistics

#### route_info / routes_info / outer_info / gateway_info
```c
int route_info(const char *destname, const char *mask, const char *metric, char *gateway, char *netdev);
int routes_info(const char *tid, const char *destname, const char *mask, const char *metric, char *gateway, char *netdev);
boole outer_info(char *gateway, char *netdev);
boole gateway_info(char *gateway, char *netdev);
```
**Description:** Route information query

#### route_switch / routes_switch / routes_switch2 / routes_switch3 / routes_switch4 / routes_ifname
```c
boole route_switch(const char *dest, const char *mask, const char *metric, talk_t v, boole clear);
boole routes_switch(const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, boole clear);
boole routes_switch2(const char *tid, const char *dest, const char *mask, const char *metric, talk_t v, talk_t v2, boole clear);
boole routes_ifname(int tid, talk_t ifnamest);
```
**Description:** Route switching

#### domain2ip
```c
const char *domain2ip(const char *addr, char *ipbuf, int ipbuflen, int timeout);
```
**Description:** Domain name resolution

### 16.11 Socket Tools

#### socket_reuse / socket_nocheck / socket_block / socket_nonblock
```c
boole socket_reuse(int sock);
boole socket_nocheck(int sock);
boole socket_block(int sock);
boole socket_nonblock(int sock);
```
**Description:** Socket option settings

#### socket_keepalive
```c
boole socket_keepalive(int sock, int keepintvl, int keepidle, int keepcnt);
```
**Description:** Set TCP keepalive

#### tcp_connect / udp_connect
```c
int tcp_connect(const char *peer, int port, int timeout, int keepintvl, int keepidle, int keepcnt);
int udp_connect(const char *peer, int port, int timeout);
```
**Description:** Create TCP/UDP connections

#### unix_connect / unix_listen
```c
int unix_connect(const char *peer, const char *local, int type);
int unix_listen(const char *local, int type);
```
**Description:** Unix domain socket connect/listen

### 16.12 Talk Transmission

#### talk2fd / talk2tcp / talk2udp / talk2socket
```c
int talk2fd(int fd, talk_t talk, int errcode);
int talk2tcp(int fd, talk_t talk, int errcode, int timeout);
int talk2udp(int fd, talk_t talk, int errcode, struct sockaddr *addr, int addrlen, int timeout);
int talk2socket(int fd, talk_t talk, int errcode, struct sockaddr *addr, int addrlen, int timeout);
```
**Description:** Send talk data

#### fd2talk / tcp2talk / udp2talk / socket2talk
```c
talk_t fd2talk(int fd);
talk_t tcp2talk(int fd, int timeout);
talk_t udp2talk(int fd, struct sockaddr *addr, socklen_t *addrlen, int timeout);
talk_t socket2talk(int fd, struct sockaddr *addr, socklen_t *addrlen, int timeout);
```
**Description:** Receive talk data

### 16.13 System Tools

#### random_long
```c
unsigned long random_long(void);
```
**Description:** Get random number

#### partition_dev / partlabel_dev
```c
boole partition_dev(const char *name, char *mtd, char *mtdblock);
boole partlabel_dev(const char *name, char *mmc);
```
**Description:** Get MTD/MMC device paths

#### fileline_merge / fileline_compare
```c
boole fileline_merge(const char *gap, const char *src, const char *adjust, const char *merge);
int   fileline_compare(const char *filea, const char *fileb);
```
**Description:** Merge file lines; **`fileline_compare`** returns whether two files’ line sets match (see `utility.h` for return codes).

### 16.14 UART

#### uart_open
```c
int uart_open(const char *path, int speed, int parity, int databit, int stopbit, int flow, int timeout);
```
**Description:** Open UART
**Parameters:**
- path - Device path
- speed - Baud rate (9600, 115200, etc.)
- parity - Parity (0 none, 1 odd, 2 even)
- databit - Data bits (5,6,7,8)
- stopbit - Stop bits (1,2)
- flow - Flow control (0 none, 1 soft, 2 hard)
- timeout - Timeout

---

## 17. Skin API Macros (skinapi.h)

### 17.1 Memory and Format Error Handling

```c
#define memory_exit(i) do { default_fault("memory oops"); exit(i); } while(0)
#define format_error(string) do { default_fault("format oops: %s", string); } while(0)
```

### 17.2 FPK API

```c
#define fpk_list(...) scalls(FPK_COM, "list", __VA_ARGS__)
#define fpk_register(...) scalls(FPK_COM, "register", __VA_ARGS__)
#define fpk_unregister(...) scalls(FPK_COM, "unregister", __VA_ARGS__)
```

### 17.3 Init API

```c
#define init_list(...) scalls(INIT_COM, "list", __VA_ARGS__)
#define init_register(item, call) scall2s(INIT_COM, "register", item, call)
```

### 17.4 Uninit API

```c
#define uninit_list(...) scalls(UNINIT_COM, "list", __VA_ARGS__)
#define uninit_register(item, call) scall2s(UNINIT_COM, "register", item, call)
```

**Note:** Matches `skinapi.h`: **`uninit_list`** routes **`"list"`** to **`UNINIT_COM`** (not **`INIT_COM`**).

### 17.5 Joint API

```c
#define joint_list(...) scalls(JOINT_COM, "list", __VA_ARGS__)
#define joint_register(item, call) scall2s(JOINT_COM, "register", item, call)
#define joint_unregister(item, call) scall2s(JOINT_COM, "unregister", item, call)
#define joint_calls(joint, string) scall2s(JOINT_COM, "call", joint, string)
#define joint_callt(joint, json) scallst(JOINT_COM, "call", joint, json)
```

### 17.6 Machine API

```c
#define machine_config(...) sgets(MACHINE_COM, __VA_ARGS__)
#define machine_status(...) scalls(MACHINE_COM, "status", __VA_ARGS__)
#define machine_restart(delay, key) scalls(MACHINE_COM, "restart", "%d,%s", delay, (key)?(key):"")
#define machine_reboot(delay, key) scalls(MACHINE_COM, "reboot", "%d,%s", delay, (key)?(key):"")
#define machine_default(delay, key) scalls(MACHINE_COM, "default", "%d,%s", delay, (key)?(key):"")
```

**Note:** **`NULL` `key`** is passed to **`scalls`** as **`""`** (ISO C). See the file-level comment in **`skinapi.h`**.

---

## 18. Predefined Component Constants (skinhead.h)

### 18.0 Limits and platform caps (`skinhead.h`)

| Macro | Typical role |
|-------|----------------|
| **`NAME_MAX`** | **256** if not defined by the system — max sensible length for single path component names in Skin helpers. |
| **`PATH_MAX`** | **512** if not defined — buffers for filesystem paths built by Skin. |
| **`LINE_MAX`** | **1024** if not defined — e.g. **`shell`** / **`execute`** command buffers. |
| **`JSON_LINE_MAX`** | **65535** — upper bound for a single JSON line / string chunk in parsers that enforce it. |

### 18.1 Hardware Project Components

```c
#define GPIO_COM        "arch@gpio"
#define DATA_COM        "arch@data"
#define CONFIG_COM      "arch@config"
#define FIRMWARE_COM    "arch@firmware"
#define FACTORY_COM     "arch@factory"
#define NET_CONFIG      "arch@net"
#define ETHERNET_COM    "arch@ethernet"
#define USB_COM         "arch@usb"
#define PCI_COM         "arch@pci"
#define SDIO_COM        "arch@sdio"
#define CUSTOM_COM      "arch@custom"
#define LOCK_COM        "arch@lock"
```

### 18.2 Core Project Components

```c
#define HEART_COM       "land@heart"
#define MACHINE_COM     "land@machine"
#define SERVICE_COM     "land@service"
#define FPK_COM         "land@fpk"
#define INIT_COM        "land@init"
#define UNINIT_COM      "land@uninit"
#define JOINT_COM       "land@joint"
#define AUTH_COM        "land@auth"
#define REG_COM         "land@register"
```

### 18.3 Network Project Components

```c
#define NETWORK_COM     "network@frame"
#define CONNECT_COM     "network@connect"
#define KEEPLIVE_COM    "network@keeplive"
#define BRIDGE_COM      "network@bridge"
#define VLAN_COM        "network@vlan"
#define HOSTS_COM       "network@hosts"
```

### 18.4 Interface Name Components

```c
#define LAN_COM         "ifname@lan"
#define LAN1_COM        "ifname@lan1"
#define LAN2_COM        "ifname@lan2"
#define LAN3_COM        "ifname@lan3"
#define LAN4_COM        "ifname@lan4"
#define WLTE_COM        "ifname@lte"
#define WAN_COM         "ifname@wan"
#define WAN2_COM        "ifname@wan2"
#define WISP_COM        "ifname@wisp"
```

### 18.5 Forward Project Components

```c
#define NAT_COM         "forward@nat"
#define DNAT_COM        "forward@dnat"
#define FIREWALL_COM    "forward@firewall"
#define RULE_COM        "forward@rule"
#define ROUTE_COM       "forward@route"
#define ROUTES_COM      "forward@routes"
```

### 18.6 Wireless Project Components

```c
#define WIFI_AP_COM     "wifi@ap"
#define WIFI_STA_COM    "wifi@sta"
#define NRADIO_COM      "wifi@n"
#define NSSID_COM       "wifi@nssid"
#define NSTA_COM        "wifi@nsta"
```

### 18.7 Modem Components

```c
#define OPERATOR_COM    "modem@operator"
#define MODEM_COM       "modem@atd"
#define LTE_COM         "modem@lte"
#define SMSD_COM        "modem@smsd"
#define SMS_COM         "modem@sms"
#define ATPROXY_COM     "modem@atproxy"
```

### 18.8 UART Project Components

```c
#define UART_COM        "uart@frame"
#define SERIAL_COM      "uart@serial"
#define SERIAL2_COM     "uart@serial2"
```

---

## 19. Complete Usage Examples

### 19.1 Basic JSON Operations

```c
#include "skin.h"ƒ

int main() {
    // Create JSON object
    talk_t config = json_create(NULL);
    
    // Set configuration values
    json_set_string(config, "hostname", "router1");
    json_set_number(config, "port", 8080);
    json_set_string(config, "ip", "192.168.1.1");
    
    // Nested JSON
    talk_t network = json_create(NULL);
    json_set_string(network, "wan_ip", "10.0.0.1");
    json_set_string(network, "lan_ip", "192.168.1.1");
    json_set_json(config, "network", network);
    
    // Print JSON
    talk_print(config);
    
    // Convert to string
    char *str = json2string(config);
    printf("JSON: %s\n", str);
    free(str);
    
    // Save to file
    json_save(config, "/tmp/config.json");
    
    // Release
    talk_free(network);
    talk_free(config);
    
    return 0;
}
```

### 19.2 Component Communication

```c
#include "skin.h"

int main() {
    // Method 1: Direct call using scall
    void *result = scalls("land@machine", "status", "");
    if (result > (void *)tpanic && json_check((talk_t)result)) {
        talk_print((talk_t)result);
        talk_free((talk_t)result);
    }
    
    // Method 2: Using parameter structure
    param_t p = param_create("eth0");
    result = scall("network@frame", "info", p);
    param_free(p);
    
    // Method 3: Using JSON parameters
    talk_t json = string2json("{\"interface\":\"eth0\",\"enable\":1}");
    result = scallt("network@frame", "config", json);
    talk_free(json);
    
    // Method 4: Get string result
    char buf[256];
    const char *ver = scalls_string(buf, sizeof(buf), "land@machine", "version", "");
    printf("Version: %s\n", ver);
    
    return 0;
}
```

### 19.3 Configuration Management

```c
#include "skin.h"

int main() {
    // Get configuration
    char buf[128];
    
    // Get string configuration
    sgets_string(buf, sizeof(buf), "network@frame", "wan/ip");
    printf("WAN IP: %s\n", buf);
    
    // Get JSON configuration
    talk_t cfg = (talk_t)sgets("network@frame", "wan");
    if (cfg > (void *)tpanic && json_check(cfg)) {
        const char *ip = json_string(cfg, "ip");
        const char *mask = json_string(cfg, "mask");
        printf("IP: %s, Mask: %s\n", ip, mask);
        talk_free(cfg);
    }
    
    // Set configuration
    ssets_string("network@frame", "192.168.1.1", "wan/%s", "ip");
    
    // Set JSON configuration
    talk_t wan_config = string2json("{\"ip\":\"192.168.1.1\",\"mask\":\"255.255.255.0\"}");
    sset("network@frame", wan_config, attr_create("wan"));
    talk_free(wan_config);
    
    return 0;
}
```

### 19.4 Registry Operations

```c
#include "skin.h"

int main() {
    // Set integer
    int counter = 100;
    reg_sset_int("land@machine", "boot_count", counter);
    
    // Get integer
    int boot_cnt = reg_sint("land@machine", "boot_count");
    printf("Boot count: %d\n", boot_cnt);
    
    // Set string
    reg_sset_string("land@machine", "device_name", "MyRouter");
    
    // Get string
    const char *name = reg_sstring("land@machine", "device_name");
    printf("Device name: %s\n", name);
    
    // Use pointer to modify (mmap shared; no explicit sync)
    int *p = reg_sintp("land@machine", "boot_count");
    (*p)++;
    
    return 0;
}
```

### 19.5 Service Management

```c
#include "skin.h"

int main() {
    // Run service (delay 5 seconds)
    sruns(5, "wan_monitor", "network@wan", "monitor", "eth0");
    
    // Run service immediately
    sruns(0, "dhcp_client", "client@dhcps", "start", "eth0");
    
    // Stop service
    sstop("wan_monitor");
    
    // Delete service
    sdelete("wan_monitor");
    
    // Get service PID
    pid_t pid = spid("dhcp_client");
    if (pid > 0) {
        printf("Service PID: %d\n", pid);
    }
    
    // Get service info
    talk_t info = sinfo("dhcp_client");
    if (info) {
        talk_print(info);
        talk_free(info);
    }
    
    // Get all services list
    talk_t list = slist();
    if (list && json_check(list)) {
        talk_print(list);
        talk_free(list);
    }
    
    return 0;
}
```

### 19.6 HE Command Execution

```c
#include "skin.h"

int main() {
    // Execute simple HE command
    talk_t result = string_he_execute("land@machine.status");
    if (result > (void *)tpanic && json_check(result)) {
        talk_print(result);
        talk_free(result);
    }
    
    // Execute HE command with parameters
    result = string_he_execute("network@frame.get[wan/ip]");
    if (result > (void *)tpanic) {
        if (json_check(result)) {
            talk_print(result);
        } else {
            printf("Result: %s\n", x2string(result));
        }
        talk_free(result);
    }
    
    // Execute set command
    result = string_he_execute("network@frame.set[wan/ip=192.168.1.1]");
    
    // Line command (prints to terminal)
    int rc = line_he_command("land@machine.version");
    
    return 0;
}
```

### 19.7 Network Tools

```c
#include "skin.h"

int main() {
    // Get network interface info
    char ip[32], mask[32], mac[32];
    netdev_info("eth0", ip, sizeof(ip), NULL, 0, mask, sizeof(mask), mac, sizeof(mac));
    printf("IP: %s, Mask: %s, MAC: %s\n", ip, mask, mac);
    
    // Get traffic statistics
    unsigned long long rx_bytes, rx_packets, tx_bytes, tx_packets;
    netdev_flew("eth0", &rx_bytes, &rx_packets, NULL, NULL, 
                &tx_bytes, &tx_packets, NULL, NULL);
    printf("RX: %llu bytes, TX: %llu bytes\n", rx_bytes, tx_bytes);
    
    // Get default gateway
    char gateway[32], netdev[32];
    if (outer_info(gateway, netdev)) {
        printf("Gateway: %s via %s\n", gateway, netdev);
    }
    
    // Domain resolution
    char ipbuf[32];
    const char *resolved = domain2ip("www.example.com", ipbuf, sizeof(ipbuf), 5);
    if (resolved) {
        printf("Resolved: %s\n", resolved);
    }
    
    // Execute iptables command
    iptables("-A INPUT -p tcp --dport 80 -j ACCEPT");
    
    return 0;
}
```

### 19.8 File Operations

```c
#include "skin.h"

int main() {
    // Write to file
    string2file("/tmp/test.txt", "Hello %s\n", "World");
    
    // Append to file
    string3file("/tmp/test.txt", "Line %d\n", 2);
    
    // Read from file
    char buf[256];
    const char *content = file2string("/tmp/test.txt", buf, sizeof(buf));
    printf("Content: %s\n", content);
    
    // Write number
    number2file("/tmp/counter.txt", 42);
    
    // Read number
    int num = file2number("/tmp/counter.txt");
    printf("Number: %d\n", num);
    
    // Open with lock
    int fd = lock_open("/tmp/locked.txt", O_RDWR|O_CREAT, 0644, -1);
    if (fd >= 0) {
        write(fd, "test", 4);
        lock_close(fd);
    }
    
    return 0;
}
```

### 19.9 Logging

```c
#include "skin.h"

int main() {
    // Log different levels
    land_verbose("This is a verbose message");
    land_debug("Debug information: %d", 42);
    land_info("System started");
    land_warn("Warning: low memory");
    land_warning("Failed to open: %s", "file.txt");  // with errno
    land_fault("Critical error occurred");
    land_faulting("System call failed");  // with errno
    
    // Network related logs
    network_info("Interface %s is up", "eth0");
    network_debug("IP address: %s", "192.168.1.1");
    
    // Use journal for custom type
    journal((LANDLOG_LAND<<16)|(LANDLOG_LAND_SERVICE<<24)|LANDLOG_INFO, 
            "Custom log entry");
    
    return 0;
}
```

### 19.10 Comprehensive Example: Configuration Management Tool

```c
#include "skin.h"

// Show component configuration
void show_config(const char *com, const char *attr) {
    char buf[256];
    const char *val = sgets_string(buf, sizeof(buf), com, attr);
    if (val) {
        printf("%s.%s = %s\n", com, attr, val);
    } else {
        printf("%s.%s = (not set)\n", com, attr);
    }
}

// Set component configuration
void set_config(const char *com, const char *attr, const char *value) {
    if (ssets_string(com, value, attr)) {
        printf("Set %s.%s = %s [OK]\n", com, attr, value);
    } else {
        printf("Set %s.%s = %s [FAILED]\n", com, attr, value);
    }
}

// Call component method
void call_method(const char *com, const char *api, const char *param) {
    void *result = scalls(com, api, "%s", param);
    if (result == tnull) {
        printf("Call %s.%s(%s) = null / no payload\n", com, api, param);
    } else if (result == ttrue) {
        printf("Call %s.%s(%s) = true\n", com, api, param);
    } else if (result == tfalse) {
        printf("Call %s.%s(%s) = false\n", com, api, param);
    } else if (result == terror) {
        printf("Call %s.%s(%s) = error\n", com, api, param);
    } else if (result == tpanic) {
        printf("Call %s.%s(%s) = panic / bad call\n", com, api, param);
    } else if (json_check((talk_t)result)) {
        printf("Call %s.%s(%s) = ", com, api, param);
        talk_print((talk_t)result);
        talk_free((talk_t)result);
    } else {
        printf("Call %s.%s(%s) = (unhandled pointer result)\n", com, api, param);
    }
}

int main(int argc, char *argv[]) {
    // Show machine info
    printf("=== Machine Info ===\n");
    show_config("land@machine", "version");
    show_config("land@machine", "serial/board");
    
    // Show network config
    printf("\n=== Network Config ===\n");
    show_config("network@frame", "wan/ip");
    show_config("network@frame", "wan/mask");
    show_config("network@frame", "wan/gateway");
    
    // Set config example
    printf("\n=== Setting Config ===\n");
    set_config("land@machine", "hostname", "MyRouter");
    
    // Call method example
    printf("\n=== Calling Methods ===\n");
    call_method("land@machine", "status", "");
    
    return 0;
}
```

---

## 20. Compilation and Usage

### 18.1 Include headers

**Umbrella (recommended for apps and quick ports):**

```c
#include "skin.h"   /* stdhead.h + skinhead.h + skinapi.h */
```

**Layered (components / minimal includes):**

- `skinhead.h` — lengths, `boole`, `PROJECT_OBJECT_GAPS`, `MACHINE_COM`, …
- `skinapi.h` — **does not include other headers**; it only expands macros such as `scalls()` / `machine_config()`. You must include **`com.h`** (for `scalls`, `obj_t`, …) and **`skinhead.h`** (for `MACHINE_COM`, `INIT_COM`, …) *before* `skinapi.h`, unless you already use `skin.h`.
- Individual modules: `talk.h`, `com.h`, `register.h`, `mxtalk.h`, `munix.h`, `config.h`, `utility.h`, … (include what you use; add `skinhead.h` / `stdhead.h` if typedefs or limits are missing)

### 18.2 Compilation options

```bash
gcc -o myapp myapp.c \
    -I/path/to/skin \
    -L/path/to/lib \
    -lskin \
    -levent \
    -lpthread \
    -ldl \
    -lm
```

### 18.3 Link libraries

- libskin.so - SkinOS core library
- libevent.so - Event loop library
- libpthread.so - Thread library
- libdl.so - Dynamic loading library
- libm.so - Math library

---

## 21. Important Notes

1. **Memory management:** Use `talk_free()` for `talk_t` values allocated by the talk/JSON APIs; use `free()` for `json2string()` and typical `utility.h` allocators unless the API says otherwise.
2. **Error handling:** Check return values and `errno` after failures; for `scall*` / `ccall*`, handle `tpanic`, `terror`, `tfalse`, and JSON results (see §1.1 and `com.h`).
3. **Thread safety:** There is **no blanket guarantee** across libskin. Assume **non-thread-safe** unless stated; `utility.h` shell/network helpers often invoke subprocesses or global state.
4. **Path format:** Component paths use `project@component`; configuration / attribute paths commonly use `level1/level2` (see `path.h`, `OBJECT_CONFIG_GAPS`).
5. **Log levels:** Tune level and output options for production; avoid verbose/debug spam (see §9.3.1 for composing `flags`).

---

## 22. Related Documents

- Public headers under the Skin library — source of truth for prototypes and Doxygen-style comments
- `skin.h` — umbrella include
- `skinhead.h` — constants and component name macros
- `skinapi.h` — convenience macros (`scalls`, `machine_*`, …)
- Optional companion docs (if present in your tree): `COM_API.md`, `SKIN_API.md`

---

*Document Version: 1.0*
*Last Updated: 2026-07-21*
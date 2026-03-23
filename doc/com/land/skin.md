# libskin API Documentation

## Overview

libskin is an embedded system platform providing component-based communication, configuration management, logging, service management, and other functionalities.

**Master header:** `#include "skin.h"` pulls in `stdhead.h` (standard C/POSIX includes), `skinhead.h` (types, limits, `*_COM` constants), and `skinapi.h` (shortcuts such as `scalls`, `machine_config`). For a smaller compile surface you can include only the headers you need (e.g. `talk.h` + `com.h`), but `skin.h` matches the layout in `skin/skin.h`.

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
   - [5.6 Shell invocation context (com.h)](#56-shell-invocation-context-comh)
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
9. [Logging API (log.h)](#9-logging-api-logh)  
   - [9.0 Summary](#90-summary)
10. [Service Management API (serv.h)](#10-service-management-api-servh)  
   - [10.0 Summary](#100-summary)
   - [10.6 Sample program (every `serv.h` function)](#106-sample-program-every-servh-function)
11. [Project Information API (project.h)](#11-project-information-api-projecth)  
   - [11.0 Summary](#110-summary)
   - [11.5 Sample program (every `project.h` API)](#115-sample-program-every-projecth-api)
12. [HE Command API (he2com.h)](#12-he-command-api-he2comh)  
   - [12.0 Summary](#120-summary)
   - [12.4 Sample program (every `he2com.h` function)](#124-sample-program-every-he2comh-function)
13. [Linked List API (link.h)](#13-linked-list-api-linkh)
14. [Utility Functions API (utility.h)](#14-utility-functions-api-utilityh)
15. [Skin API Macros (skinapi.h)](#15-skin-api-macros-skinapih)
16. [Predefined Component Constants (skinhead.h)](#16-predefined-component-constants-skinheadh)
17. [Complete Usage Examples](#17-complete-usage-examples)
18. [Compilation and Usage](#18-compilation-and-usage)
19. [Important Notes](#19-important-notes)
20. [Related Documents](#20-related-documents)

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

The program below is **educational** (not minimal production code). It calls **every function** declared in `talk.h` at least once. **`talk.h` macros** (`JSON_PATCH_OP`, `JSON_STRING_PREFIX`, `ttrue`, …) are not functions; this sample uses **`JSON_PATCH_OP`** in a comment and builds patch mode via `json_set_string(..., ".", ...)` as in `talk.c`.

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

---

### 3.1 Creation and Release

#### param_create
```c
param_t param_create(const char *a);
```
**Description:** Create parameter structure from string
**Parameters:** a - Option description string, e.g., "opt1,opt2,opt3"
**Returns:** Parameter structure pointer

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

#### param_string / param_talk / param_pointer
```c
const char *param_string(param_t parameter, int serial);
talk_t param_talk(param_t parameter, int serial);
void *param_pointer(param_t parameter, int serial);
```
**Description:** Get parameter value at specified serial (starts from 1, -1 for last)

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
| **`obj_t`** | Parsed **component path** (`project@component`, optional extra layers up to **`OBJ_MAX_LEVEL`** = 3). Holds project/component strings, optional `com` / register handles, and buffers; **`obj_create` / `obj_free`**. |
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
**Parameters:** string - Component path in format "project@component"
**Returns:** Object pointer

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
| **Types** | `com_t` handle; **`COM_FILE_*`** file kinds; optional **`comget_t` / `comset_t` / `comfetch_t` / `comsave_t`** typedefs for standard hooks. |
| **Discovery** | `com_list`, `com_project_list`, `com_register` aliases, **`com_path`** → buffer + returns **`char`** component type code. |
| **Open / symbol** | `com_open` / `com_sopen`, **`com_symbol`** (names use **`COM_API_PREFIX`**, e.g. `_status`). |
| **Calls** | **`ccall` / `scall`** + variants: `*t` (JSON), `*s` (format string), `*st`, `*4p`, `*_string` (result into user buffer). Results are **`void *`**: often **`talk_t` JSON** (caller **`talk_free`**) or sentinels **`ttrue` / `tfalse` / `terror` / `tpanic` / `tnull`** — see §1.1. |
| **Shell** | **`shell_object`**, **`shell_param`**, **`shell_api`**, **`shell_pipe`** when running inside an HE-driven API (§5.6). |

---

### 5.1 Component Type Definitions

```c
#define COM_FILE_KO      1    // Kernel module
#define COM_FILE_LIB     2    // Dynamic library (.com)
#define COM_FILE_SHELL   3    // Shell script (.ash)
#define COM_FILE_EXECUTE 4    // Executable file
```

### 5.2 Component Discovery and Registration

#### com_project_list
```c
talk_t com_project_list(void);
```
**Description:** Get project component list
**Returns:** JSON object with keys as "project@component" and values as paths

#### com_list
```c
talk_t com_list(const char *project);
```
**Description:** Get component list
**Parameters:** project - Project name (NULL for all)
**Returns:** JSON object

#### com_register / com_unregister
```c
boole com_register(const char *target, const char *origin);
boole com_unregister(const char *target);
```
**Description:** Register/Unregister component alias

#### com_register_list
```c
talk_t com_register_list(void);
```
**Description:** Get registered component list

#### com_path
```c
char com_path(obj_t obj, char *buffer, int buflen);
```
**Description:** Get component file path
**Returns:** Component type (COM_FILE_*) or 0 for failure

**Example:**
```c
// Get all component list
talk_t list = com_list(NULL);
talk_free(list);

// Register component alias
com_register("mywan", "network@wan");

// Get component path
obj_t o = obj_create("land@machine");
char path[PATH_MAX];
char type = com_path(o, path, sizeof(path));  /* COM_FILE_* or 0 on failure */
obj_free(o);
```

### 5.3 Component Open and Close

#### com_open / com_sopen
```c
com_t com_open(obj_t obj);
com_t com_sopen(const char *com);
```
**Description:** Open component
**Returns:** Component handle

#### com_symbol
```c
void *com_symbol(com_t com, const char *name);
```
**Description:** Get symbol from component

#### com_close
```c
void com_close(com_t com);
```
**Description:** Close component

#### com_exist / com_sexist
```c
boole com_exist(obj_t obj, const char *api);
boole com_sexist(const char *com, const char *api);
```
**Description:** Check if component/API exists

**Example:**
```c
com_t c = com_sopen("land@machine");
if (c) {
    void *fn = com_symbol(c, "_status");
    com_close(c);
}

// Check if API exists
if (com_sexist("land@machine", "status")) {
    // API exists
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

When a component API is entered through the HE/shell command path, the runtime can query the current call:

#### shell_object / shell_param / shell_api / shell_pipe
```c
obj_t shell_object(void);
param_t shell_param(void);
const char *shell_api(void);
int shell_pipe(void);
```
**Description:** Current component object, `param_t`, method name, and optional reply pipe descriptor. **Outside a shell-driven API call**, `shell_object` / `shell_param` / `shell_api` return **NULL**, and `shell_pipe` returns a negative value (see `com.h`).

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

    (void)shell_object();
    (void)shell_param();
    (void)shell_api();
    (void)shell_pipe();

    tl = com_project_list();
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);
    (void)com_register("demo_alias@com", "land@machine");
    (void)com_unregister("demo_alias@com");
    tl = com_register_list();
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);
    (void)com_path(o, cbuf, sizeof cbuf);
    tl = com_list(NULL);
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);

    ch = com_open(o);
    if (ch) {
        (void)com_symbol(ch, "_api");
        com_close(ch);
    }
    ch = com_sopen("land@machine");
    if (ch) com_close(ch);

    (void)com_exist(o, "status");
    (void)com_sexist("land@machine", "status");

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

    (void)csave(o, fad, j1, ak);
    (void)csaves(o, fad, j1, "%s", "walk/key");
    (void)ssave("land@machine", "/var/tmp/skin_walkthrough.db", j1, ak);
    (void)ssaves("land@machine", "/var/tmp/skin_walkthrough.db", j1, "%s", "walk/key");
    (void)csave_string(o, fad, "sv", ak);
    (void)csaves_string(o, fad, "sv", "%s", "walk/key");
    (void)ssave_string("land@machine", "/var/tmp/skin_walkthrough.db", "sv", ak);
    (void)ssaves_string("land@machine", "/var/tmp/skin_walkthrough.db", "sv", "%s", "walk/key");

    rv = cfetch(o, fad, ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = cfetchs(o, fad, "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sfetch("land@machine", "/var/tmp/skin_walkthrough.db", ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sfetchs("land@machine", "/var/tmp/skin_walkthrough.db", "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    (void)cfetch_string(cbuf, sizeof cbuf, o, fad, ak);
    (void)cfetchs_string(cbuf, sizeof cbuf, o, fad, "%s", "walk/key");
    (void)sfetch_string(cbuf, sizeof cbuf, "land@machine", "/var/tmp/skin_walkthrough.db", ak);
    (void)sfetchs_string(cbuf, sizeof cbuf, "land@machine", "/var/tmp/skin_walkthrough.db", "%s", "walk/key");

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
| **Returns** | **`config_get*`** often returns heap **`talk_t`** → **`talk_free`**; **`tpanic`** on dispatch errors per comments. **`config_set`** copies **`v`**; caller still owns original `v` unless documented otherwise. |
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
**Description:** Set configuration value

#### config_set_string / config_sset_string / config_ssets_string
```c
boole config_set_string(obj_t com, const char *string, attr_t attr);
boole config_sset_string(const char *com, const char *string, attr_t attr);
boole config_ssets_string(const char *com, const char *string, const char *attr, ...);
```
**Description:** Set configuration value as string

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
- fa - File path attribute
- attr - Data key

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

`register.h` exposes a **mmap-backed, file-based key/value store** shared by object name (e.g. **`land@machine`**): binary-safe, distinct from environment variables.

| Layer | Role |
|-------|------|
| **Low-level** | **`register_open` / `register_search` / `register_close`** on a backing file; variable layout via **`register_var_t`**. |
| **Object API** | **`register_set` / `register_pointer` / `register_value`** and **`register_sync` / `register_ssync`** (flush mmap). |
| **Locks** | **`register_lock` / `register_lockw` / `register_unlock`** — record locks + `flock`; see §8.2 and `register.h` notes. |
| **Typed macros** | **`reg_int`**, **`reg_string`**, **`reg_set_*`**, **`reg_*v` / `reg_*p`**, etc., for common C types. |
| **Listing** | **`reg_list` / `reg_slist`** → JSON of variables (**`talk_free`**). |

(Unrelated but often used with the same codebase: **`utility.h`** declares **`directory_subsize` / `directory_sum`** without implementations — see §14.5.)

---

### 8.1 Low-level Operations

#### register_open / register_search / register_close
```c
register_file_t register_open(const char *object, int flags, int mode, int value_number, int total_size);
register_var_t register_search(register_file_t h, void *point, const char *name);
void register_close(register_file_t h);
```
**Description:** Open/Search/Close registry file

### 8.2 General Operations

#### register_set / register_sset
```c
void *register_set(obj_t this, const char *name, const void *v, int size, int capacity);
void *register_sset(const char *object, const char *name, const void *v, int size, int capacity);
```
**Description:** Set registry value

#### register_pointer / register_spointer
```c
void *register_pointer(obj_t this, const char *name);
void *register_spointer(const char *object, const char *name);
```
**Description:** Get registry value pointer (read-write)

#### register_value / register_svalue
```c
const void *register_value(obj_t this, const char *name);
const void *register_svalue(const char *object, const char *name);
```
**Description:** Get registry value (read-only)

#### register_size / register_ssize
```c
int register_size(obj_t this, const char *name);
int register_ssize(const char *object, const char *name);
```
**Description:** Get registry value size

#### register_sync / register_ssync
```c
void register_sync(obj_t this);
void register_ssync(const char *object);
```
**Description:** Sync registry to disk

#### register_lock / register_lockw / register_unlock
```c
boole register_lock(obj_t this, void *point, const char *name, int flag);
boole register_lockw(obj_t this, void *point, const char *name, int flag);
boole register_unlock(obj_t this, void *point, const char *name);
```
**Description:** Advisory / record locks on a register variable’s value range. `flag` is `F_WRLCK` or `F_RDLCK`. **`register_lock`** uses `fcntl(F_SETLK)` (does not wait for the *range* lock); **`register_lockw`** uses `fcntl(F_SETLKW)` (waits). Both may **block on `flock(LOCK_SH)`** on the register file first — see `register.h` comments. On success, `point` (if non-NULL) can receive a pointer to the variable’s mmap’d storage.

### 8.3 Integer Operations

#### reg_int / reg_sint
```c
int reg_int(obj_t this, const char *name);
int reg_sint(const char *object, const char *name);
```
**Description:** Get integer registry value

#### reg_set_int / reg_sset_int
```c
#define reg_set_int(this, name, v) register_set(this, name, &v, sizeof(int), sizeof(int))
#define reg_sset_int(this, name, v) register_sset(this, name, &v, sizeof(int), sizeof(int))
```
**Description:** Set integer registry value

#### reg_intv / reg_sintv / reg_intp / reg_sintp
```c
#define reg_intv(this, name) (const int*)register_value(this, name)
#define reg_sintv(this, name) (const int*)register_svalue(this, name)
#define reg_intp(this, name) (int*)register_pointer(this, name)
#define reg_sintp(this, name) (int*)register_spointer(this, name)
```
**Description:** Get integer pointer (read-only/read-write)

### 8.4 Boolean Operations

#### reg_boole / reg_sboole
```c
boole reg_boole(obj_t this, const char *name);
boole reg_sboole(const char *object, const char *name);
```
**Description:** Get boolean registry value

#### reg_set_boole / reg_sset_boole
```c
#define reg_set_boole(this, name, v) register_set(this, name, &v, sizeof(boole), sizeof(boole))
#define reg_sset_boole(this, name, v) register_sset(this, name, &v, sizeof(boole), sizeof(boole))
```

#### reg_boolev / reg_sboolev / reg_boolep / reg_sboolep
```c
#define reg_boolev(this, name) (const boole*)register_value(this, name)
#define reg_sboolev(this, name) (const boole*)register_svalue(this, name)
#define reg_boolep(this, name) (boole*)register_pointer(this, name)
#define reg_sboolep(this, name) (boole*)register_spointer(this, name)
```

### 8.5 String Operations

#### reg_string / reg_sstring
```c
const char *reg_string(obj_t this, const char *name);
const char *reg_sstring(const char *object, const char *name);
```
**Description:** Get string registry value

#### reg_set_string / reg_sset_string
```c
char *reg_set_string(obj_t this, const char *name, const char *v);
char *reg_sset_string(const char *object, const char *name, const char *v);
```
**Description:** Set string registry value

#### reg_stringv / reg_sstringv / reg_stringp / reg_sstringp
```c
#define reg_stringv(this, name)       ((const char *)register_value(this, name))
#define reg_sstringv(this, name)      ((const char *)register_svalue(this, name))
#define reg_stringp(this, name)       ((char *)register_pointer(this, name))
#define reg_sstringp(this, name)      ((char *)register_spointer(this, name))
```
**Description:** Shorthand to treat register storage as C string. **`reg_stringv` / `reg_sstringv`** return read-only views; **`reg_stringp` / `reg_sstringp`** return writable mmap-backed storage (same lifetime and bounds rules as `register_pointer`). Macro parameter names match `register.h` (`this` is `obj_t` for `reg_*`, or the **object name string** for `reg_s*` variants).

### 8.6 List Operations

#### reg_list / reg_slist
```c
talk_t reg_list(obj_t this);
talk_t reg_slist(const char *object);
```
**Description:** Get registry list

**Example:**
```c
// Set integer
int val = 42;
reg_sset_int("land@machine", "counter", val);

// Get integer
int cnt = reg_sint("land@machine", "counter");

// Set string
reg_sset_string("land@machine", "hostname", "router1");

// Get string
const char *host = reg_sstring("land@machine", "hostname");

// Use pointer to modify
int *p = reg_sintp("land@machine", "counter");
*p = 100;
register_ssync("land@machine");
```

### 8.7 Sample program (every `register.h` API)

Touches **every function and typed macro** in `register.h`. **`register_open` / mmap paths** need a working register backend; **`register_lock*`** uses **`fcntl`** record locks (`F_RDLCK` / `F_WRLCK` from `<fcntl.h>`).

```c
#ifndef PROJECT_ID
#define PROJECT_ID "land"
#endif
#include "skin.h"
#include <fcntl.h>

static void demo_register_all(void)
{
    int i = 7;
    boole b = true;
    register_file_t h;
    register_var_t rv;
    obj_t o = obj_create("land@machine");
    talk_t tl;

    h = register_open("land@machine", O_RDWR, 0644, REGISTER_VAR_NUM, REGISTER_VAR_SIZE);
    if (h) {
        rv = register_search(h, NULL, "demo_var");
        (void)rv;
        (void)register_value_size(h, "demo_var");
        (void)register_value_pointer(h, "demo_var");
        (void)register_value_set(h, "demo_var", &i, sizeof i, sizeof i);
        register_close(h);
    }

    (void)register_set(o, "rw_i", &i, sizeof i, sizeof i);
    (void)register_sset("land@machine", "rw_i", &i, sizeof i, sizeof i);
    (void)register_pointer(o, "rw_i");
    (void)register_spointer("land@machine", "rw_i");
    (void)register_value(o, "rw_i");
    (void)register_svalue("land@machine", "rw_i");
    (void)register_size(o, "rw_i");
    (void)register_ssize("land@machine", "rw_i");
    register_sync(o);
    register_ssync("land@machine");

    (void)register_lock(o, NULL, "rw_i", F_RDLCK);
    (void)register_lockw(o, NULL, "rw_i", F_RDLCK);
    (void)register_unlock(o, NULL, "rw_i");

    tl = reg_list(o);
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);
    tl = reg_slist("land@machine");
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);

    errno = 0;
    (void)reg_int(o, "rw_i");
    (void)reg_sint("land@machine", "rw_i");
    reg_set_int(o, "rw_i", i);
    reg_sset_int("land@machine", "rw_i", i);
    (void)reg_intv(o, "rw_i");
    (void)reg_sintv("land@machine", "rw_i");
    (void)reg_intp(o, "rw_i");
    (void)reg_sintp("land@machine", "rw_i");

    (void)reg_boole(o, "rw_b");
    (void)reg_sboole("land@machine", "rw_b");
    reg_set_boole(o, "rw_b", b);
    reg_sset_boole("land@machine", "rw_b", b);
    (void)reg_boolev(o, "rw_b");
    (void)reg_sboolev("land@machine", "rw_b");
    (void)reg_boolep(o, "rw_b");
    (void)reg_sboolep("land@machine", "rw_b");

    (void)reg_string(o, "rw_s");
    (void)reg_sstring("land@machine", "rw_s");
    (void)reg_set_string(o, "rw_s", "txt");
    (void)reg_sset_string("land@machine", "rw_s", "txt");
    (void)reg_stringv(o, "rw_s");
    (void)reg_sstringv("land@machine", "rw_s");
    (void)reg_stringp(o, "rw_s");
    (void)reg_sstringp("land@machine", "rw_s");

    obj_free(o);
}
```

---

## 9. Logging API (log.h)

### 9.0 Summary

`log.h` defines **severity levels**, **output options**, and **subsystem type/subtype** constants that are packed into a single **`unsigned int flags`** passed to **`landlog()`**. The runtime (`log.c`) splits `flags` into level, options, type, and subtype, then filters against **`register`** keys such as **`log_mask`** / **`log_options`** before formatting and writing (TUI, syslog, file, etc.).

| Entry point | Role |
|-------------|------|
| **`landlog(flags, file, line, fmt, …)`** | Low-level logger; usually invoked via **`journal(flags, …)`** or the predefined macros (`default_info`, `network_debug`, `shell_fault`, …). |
| **Macros** | Families include **default_***, **shell_***, **land_***, **auth_***, **network_***, … — each expands to a fully composed `flags` word plus `__FILE__` / `__LINE__`. |

**Composing custom `flags`:** OR **level** + **options** + **(type shifted by `LANDLOG_TYPE_OFFSET`)** + **(subtype shifted by `LANDLOG_SUBTYPE_OFFSET`)** — see **§9.3.1** and the `LANDLOG_*_OFFSET` / `*_MASK` macros in `log.h`. **`LANDLOG_ERRNO`** appends `strerror(errno)` when set.

---

### 9.1 Log Levels

```c
#define LANDLOG_FAULT     (1)    // Fault
#define LANDLOG_WARN      (2)    // Warning
#define LANDLOG_INFO      (4)    // Information
#define LANDLOG_DEBUG     (8)    // Debug
#define LANDLOG_VERBOSE   (16)   // Verbose
```

### 9.2 Log Options

```c
#define LANDLOG_TUI       (1<<8)   // Output to TUI
#define LANDLOG_SYSLOG    (2<<8)   // Output to system log
#define LANDLOG_FILE      (4<<8)   // Output to file
#define LANDLOG_TRACE     (8<<8)   // Output trace info
#define LANDLOG_ERRNO     (16<<8)  // Include errno
```

### 9.3 Log Types

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

### 9.3.1 Composing `flags` for `landlog()`

`landlog()` treats `flags` as a bit layout (`log.c`):

| Field | Bits (conceptually) | Macros |
|-------|---------------------|--------|
| Level | low byte | `LANDLOG_FAULT`, `LANDLOG_WARN`, `LANDLOG_INFO`, `LANDLOG_DEBUG`, `LANDLOG_VERBOSE` (`LANDLOG_LEVEL_MASK`, `LANDLOG_LEVEL_OFFSET`) |
| Options | next byte | `LANDLOG_TUI`, `LANDLOG_SYSLOG`, `LANDLOG_FILE`, `LANDLOG_TRACE`, `LANDLOG_ERRNO` (`LANDLOG_OPTION_MASK`, `LANDLOG_OPTION_OFFSET`) |
| Type | next byte | e.g. `(LANDLOG_LAND << LANDLOG_TYPE_OFFSET)` (`LANDLOG_TYPE_MASK`, `LANDLOG_TYPE_OFFSET`) |
| Subtype | high byte | e.g. `(LANDLOG_LAND_DEFAULT << LANDLOG_SUBTYPE_OFFSET)` (`LANDLOG_SUBTYPE_MASK`, `LANDLOG_SUBTYPE_OFFSET`) |

The predefined macros such as `default_info(...)` expand to a full combination, for example  
`(LANDLOG_DEFAULT << LANDLOG_TYPE_OFFSET) | (LANDLOG_DEFAULT_NONE << LANDLOG_SUBTYPE_OFFSET) | LANDLOG_INFO`.  
When building custom flags, OR **shifted** type/subtype constants with level and options instead of using raw `LANDLOG_LAND` alone as the entire `flags` word.

### 9.4 Log Functions

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

### 9.5 Predefined Log Macros

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

## 10. Service Management API (serv.h)

### 10.0 Summary

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

### 10.1 Service Register and Run

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

### 10.2 Service Reset

#### sreset / creset / sresett / cresett / sreset2t / creset2t / sresets / cresets
```c
boole sreset(const char *com, const char *api, param_t param, const char *nameformat, ...);
boole creset(obj_t com, const char *api, param_t param, const char *nameformat, ...);
```
**Description:** Reset or start service (registers if not exists)

### 10.3 Service Start

#### sstart / cstart / sstartt / cstartt / sstart2t / cstart2t / sstarts / cstarts
```c
boole sstart(const char *com, const char *api, param_t param, const char *nameformat, ...);
boole cstart(obj_t com, const char *api, param_t param, const char *nameformat, ...);
```
**Description:** Start service

### 10.4 Service Control

#### sdelete / sstop / soff / soffdel
```c
boole sdelete(const char *nameformat, ...);
boole sstop(const char *nameformat, ...);
boole soff(const char *nameformat, ...);
boole soffdel(const char *nameformat, ...);
```
**Description:** Delete/Stop/Off/Off and delete service

### 10.5 Service Query

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

### 10.6 Sample program (every `serv.h` function)

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

## 11. Project Information API (project.h)

### 11.0 Summary

`project.h` manages the **multi-project filesystem layout** (`PROJECT_INFOFILE` / **`prj.json`**, default version **`PROJECT_DEFAULT_VERSION`**).

| Area | APIs |
|------|------|
| **Discovery** | **`project_scan`** (refresh), **`project_list`** (cache), **`project_dirty`**, **`project_check`**. |
| **Paths** | **`project_path`**, **`project_storage`**, **`project_exe_path`**, **`project_var_path`**, **`project_internal_path`** — plus **`project2path`**, **`exe2path`**, … macros using **`PROJECT_ID`**. |
| **Bootstrapping** | **`project_add_init` / `project_add_uninit` / `project_add_joint` / `project_add_object`** register entries in project metadata. |
| **i18n** | **`project_i18n`**, **`project_i18n_get`**. |

Most functions return paths into **`buffer`** or heap **`talk_t`** lists — **`talk_free`** when applicable; see each prototype in `project.h`.

---

### 11.1 Project Scan and List

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

### 11.2 Project Paths

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

#### project_exe_path / exe2path / project_var_path / var2path / project_internal_path / internal2path
```c
const char *project_exe_path(char *buffer, int buflen, const char *name, const char *execute, ...);
#define exe2path(buffer, buflen, ...) project_exe_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
const char *project_var_path(char *buffer, int buflen, const char *name, const char *variable, ...);
#define var2path(buffer, buflen, ...) project_var_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
const char *project_internal_path(char *buffer, int buflen, const char *name, const char *variable, ...);
#define internal2path(buffer, buflen, ...) project_internal_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
```
**Description:** Get executable/variable/internal file paths

### 11.3 Project Configuration

#### project_add_init / project_add_uninit / project_add_joint / project_add_object
```c
boole project_add_init(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_uninit(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_joint(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_object(const char *name, const char *prjpath, const char *object, const char *com);
```
**Description:** Add init/uninit/event/object operations

### 11.4 Internationalization

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

### 11.5 Sample program (every `project.h` API)

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
    (void)project_exe_path(buf, sizeof buf, "land", "app");
    (void)exe2path(buf, sizeof buf, "app");
    (void)ko2path(buf, sizeof buf, "drv.ko");
    (void)shell2path(buf, sizeof buf, "s.ash");
    (void)misc2path(buf, sizeof buf, "m.bin");
    (void)cfg2path(buf, sizeof buf, "c.json");
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

## 12. HE Command API (he2com.h)

### 12.0 Summary

`he2com.h` parses and executes **HE** strings — the CLI/config language mapping to **get/set/call** on components (similar to shell `project@component:attr=value` or `project@component.method[param]`).

| Piece | Role |
|-------|------|
| **Types** | **`HE_GET` / `HE_SET` / `HE_OR` / `HE_CALL` / `HE_DBS_*`** — operation kind stored in **`he_t`’s `flags` field**. |
| **`he_t`** | Parsed command: object, file attr, method, **`param_t`**, value JSON, buffers — **`he_free`**. |
| **Parse** | **`string2he`**, **`json2he`**; **serialize** **`he2json`**, **`he2string`**. |
| **Execute** | **`he_execute`**, **`string_he_execute`**, **`json_he_execute`**, **`talk_he_command`** (batch array); **`line_he_command`** for terminal-oriented use. |

Results are usually **`talk_t`** JSON (**`talk_free`** if not a sentinel) or **`ttrue` / `tfalse` / `NULL` / `terror` / `tpanic`** per `he2com.h`. Ownership: anything attached to **`he_t`** is freed by **`he_free`**; execution APIs document whether returned JSON is new.

---

### 12.1 HE Command Types

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

### 12.2 HE Structure Operations

#### string2he / json2he
```c
he_t string2he(const char *cmd);
he_t json2he(talk_t cmd);
```
**Description:** Parse HE command from string/JSON

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

### 12.3 HE Command Execution

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

### 12.4 Sample program (every `he2com.h` function)

The **`HE_*`** symbols are **integer constants** (see §12.1); the first line forces them to appear in the snippet. **`json2he()`** expects at least **`"obj"`**; for a call shape, add **`"op"`** with the method name (see `he2com.c`).

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

## 13. Linked List API (link.h)

### 13.1 Macro Definitions

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

### 13.2 Linked List Operations

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

## 14. Utility Functions API (utility.h)

This chapter lists the most common entry points. **`utility.h` declares many more helpers** (network, shell, sockets, UART, time, etc.); refer to the header for full prototypes.

**Conventions (typical patterns, not a guarantee for every symbol):**

- Many functions indicate failure with **negative integers** or **`false`** and set **`errno`** where a system call failed; check the declaration’s comment in `utility.h`.
- Allocating helpers (`md5_encode`, `b64_encode`, `url_encode`, …) usually return **heap-allocated** buffers; **caller frees** unless the header states otherwise.
- **Thread safety:** utilities are not uniformly re-entrant; treat global process state, static buffers (if any), and subprocess/shell helpers as **non-thread-safe** unless documented.

### 14.1 String Processing

#### char2char / low2upp / upp2low
```c
void char2char(char *src, char a, char b);
void low2upp(char *str);
void upp2low(char *str);
```
**Description:** Character replacement/To uppercase/To lowercase

### 14.2 Encoding/Decoding

#### md5_encode / b64_encode / b64_decode
```c
char *md5_encode(const char *s, int len);
char *b64_encode(const char *s, int len);
char *b64_decode(const char *s, int *len);
```
**Description:** MD5/Base64 encode/decode

#### url_encode / url_decode
```c
char *url_encode(char const *s, int len, int *new_length);
int url_decode(char *str, int len);
```
**Description:** URL encode/decode

#### simple_encode / simple_decode
```c
char *simple_encode(const char *message, const char *key);
char *simple_decode(const char *message, const char *key);
```
**Description:** Simple encryption/decryption

#### string2hex / hex2string / hex2printf
```c
void string2hex(const char *src, char *dest, int len);
void hex2string(const char *src, char *dest, int len);
void hex2printf(const char *src, char *dest, int len);
```
**Description:** String and hex conversion

### 14.3 MAC Address Processing

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

### 14.4 Signal Handling

#### signal_noprocess / signal_register
```c
void signal_noprocess(int signo);
sighandler_t signal_register(int signo, sighandler_t func, int sa_flags);
```
**Description:** No-op signal handler/Register signal handler

### 14.5 Directory Operations

#### directory_size / directory_subsize / directory_sum / directory_subsum
```c
int directory_size(const char *dir);
int directory_subsize(const char *dir);
int directory_sum(const char *dir);
int directory_subsum(const char *dir);
```
**Description:** Get directory size or entry count. **`directory_subsize` and `directory_sum` are declared in the header but not implemented in this tree** (linking a call will fail); use `directory_size` / `directory_subsum` or add implementations if you need them.

### 14.6 File Locking

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

### 14.7 File Read/Write

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

### 14.8 Time Functions

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

### 14.9 System Commands

#### shell / execute / silent_execute
```c
int shell(const char *format, ...);
int execute(int timeout, boole silent, const char *format, ...);
#define silent_execute(...) execute(0, 1, __VA_ARGS__)
```
**Description:** Execute shell commands (with timeout control)

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
**Description:** Load/Unload/Check kernel modules

### 14.10 Network Tools

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

### 14.11 Socket Tools

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

### 14.12 Talk Transmission

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

### 14.13 System Tools

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

#### fileline_merge
```c
boole fileline_merge(const char *gap, const char *src, const char *adjust, const char *merge);
```
**Description:** Merge file lines

### 14.14 UART

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

## 15. Skin API Macros (skinapi.h)

### 15.1 Memory and Format Error Handling

```c
#define memory_exit(i) do { default_fault("memory oops"); exit(i); } while(0)
#define format_error(string) do { default_fault("format oops: %s", string); } while(0)
```

### 15.2 FPK API

```c
#define fpk_list(...) scalls(FPK_COM, "list", __VA_ARGS__)
#define fpk_register(...) scalls(FPK_COM, "register", __VA_ARGS__)
#define fpk_unregister(...) scalls(FPK_COM, "unregister", __VA_ARGS__)
```

### 15.3 Init API

```c
#define init_list(...) scalls(INIT_COM, "list", __VA_ARGS__)
#define init_register(item, call) scall2s(INIT_COM, "register", item, call)
```

### 15.4 Uninit API

```c
#define uninit_list(...) scalls(INIT_COM, "list", __VA_ARGS__)
#define uninit_register(item, call) scall2s(UNINIT_COM, "register", item, call)
```

**Note:** `uninit_list` is expanded exactly as in `skinapi.h` (it uses `INIT_COM` for `"list"`, same token sequence as `init_list`). If you intended a separate uninit-only listing API, confirm the daemon’s routing; the macro matches the header as shipped.

### 15.5 Joint API

```c
#define joint_list(...) scalls(JOINT_COM, "list", __VA_ARGS__)
#define joint_register(item, call) scall2s(JOINT_COM, "register", item, call)
#define joint_unregister(item, call) scall2s(JOINT_COM, "unregister", item, call)
#define joint_calls(joint, string) scall2s(JOINT_COM, "call", joint, string)
#define joint_callt(joint, json) scallst(JOINT_COM, "call", joint, json)
```

### 15.6 Machine API

```c
#define machine_config(...) sgets(MACHINE_COM, __VA_ARGS__)
#define machine_status(...) scalls(MACHINE_COM, "status", __VA_ARGS__)
#define machine_restart(delay, key) scalls(MACHINE_COM, "restart", "%d,%s", delay, key?:"")
#define machine_reboot(delay, key) scalls(MACHINE_COM, "reboot", "%d,%s", delay, key?:"")
#define machine_default(delay, key) scalls(MACHINE_COM, "default", "%d,%s", delay, key?:"")
```

**Note:** `key?:""` is a **GNU C extension** (empty string if `key` is null). It requires a compiler that supports this extension (e.g. GCC/Clang with GNU extensions). For strict ISO C, pass `key ? key : ""` instead.

---

## 16. Predefined Component Constants (skinhead.h)

### 16.1 Hardware Project Components

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

### 16.2 Core Project Components

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

### 16.3 Network Project Components

```c
#define NETWORK_COM     "network@frame"
#define CONNECT_COM     "network@connect"
#define KEEPLIVE_COM    "network@keeplive"
#define BRIDGE_COM      "network@bridge"
#define VLAN_COM        "network@vlan"
#define HOSTS_COM       "network@hosts"
```

### 16.4 Interface Name Components

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

### 16.5 Forward Project Components

```c
#define NAT_COM         "forward@nat"
#define DNAT_COM        "forward@dnat"
#define FIREWALL_COM    "forward@firewall"
#define RULE_COM        "forward@rule"
#define ROUTE_COM       "forward@route"
#define ROUTES_COM      "forward@routes"
```

### 16.6 Wireless Project Components

```c
#define WIFI_AP_COM     "wifi@ap"
#define WIFI_STA_COM    "wifi@sta"
#define NRADIO_COM      "wifi@n"
#define NSSID_COM       "wifi@nssid"
#define NSTA_COM        "wifi@nsta"
```

### 16.7 Modem Components

```c
#define OPERATOR_COM    "modem@operator"
#define MODEM_COM       "modem@atd"
#define LTE_COM         "modem@lte"
#define SMSD_COM        "modem@smsd"
#define SMS_COM         "modem@sms"
#define ATPROXY_COM     "modem@atproxy"
```

### 16.8 UART Project Components

```c
#define UART_COM        "uart@frame"
#define SERIAL_COM      "uart@serial"
#define SERIAL2_COM     "uart@serial2"
```

---

## 17. Complete Usage Examples

### 17.1 Basic JSON Operations

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

### 17.2 Component Communication

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

### 17.3 Configuration Management

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

### 17.4 Registry Operations

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
    
    // Use pointer to modify
    int *p = reg_sintp("land@machine", "boot_count");
    (*p)++;
    register_ssync("land@machine");
    
    return 0;
}
```

### 17.5 Service Management

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

### 17.6 HE Command Execution

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

### 17.7 Network Tools

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

### 17.8 File Operations

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

### 17.9 Logging

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

### 17.10 Comprehensive Example: Configuration Management Tool

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

## 18. Compilation and Usage

### 18.1 Include headers

**Umbrella (recommended for apps and quick ports):**

```c
#include "skin.h"   /* stdhead.h + skinhead.h + skinapi.h */
```

**Layered (components / minimal includes):**

- `skinhead.h` — lengths, `boole`, `PROJECT_OBJECT_GAPS`, `MACHINE_COM`, …
- `skinapi.h` — **does not include other headers**; it only expands macros such as `scalls()` / `machine_config()`. You must include **`com.h`** (for `scalls`, `obj_t`, …) and **`skinhead.h`** (for `MACHINE_COM`, `INIT_COM`, …) *before* `skinapi.h`, unless you already use `skin.h`.
- Individual modules: `talk.h`, `com.h`, `register.h`, `config.h`, `utility.h`, … (include what you use; add `skinhead.h` / `stdhead.h` if typedefs or limits are missing)

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

## 19. Important Notes

1. **Memory management:** Use `talk_free()` for `talk_t` values allocated by the talk/JSON APIs; use `free()` for `json2string()` and typical `utility.h` allocators unless the API says otherwise.
2. **Error handling:** Check return values and `errno` after failures; for `scall*` / `ccall*`, handle `tpanic`, `terror`, `tfalse`, and JSON results (see §1.1 and `com.h`).
3. **Thread safety:** There is **no blanket guarantee** across libskin. Assume **non-thread-safe** unless stated; use `register_lock` / process-level locking for mmap’d register files and shared resources; `utility.h` shell/network helpers often invoke subprocesses or global state.
4. **Path format:** Component paths use `project@component`; configuration / attribute paths commonly use `level1/level2` (see `path.h`, `OBJECT_CONFIG_GAPS`).
5. **Log levels:** Tune level and output options for production; avoid verbose/debug spam (see §9.3.1 for composing `flags`).

---

## 20. Related Documents

- `skin/com.h`, `skin/talk.h`, … — source of truth for prototypes and Doxygen-style comments
- `skin.h` — umbrella include
- `skinhead.h` — constants and component name macros
- `skinapi.h` — convenience macros (`scalls`, `machine_*`, …)
- Optional companion docs (if present in your tree): `COM_API.md`, `SKIN_API.md`

---

*Document Version: 1.0*
*Last Updated: 2026-03-22*
# Component interface doc — detailed rules

This file is the full rulebook formerly in `doc/dev/component_doc_guide.md`.  
**Layout ground truth:** `doc/com/land/auth.md`.  
**Skill entry:** [SKILL.md](SKILL.md).

Teaching comments (`//''`, Chinese guide notes) must **never** appear in the shipped component `.md`.

---

## Basic conventions

1. Doc lives next to the component in the project tree; name is usually `<component-id>.md`.
2. Match **punctuation and spacing** of auth for every parallel block; wording is component-specific.
3. Omit optional sections with no content (including Architecture / Dependencies / Joint Events Hook / Concepts / Configuration* / Published Joint Events / Other).
4. If the component has **no** configuration, omit **all three** of: Configuration reference, Configuration example, Configuration settings example. Do not write “no configuration”.
5. Replace every `<>` placeholder with real names before shipping.
6. Final doc is **all English**.
7. Focus on the **interface** (usage, config, APIs, events). Avoid internal implementation and source filenames.
8. Do **not** document `get` / `set` as user-facing APIs (they back Configuration).

---

## Section order (auth-aligned)

| Order | Structure | Notes |
|------|-----------|--------|
| 1 | `## <id> — <English subtitle>` | Em dash `—` |
| 2 | `### Overview` | Paragraphs + `-` bullets; continuations: **4 spaces + `>` + space** |
| 3 | `### Configuration reference ( <id> )` | Optional; omit if no config |
| 4 | `` ```json `` … `` ``` `` | First line: `// Attributes introduction ` (trailing space) |
| 5 | `#### Configuration example` | Lowercase **example** |
| 6 | `#### Configuration settings example` | Lowercase **settings** / **example** |
| 7 | `### Concepts` | Optional; between Configuration (or Overview) and API Reference |
| 8 | `### API Reference` | |
| 9 | `#### Management APIs` | |
| 10 | `#### Query APIs` | |
| 11 | `#### Control APIs` | |
| 12 | `### Published Joint Events` | Optional |
| 13 | `### Other` | Optional |

Optional (not in auth): `### Architecture`, `### Dependencies`, `### Joint Events Hook`, `### Concepts` — omit when unused.

---

## Blank-line targets (re-check against current auth.md)

- After last Overview content → Configuration / Concepts / API Reference: **two** blank lines
- After Configuration `` ```json `` close → `#### Configuration example`: **one** blank line
- After last Configuration example sample → `#### Configuration settings example`: **one** blank line
- After last settings example → Concepts or API Reference: **two** blank lines
- After Concepts (if any) → API Reference: **two** blank lines
- After `### API Reference` → `#### Management APIs`: **one** blank line
- After last Management API → Query: **two** blank lines
- After last Query API → Control: **two** blank lines
- After last Control API → Published Joint Events: **two** blank lines
- After Joint Events table → Other: **two** blank lines

Do **not** use Markdown `---` between major sections.

---

## Configuration JSON block

### Structure

- First inner line: `// Attributes introduction ` (**trailing space** after `introduction`)
- English `//` comments + JSONC only; not strict `JSON.parse` JSON
- Attribute line pattern:

```
    "name": "value description",                    // [ type ], extra note
```

### Type comment patterns (from auth)

| Pattern | Meaning |
|---------|---------|
| `[ string ]` | String |
| `[ number ]` | Number |
| `[ "disable", "enable" ]` | Enum (comma + space) — used in some places |
| `[ json ]` | Top-level object type tag; nested expand with `{ json }` |
| `[ string ]: { json }` | String key → object value |
| `[ string ]: [ "disable","enable" ]` | String key → enum (**no** space after comma in **config** JSON) |

**Spacing split:** in **config** JSON enums often use `[ "disable","enable" ]` (no space after comma); in **API return** JSON use `[ "enable", "disable" ]` (space after comma). Mirror the nearest auth line; do not “normalize” both styles.

### Continuations & placeholders

- Enum meaning continuations: indent so `//` aligns with `[` of the type comment above.
- Variable property count:

```
    // "...":"..."  How many <kind> show how many properties
    // "...":{ ... }  How many <kind> show how many properties
```

- Empty string value = default: document that in the `//` comment.

### Nested objects

Indent nested fields four more spaces; recurse the same rules. Prefer `[ string ]: { json }` for maps of objects.

---

## Configuration example

- Heading: `#### Configuration example`
- Each sample: line `Example, <English…>` (capital E, comma, space) → blank line → `` ```shell ``
- Do **not** append `(HE / Shell; …)` on the `Example,` line
- Inside shell: command, then body; `#` comments column-aligned; **no** blank line before closing fence
- Cover as many attributes as practical

```
Example, show all the configure

```shell
<component-id>
{
    "attr":"value",                         # meaning
}
```
```

---

## Configuration settings example

At least **two** forms:

**(1) Single attribute**

```
Example, <intent>

```shell
<component-id>:<path/to/attribute>=<value>
ttrue
```
```

**(2) Merge set**

```
Example, merge set <intent>( include "<a>" "<b>" "<c>" )

```shell
<component-id>|{"<key>":{"<nested>":{"<a>":"<v1>","<b>":"<v2>","<c>":"<v3>"}}}
ttrue
```
```

`ttrue` immediately follows the command (no blank line).

---

## API Reference entries

### `+` line

- `+` + space + `` `api[ … ]` `` + space + `**English summary**`
- Mirror auth: usually **no** `project@` prefix on the `+` line; shell examples use `project@component.api[…]`
- No `*italic*` return text on the `+` line
- Trailing spaces after `**`: copy closest auth analogue when present
- Parameters: bare names only — `api[ a, b, c ]` not `api[ a, [b] ]`
- Zero-arg lifecycle often `setup[]` when source uses empty brackets; otherwise prefer `api` without brackets if truly no-arg and auth analogue has no `[]`

### `-` lines

- Four spaces + `-` + space + text
- Hyphen run between name and `[ type ]`: copy closest auth slot (lengths differ by API)
- Optional params: include `optional` in the description
- No trailing `//` on `failed return` / `succeed return` lines
- `error return terror` only if the closest auth analogue has it

### Return shapes

- Bool: `- failed return tfalse` / `- succeed return ttrue`
- JSON: `- failed return NULL` / `- succeed return [ json ], …` then **mandatory** nested `` ```json `` (four-space indent before fence)
- Scalar: `- succeed return [ string ]` or `[ number ]`
- **Never** omit the JSON annotation block; **never** “same as X”

### Semantic types

- `[ number ]`: counters, fixed-unit numerics (kB, jiffies, ports, PIDs, percents as numbers), IDs
- `[ string ]`: names, paths, descriptions, unit-bearing strings (`"256M"`, `"50%"`), enums

### Classification

| Group | Examples |
|-------|----------|
| Management | setup, shut, service, online, offline |
| Query | list, status, info, check (read-only) |
| Control | add, delete, modify, reset, change |

Same symbol, different arity/semantics → list under both Query and Control as needed, each describing only that use.

### Examples under an API

`Example, …` → blank → `` ```shell `` → `project@component.api[ … ]` + output → fence.  
Blank line between two Examples: follow closest auth API.

---

## Published Joint Events

- English intro paragraph before the table; bold the joint registration name (e.g. **`land@joint`**)
- Header lines **exact**:

```
| Event | Description |
|-------|-------------|
```

- Event names in backticks; English trigger/params in Description
- Omit whole section if the component publishes nothing

---

## Other

- Heading `### Other` only
- Catch-all for material that does not fit earlier sections (including odd APIs)
- Do **not** require auth’s “Some helpful APIs” lead-in or its utility list
- APIs here still use `+` / `-` style
- Omit if empty

---

## Hard bans (repeat)

- No `#### Configuration attributes`
- No `#####` API headings
- No `//''` / teaching `//'…'`
- No `---` between major sections
- No dedicated `get` / `set` user-API docs
- No “has no configuration” filler
- No omitting JSON blocks after `[ json ]` success
- No `[param]` wrappers on the `+` line
- No `[ string ]` for semantic numbers
- No `*italic*` returns on the `+` line
- Shell `#` comments column-aligned

---

## English checklist (copy for AI turns)

Write the component interface document in English only. Match **`doc/com/land/auth.md`** section-for-section for everything auth contains: same Markdown heading literals, blank-line counts, `+` / `-` API layout, nested fence indentation, table header character lines, per-line punctuation (hyphen runs; trailing spaces after `**` when the closest auth analogue has them). Optional sections not in auth: Architecture, Dependencies, Joint Events Hook, Concepts — omit when unused. Omit all Configuration sections when there is no config. Do not emit teaching lines. Configuration JSON first line exactly `// Attributes introduction ` (trailing space). Do not add `#### Configuration attributes`. Do not use `#####` for APIs. Do not place `---` between major sections. Do not document `get` / `set` as user-facing APIs. Every `- succeed return [ json ]` must be followed by a full annotated `` ```json `` block (never “same as X”).

---

## Skeleton template

```markdown
## <component-id> — <short English intro>

### Overview

<What it solves; what it does not.>

- <capability>
- <capability>
    > <continuation>


### Configuration reference ( <component-id> )

```json
// Attributes introduction 
{
    "attr": "meaning",                              // [ string ], …
}
```

#### Configuration example

Example, show all the configure

```shell
<component-id>
{
    "attr":"value"                                  # meaning
}
```

#### Configuration settings example

Example, set one attribute

```shell
<component-id>:attr=value
ttrue
```

Example, merge set several attributes( include "a" "b" )

```shell
<component-id>|{"a":"1","b":"2"}
ttrue
```


### API Reference

#### Management APIs

+ `setup[]` **initialize the component**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation


#### Query APIs

+ `status[ ]` **…**
    - failed return NULL
    - succeed return [ json ], …
    ```json
    {
        "field": "…"  // [ string ], …
    }
    ```

    Example, …
    ```shell
    project@component.status
    {
        "field":"…"
    }
    ```


#### Control APIs

+ `reset[ ]` **…**
    - failed return tfalse
    - succeed return ttrue

    Example, …
    ```shell
    project@component.reset
    ttrue
    ```


### Published Joint Events

The following joint events are published when …. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `name/event` | When it fires; related API; params |
```

Adjust or delete optional blocks; fix `status[ ]` / spacing to match real APIs and auth analogues.

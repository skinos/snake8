#!/usr/bin/env python3
"""Generate clangd index files under a single .map/ directory.

Usage:
  python3 tools/clangd_all.py [sdk_root]

Output (all under <sdk_root>/.map/, plus a tiny root .clangd pointer):
  .map/<unit>/compile_commands.json   # one DB per project / config package
  .clangd                             # PathMatch → .map/<unit>

Design:
  - Components in different projects do not call each other.
  - Each unit indexes its own com/exe/lib/cmd (from prj.json) plus
    dependent projects' lib/ trees (from Makefile DEPENDS).
  - Skips osc/, uboot, atf, adjust, rootfs copies — those dominate size/slowness.
  - Delete anytime:  rm -rf .map .clangd
"""

from __future__ import annotations

import json
import os
import re
import shutil
import sys

MAP_DIRNAME = ".map"
SKIP_DIR_NAMES = {
    ".git", ".svn", ".hg", ".repo",
    "node_modules", "__pycache__", ".fpk", ".cache",
    "build", "osc",
}

# prj.json keys whose directory names are indexed for the owning project
OWN_UNIT_KEYS = ("com", "exe", "lib", "cmd")

# Only these keys are pulled from dependency projects
DEP_UNIT_KEYS = ("lib",)

# Known skinos project names that may appear bare in PKG_BUILD_DEPENDS
KNOWN_PROJECTS = {
    "land", "network", "modem", "uart", "webs", "wui", "tui",
    "forward", "vpn", "nvpn", "ipsec", "camera", "gnss", "agent",
    "client", "wifi", "storage", "clock", "snmp", "ddns", "ifname",
}

CONFIG_SKIP_PATH_PARTS = {
    "uboot", "atf", "adjust", "rootfs", "dl", "boot", "kernel",
}


def find_sdk_root(start: str) -> str | None:
    p = os.path.abspath(start)
    while True:
        has_project = os.path.isdir(os.path.join(p, "project"))
        has_tools = os.path.isdir(os.path.join(p, "tools"))
        if has_project and has_tools:
            return p
        parent = os.path.dirname(p)
        if parent == p:
            return None
        p = parent


def load_json(path: str) -> dict | None:
    try:
        with open(path, encoding="utf-8") as f:
            data = json.load(f)
    except (OSError, ValueError, TypeError):
        return None
    if isinstance(data, dict):
        return data
    return None


def load_project_id(root: str, data: dict | None = None) -> str:
    if data is None:
        data = load_json(os.path.join(root, "prj.json"))
    if data is not None:
        name = data.get("name")
        if isinstance(name, str) and name:
            return name
    base = os.path.basename(os.path.abspath(root).rstrip(os.sep))
    if base:
        return base
    return "unknown"


def unit_dirs_from_prj(prj: dict, keys: tuple[str, ...]) -> list[str]:
    names: list[str] = []
    seen: set[str] = set()
    for key in keys:
        block = prj.get(key)
        if not isinstance(block, dict):
            continue
        for name in block.keys():
            if not isinstance(name, str) or not name:
                continue
            if name in seen:
                continue
            seen.add(name)
            names.append(name)
    return names


def collect_c_under(tree_root: str) -> list[str]:
    files: list[str] = []
    if not os.path.isdir(tree_root):
        return files
    for dirpath, dirnames, filenames in os.walk(tree_root):
        filtered: list[str] = []
        for d in sorted(dirnames):
            if d.startswith("."):
                continue
            if d in SKIP_DIR_NAMES:
                continue
            filtered.append(d)
        dirnames[:] = filtered
        for name in sorted(filenames):
            if name.endswith(".c"):
                files.append(os.path.join(dirpath, name))
    return files


def collect_unit_c_files(project_root: str, unit_names: list[str]) -> list[str]:
    files: list[str] = []
    for name in unit_names:
        unit_path = os.path.join(project_root, name)
        if not os.path.isdir(unit_path):
            continue
        unit_files = collect_c_under(unit_path)
        files.extend(unit_files)
    return files


def read_makefile_text(project_root: str) -> str:
    path = os.path.join(project_root, "Makefile")
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            return f.read()
    except OSError:
        return ""


def parse_dep_project_names(makefile_text: str) -> list[str]:
    """Extract skinos project deps from DEPENDS / PKG_BUILD_DEPENDS."""
    names: list[str] = []
    seen: set[str] = set()

    # Join line continuations for simple scanning
    text = makefile_text.replace("\\\n", " ")

    patterns = [
        re.compile(r"PKG_BUILD_DEPENDS\s*:?=\s*([^\n#]+)"),
        re.compile(r"^\s*DEPENDS\s*:?=\s*([^\n#]+)", re.MULTILINE),
    ]
    chunks: list[str] = []
    for pat in patterns:
        for m in pat.finditer(text):
            chunks.append(m.group(1))

    token_re = re.compile(r"[A-Za-z0-9_@.+-]+")
    for chunk in chunks:
        for tok in token_re.findall(chunk):
            name = tok.lstrip("+")
            # skinos_land / PACKAGE_skinos_land:skinos_land
            if "skinos_" in name:
                idx = name.rfind("skinos_")
                name = name[idx + len("skinos_"):]
            # Drop conditional package noise
            if ":" in name:
                name = name.split(":")[-1]
                if name.startswith("skinos_"):
                    name = name[len("skinos_"):]
            if name in ("arch",):
                continue
            if name not in KNOWN_PROJECTS and name not in seen:
                # allow unknown only if looks like a plain id
                if not re.fullmatch(r"[a-z][a-z0-9_]*", name):
                    continue
            if name in seen:
                continue
            if name in ("libevent2", "libopenssl", "libcurl", "libreadline",
                        "libmosquitto", "libmodbus", "librt", "libxcrypt",
                        "smstools3", "iconv", "dnsmasq", "snmpd", "proftpd"):
                continue
            seen.add(name)
            names.append(name)
    return names


def discover_project_units(sdk_root: str) -> list[dict]:
    """Find project/*/prj.json packages."""
    units: list[dict] = []
    project_root = os.path.join(sdk_root, "project")
    if not os.path.isdir(project_root):
        return units
    for name in sorted(os.listdir(project_root)):
        root = os.path.join(project_root, name)
        prj_path = os.path.join(root, "prj.json")
        if not os.path.isfile(prj_path):
            continue
        prj = load_json(prj_path)
        if prj is None:
            continue
        pid = load_project_id(root, prj)
        map_id = f"project-{pid}"
        path_match = f"project/{name}/.*"
        units.append({
            "map_id": map_id,
            "path_match": path_match,
            "root": root,
            "prj": prj,
            "project_id": pid,
            "kind": "project",
        })
    return units


def config_path_allowed(rel: str) -> bool:
    """Only real platform packages: arch / center / cdriver / pdriver."""
    parts = rel.split(os.sep)
    # expect: config/<platform>/<kind>/...
    if len(parts) < 3 or parts[0] != "config":
        return False
    kind = parts[2]
    if kind not in ("arch", "center", "cdriver", "pdriver"):
        return False
    for p in parts:
        if p in CONFIG_SKIP_PATH_PARTS:
            return False
    return True


def discover_config_units(sdk_root: str) -> list[dict]:
    """Find config/**/prj.json packages (arch/center/cdriver/pdriver, …)."""
    units: list[dict] = []
    config_root = os.path.join(sdk_root, "config")
    if not os.path.isdir(config_root):
        return units

    for dirpath, dirnames, filenames in os.walk(config_root):
        dirnames[:] = sorted(
            d for d in dirnames
            if not d.startswith(".") and d not in SKIP_DIR_NAMES
            and d not in CONFIG_SKIP_PATH_PARTS
        )
        if "prj.json" not in filenames:
            continue
        rel = os.path.relpath(dirpath, sdk_root)
        if not config_path_allowed(rel):
            continue
        prj = load_json(os.path.join(dirpath, "prj.json"))
        if prj is None:
            continue
        pid = load_project_id(dirpath, prj)
        # Stable id from relative path (already starts with config-)
        map_id = rel.replace(os.sep, "-")
        path_match = rel.replace("\\", "/") + "/.*"
        units.append({
            "map_id": map_id,
            "path_match": path_match,
            "root": dirpath,
            "prj": prj,
            "project_id": pid,
            "kind": "config",
        })
    return units


def resolve_dep_roots(sdk_root: str, dep_names: list[str]) -> list[str]:
    roots: list[str] = []
    seen: set[str] = set()
    for name in dep_names:
        root = os.path.join(sdk_root, "project", name)
        real = os.path.realpath(root)
        if real in seen:
            continue
        if not os.path.isdir(root):
            continue
        if not os.path.isfile(os.path.join(root, "prj.json")):
            continue
        seen.add(real)
        roots.append(root)
    return roots


def include_flags_for(
    sdk_root: str,
    own_root: str,
    dep_roots: list[str],
) -> list[str]:
    flags = [f"-I{own_root}", f"-I{sdk_root}"]
    land = os.path.join(sdk_root, "project", "land")
    if os.path.isdir(land):
        flags.append(f"-I{land}")
    install_inc = os.path.join(sdk_root, "build", "install", "include")
    if os.path.isdir(install_inc):
        flags.append(f"-I{install_inc}")
    for root in dep_roots:
        flags.append(f"-I{root}")

    seen: set[str] = set()
    out: list[str] = []
    for f in flags:
        if f in seen:
            continue
        seen.add(f)
        out.append(f)
    return out


def com_id_for_file(project_root: str, filepath: str, project_id: str) -> str:
    rel = os.path.relpath(filepath, project_root)
    parts = rel.split(os.sep)
    if len(parts) > 1 and parts[0] not in (".", ""):
        return parts[0]
    return project_id


def make_entry(
    filepath: str,
    project_root: str,
    project_id: str,
    include_flags: list[str],
) -> dict:
    cdir = os.path.dirname(filepath)
    name = os.path.basename(filepath)
    com = com_id_for_file(project_root, filepath, project_id)
    defines = [
        f'-DPROJECT_ID="{project_id}"',
        f'-DCOM_ID="{com}"',
        f'-DCOM_IDPATH="{project_id}@{com}"',
        f'-DEXE_ID="{com}"',
        f'-DEXE_IDPATH="{project_id}@{com}"',
        f'-DLIB_ID="{com}"',
        f'-DLIB_IDPATH="{project_id}@{com}"',
    ]
    args = [
        "gcc",
        "-fsyntax-only",
        "-I.",
        *include_flags,
        "-std=gnu11",
        "-D_GNU_SOURCE",
        "-Wno-error",
        *defines,
        name,
    ]
    return {
        "directory": cdir,
        "file": name,
        "arguments": args,
    }


def build_entries_for_unit(sdk_root: str, unit: dict) -> list[dict]:
    own_root = unit["root"]
    own_prj = unit["prj"]
    own_id = unit["project_id"]
    own_real = os.path.realpath(own_root)

    own_units = unit_dirs_from_prj(own_prj, OWN_UNIT_KEYS)
    files_own = collect_unit_c_files(own_root, own_units)

    makefile_text = read_makefile_text(own_root)
    dep_names = parse_dep_project_names(makefile_text)
    # Always pull land libs unless this unit is land itself
    if own_id != "land" and "land" not in dep_names:
        dep_names.insert(0, "land")
    dep_roots = resolve_dep_roots(sdk_root, dep_names)

    # Source trees to index: own units + each dep's lib/ only
    trees: list[tuple[str, str, list[str]]] = []
    trees.append((own_root, own_id, files_own))

    for dep_root in dep_roots:
        dep_real = os.path.realpath(dep_root)
        if dep_real == own_real:
            continue
        dep_prj = load_json(os.path.join(dep_root, "prj.json"))
        if dep_prj is None:
            continue
        dep_id = load_project_id(dep_root, dep_prj)
        dep_lib_names = unit_dirs_from_prj(dep_prj, DEP_UNIT_KEYS)
        dep_files = collect_unit_c_files(dep_root, dep_lib_names)
        trees.append((dep_root, dep_id, dep_files))

    includes = include_flags_for(sdk_root, own_root, dep_roots)
    entries: list[dict] = []
    seen: set[str] = set()
    for tree_root, tree_id, file_list in trees:
        for filepath in file_list:
            real = os.path.realpath(filepath)
            if real in seen:
                continue
            seen.add(real)
            entry = make_entry(filepath, tree_root, tree_id, includes)
            entries.append(entry)

    entries.sort(key=lambda e: (e["directory"], e["file"]))
    return entries


def write_json(path: str, entries: list[dict]) -> None:
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(entries, f, indent=2)
        f.write("\n")


def write_clangd(sdk_root: str, units: list[dict]) -> str:
    """Tiny root .clangd: route each source tree to its .map/<id> DB."""
    lines: list[str] = [
        "# Generated by make map (tools/clangd_all.py).",
        "# Intermediate; safe to delete. Regenerate: make map",
        "# All compile DBs live under .map/",
        "",
    ]
    for i, unit in enumerate(units):
        if i > 0:
            lines.append("---")
            lines.append("")
        lines.append("If:")
        lines.append(f"  PathMatch: {unit['path_match']}")
        lines.append("CompileFlags:")
        lines.append(f"  CompilationDatabase: {MAP_DIRNAME}/{unit['map_id']}")
        lines.append("")

    path = os.path.join(sdk_root, ".clangd")
    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    return path


def remove_stale_root_cdb(sdk_root: str) -> None:
    """Remove the old monolithic compile_commands.json if present."""
    legacy = os.path.join(sdk_root, "compile_commands.json")
    if os.path.isfile(legacy):
        try:
            os.remove(legacy)
        except OSError:
            pass


def main() -> int:
    if len(sys.argv) > 1:
        sdk_root = os.path.abspath(sys.argv[1])
    else:
        here = os.path.dirname(os.path.abspath(__file__))
        sdk_root = os.path.dirname(here)

    if not os.path.isdir(sdk_root):
        print(f"error: not a directory: {sdk_root}", file=sys.stderr)
        return 1
    if not os.path.isdir(os.path.join(sdk_root, "project")):
        print(f"error: no project/ dir under: {sdk_root}", file=sys.stderr)
        return 1

    map_root = os.path.join(sdk_root, MAP_DIRNAME)
    if os.path.isdir(map_root):
        shutil.rmtree(map_root)
    os.makedirs(map_root, exist_ok=True)

    units = discover_project_units(sdk_root)
    config_units = discover_config_units(sdk_root)
    units.extend(config_units)

    total_entries = 0
    summaries: list[str] = []
    for unit in units:
        entries = build_entries_for_unit(sdk_root, unit)
        out = os.path.join(map_root, unit["map_id"], "compile_commands.json")
        write_json(out, entries)
        total_entries += len(entries)
        summaries.append(f"{unit['map_id']}={len(entries)}")

    clangd_path = write_clangd(sdk_root, units)
    remove_stale_root_cdb(sdk_root)

    print(
        f"{map_root}/  {len(units)} databases, {total_entries} entries total"
    )
    print(f"{clangd_path}  (PathMatch → {MAP_DIRNAME}/<unit>)")
    # Keep summary readable
    if len(summaries) <= 40:
        print("  " + ", ".join(summaries))
    else:
        print("  " + ", ".join(summaries[:40]) + f", … (+{len(summaries) - 40})")
    print("Delete anytime:  rm -rf .map .clangd")
    return 0


if __name__ == "__main__":
    sys.exit(main())

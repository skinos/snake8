#!/usr/bin/env python3
"""Generate compile_commands.json for clangd / Cursor go-to-definition.

Usage:
  python3 ~/snake8/tools/clangd_sym.py [project_dir]

  project_dir  skinos project root (directory with prj.json), or any C tree.
               Default: directory of this script (legacy).

Output:
  <project_dir>/compile_commands.json

Indexing:
  - Every .c under project_dir (skip hidden / junk dirs)
  - Also every .c under <sdk>/project/land when the SDK root is found, so
    jumps into libskin / land components work from any project after Reload.

Include search (host/clangd friendly, not a full cross compile):
  -I.                         # local headers next to the .c (gtog.h, skin.h, …)
  -I<project_root>
  -I<sdk>/project/land         # #include "skin/skin.h" → land sources
  -I<sdk>/build/install/include
  -I<sdk>/project/network      # skinnet (if present)
  -I<sdk>/project/modem        # skinmodem (if present)
  -I<sdk>/project/uart         # skinuart (if present)
"""

from __future__ import annotations

import json
import os
import sys

SKIP_DIR_NAMES = {
    ".git",
    ".svn",
    ".hg",
    ".repo",
    "node_modules",
    "__pycache__",
    ".fpk",
    "build",
}


def find_sdk_root(start: str) -> str | None:
    """Walk parents for snake8-style root: tools/ + project/land/."""
    p = os.path.abspath(start)
    while True:
        if os.path.isdir(os.path.join(p, "tools")) and os.path.isdir(
            os.path.join(p, "project", "land")
        ):
            return p
        parent = os.path.dirname(p)
        if parent == p:
            return None
        p = parent


def load_project_id(root: str) -> str:
    path = os.path.join(root, "prj.json")
    if os.path.isfile(path):
        try:
            with open(path, encoding="utf-8") as f:
                name = json.load(f).get("name")
            if isinstance(name, str) and name:
                return name
        except (OSError, ValueError, TypeError):
            pass
    return os.path.basename(os.path.abspath(root).rstrip(os.sep)) or "unknown"


def collect_c_files(tree_root: str) -> list[str]:
    files: list[str] = []
    for dirpath, dirnames, filenames in os.walk(tree_root):
        dirnames[:] = sorted(
            d
            for d in dirnames
            if not d.startswith(".") and d not in SKIP_DIR_NAMES
        )
        for name in sorted(filenames):
            if name.endswith(".c"):
                files.append(os.path.join(dirpath, name))
    return files


def com_id_for_file(project_root: str, filepath: str, project_id: str) -> str:
    rel = os.path.relpath(filepath, project_root)
    parts = rel.split(os.sep)
    if len(parts) > 1 and parts[0] not in (".", ""):
        return parts[0]
    return project_id


def common_include_flags(sdk_root: str | None, project_root: str) -> list[str]:
    flags = ["-I.", f"-I{project_root}"]
    if sdk_root is not None:
        land = os.path.join(sdk_root, "project", "land")
        if os.path.isdir(land):
            flags.append(f"-I{land}")

        install_inc = os.path.join(sdk_root, "build", "install", "include")
        if os.path.isdir(install_inc):
            flags.append(f"-I{install_inc}")

        for rel in ("project/network", "project/modem", "project/uart"):
            d = os.path.join(sdk_root, rel)
            if os.path.isdir(d):
                flags.append(f"-I{d}")

    # Keep order; drop duplicate -I paths (land tree is often also project_root).
    seen: set[str] = set()
    out: list[str] = []
    for f in flags:
        if f in seen:
            continue
        seen.add(f)
        out.append(f)
    return out


def make_entry(
    filepath: str,
    project_root: str,
    project_id: str,
    include_flags: list[str],
) -> dict:
    cdir = os.path.dirname(filepath)
    name = os.path.basename(filepath)
    com = com_id_for_file(project_root, filepath, project_id)
    # Stub build macros so COM_IDPATH / PROJECT_ID parse cleanly under clangd.
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


def main() -> int:
    if len(sys.argv) > 1:
        root = os.path.abspath(sys.argv[1])
    else:
        root = os.path.dirname(os.path.abspath(__file__))

    if not os.path.isdir(root):
        print(f"error: not a directory: {root}", file=sys.stderr)
        return 1

    sdk_root = find_sdk_root(root)
    project_id = load_project_id(root)
    land_root = (
        os.path.join(sdk_root, "project", "land") if sdk_root else None
    )
    if land_root and not os.path.isdir(land_root):
        land_root = None

    # Same physical tree as root → do not index land twice.
    root_real = os.path.realpath(root)
    land_real = os.path.realpath(land_root) if land_root else None
    include_land = land_real is not None and land_real != root_real

    trees: list[tuple[str, str]] = [(root, project_id)]
    if include_land:
        trees.append((land_root, load_project_id(land_root)))

    entries: list[dict] = []
    seen: set[str] = set()
    for tree_root, tree_id in trees:
        includes = common_include_flags(sdk_root, tree_root)
        for filepath in collect_c_files(tree_root):
            real = os.path.realpath(filepath)
            if real in seen:
                continue
            seen.add(real)
            entries.append(make_entry(filepath, tree_root, tree_id, includes))

    entries.sort(key=lambda e: (e["directory"], e["file"]))
    out = os.path.join(root, "compile_commands.json")
    with open(out, "w", encoding="utf-8") as f:
        json.dump(entries, f, indent=2)
        f.write("\n")

    land_n = 0
    if include_land:
        land_n = sum(1 for e in entries if e["directory"].startswith(land_real + os.sep) or e["directory"] == land_real)
    print(
        f"{out}  {len(entries)} entries"
        f"  (project={project_id}"
        + (f", land=+{land_n}" if include_land else ", land=skip")
        + (f", sdk={sdk_root}" if sdk_root else ", sdk=not-found")
        + ")"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())

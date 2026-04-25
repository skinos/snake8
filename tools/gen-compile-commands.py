#!/usr/bin/env python3
import json
import os
import sys

if len(sys.argv) > 1:
    root = os.path.abspath(sys.argv[1])
else:
    root = os.path.dirname(os.path.abspath(__file__))
entries = []
flags = ["gcc", "-fsyntax-only", "-I.", "-std=gnu11", "-D_GNU_SOURCE", "-Wno-error"]

for dirpath, dirnames, filenames in os.walk(root):
    dirnames[:] = [d for d in dirnames if not d.startswith(".")]
    for name in sorted(filenames):
        if not name.endswith(".c"):
            continue
        rel = os.path.relpath(os.path.join(dirpath, name), root)
        entries.append(
            {
                "directory": root,
                "file": rel,
                "arguments": flags + [rel],
            }
        )

entries.sort(key=lambda e: e["file"])
out = os.path.join(root, "compile_commands.json")
with open(out, "w", encoding="utf-8") as f:
    json.dump(entries, f, indent=2)
    f.write("\n")

print(out, len(entries), "entries")

#!/usr/bin/env python3
"""Check that active competitive-programming snippets remain paste-ready."""

from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

LOCAL_INCLUDE_RE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser()
    p.add_argument("--compiler", default=os.environ.get("CXX"), help="C++ compiler (default: clang++ then g++)")
    p.add_argument("--std", default="gnu++20")
    p.add_argument("-j", "--jobs", type=int, default=max(1, min(8, os.cpu_count() or 1)))
    p.add_argument("--compile", action="store_true", help="also syntax-check every active header independently")
    return p.parse_args()


def allowed_local_include(path: Path, include: str) -> bool:
    parts = path.parts
    return (
        len(parts) >= 3
        and parts[-3:-1] == ("algebra", "acted_monoid")
        and include.startswith("../monoid/")
    )


def syntax_check(compiler: str, std: str, root: Path, path: Path, extra_flags: list[str]) -> tuple[Path, str | None]:
    proc = subprocess.run(
        [compiler, f"-std={std}", *extra_flags, "-fsyntax-only", "-x", "c++", str(path)],
        cwd=root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    if proc.returncode == 0:
        return path, None
    return path, proc.stderr.strip()


def main() -> int:
    args = parse_args()
    root = Path(__file__).resolve().parents[1]

    compiler = args.compiler
    if compiler is None:
        compiler = shutil.which("clang++") or shutil.which("g++")
    if compiler is None:
        print("error: no C++ compiler found (tried clang++ and g++)", file=sys.stderr)
        return 2

    headers = sorted(
        p for p in root.rglob("*.h")
        if ".git" not in p.parts and "legacy" not in p.parts
    )

    convention_errors: list[str] = []
    for path in headers:
        rel = path.relative_to(root)
        text = path.read_text(errors="replace")

        if "#include <bits/stdc++.h>" not in text:
            convention_errors.append(f"{rel}: missing #include <bits/stdc++.h>")
        if "using namespace std;" not in text:
            convention_errors.append(f"{rel}: missing using namespace std;")

        for include in LOCAL_INCLUDE_RE.findall(text):
            if not allowed_local_include(rel, include):
                convention_errors.append(f'{rel}: unexpected local include "{include}"')

    if not args.compile:
        if convention_errors:
            print("Convention errors:")
            for err in convention_errors:
                print(f"  - {err}")
            return 1
        print(f"OK: {len(headers)} active headers satisfy the dependency convention.")
        print("Run again with --compile for the full independent syntax pass.")
        return 0

    failures: list[tuple[Path, str]] = []
    extra_flags: list[str] = []
    temp_dir: tempfile.TemporaryDirectory[str] | None = None
    # Loading bits/stdc++.h from a PCH makes hundreds of independent header checks much faster with Clang.
    if "clang" in Path(compiler).name:
        temp_dir = tempfile.TemporaryDirectory(prefix="cohle-snippets-")
        pch_header = Path(temp_dir.name) / "pch.hpp"
        pch_file = Path(temp_dir.name) / "pch.pch"
        pch_header.write_text("#include <bits/stdc++.h>\n")
        pch = subprocess.run(
            [compiler, f"-std={args.std}", "-x", "c++-header", str(pch_header), "-o", str(pch_file)],
            cwd=root, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True,
        )
        if pch.returncode == 0:
            extra_flags = ["-include-pch", str(pch_file)]

    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futs = [pool.submit(syntax_check, compiler, args.std, root, p, extra_flags) for p in headers]
        for fut in as_completed(futs):
            path, error = fut.result()
            if error is not None:
                failures.append((path.relative_to(root), error))

    if temp_dir is not None:
        temp_dir.cleanup()

    if convention_errors:
        print("Convention errors:")
        for err in convention_errors:
            print(f"  - {err}")

    if failures:
        print("Syntax failures:")
        for path, error in sorted(failures):
            first_lines = "\n".join(error.splitlines()[:12])
            print(f"\n[{path}]\n{first_lines}")

    if convention_errors or failures:
        return 1

    print(f"OK: {len(headers)} active headers are self-contained and syntax-check successfully.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

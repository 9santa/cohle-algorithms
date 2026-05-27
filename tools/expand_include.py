#!/usr/bin/env python3
"""Expand quoted local C/C++ includes into a single paste-ready file."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


INCLUDE_RE = re.compile(r'^(\s*)#\s*include\s*"([^"]+)"(.*)$')


class Expander:
    def __init__(
        self,
        include_roots: list[Path],
        markers: bool,
        strict: bool,
        omit_patterns: list[str],
    ) -> None:
        self.include_roots = [p.resolve() for p in include_roots]
        self.markers = markers
        self.strict = strict
        self.omit_patterns = omit_patterns
        self.seen: set[Path] = set()
        self.stack: list[Path] = []
        self.warnings: list[str] = []

    def expand(self, path: Path, *, is_root: bool = True) -> list[str]:
        path = path if path.is_absolute() else Path.cwd() / path
        if not path.exists():
            raise FileNotFoundError(path)
        path_id = path.resolve() if path.is_file() else path.absolute()

        if not is_root:
            if path_id in self.seen:
                return []
            self.seen.add(path_id)

        self.stack.append(path_id)
        out: list[str] = []
        omit_output = not is_root and self.should_omit(path)

        if self.markers and not is_root and not omit_output:
            out.append(f"// BEGIN {self.display_path(path)}\n")

        for line in path.read_text().splitlines(keepends=True):
            if not is_root and line.strip() == "#pragma once":
                continue

            match = INCLUDE_RE.match(line)
            if match is None:
                if not omit_output:
                    out.append(line)
                continue

            include_name = match.group(2)
            included = self.resolve_include(include_name, path.parent)
            if included is None:
                self.handle_unresolved(include_name, path)
                if not omit_output:
                    out.append(line)
                continue

            if included in self.stack:
                self.warnings.append(
                    f"skipping cyclic include {include_name!r} from {self.display_path(path)}"
                )
                continue

            out.extend(self.expand(included, is_root=False))

        if self.markers and not is_root and not omit_output:
            out.append(f"// END {self.display_path(path)}\n")

        self.stack.pop()
        return out

    def resolve_include(self, include_name: str, current_dir: Path) -> Path | None:
        candidates = [current_dir / include_name]
        candidates.extend(root / include_name for root in self.include_roots)

        for candidate in candidates:
            if candidate.is_file():
                return candidate.resolve()
        return None

    def handle_unresolved(self, include_name: str, path: Path) -> None:
        looks_local = "/" in include_name or include_name.endswith((".h", ".hpp", ".hh", ".hxx"))
        if not looks_local:
            return

        message = f"unresolved quoted include {include_name!r} from {self.display_path(path)}"
        if self.strict:
            raise FileNotFoundError(message)
        self.warnings.append(message)

    def display_path(self, path: Path) -> str:
        cwd = Path.cwd().resolve()
        try:
            return str(path.resolve().relative_to(cwd))
        except ValueError:
            return str(path)

    def should_omit(self, path: Path) -> bool:
        display = self.display_path(path)
        name = path.name
        for pattern in self.omit_patterns:
            if name == pattern or Path(display).match(pattern):
                return True
        return False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Expand quoted local C/C++ includes recursively and leave system includes untouched."
    )
    parser.add_argument("input", type=Path, help="source file or header to expand")
    parser.add_argument("-o", "--output", type=Path, help="write expanded output to this file")
    parser.add_argument(
        "-I",
        "--include-dir",
        action="append",
        type=Path,
        default=[],
        help="extra include search directory; can be passed multiple times",
    )
    parser.add_argument(
        "--markers",
        action="store_true",
        help="add // BEGIN and // END comments around expanded headers",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="fail when a quoted local-looking include cannot be resolved",
    )
    parser.add_argument(
        "--keep-core",
        action="store_true",
        help="include core.h headers instead of assuming your template already provides them",
    )
    parser.add_argument(
        "--omit",
        action="append",
        default=[],
        metavar="PATTERN",
        help="omit expanded output for local files matching PATTERN; can be passed multiple times",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    script_root = Path(__file__).resolve().parents[1]
    include_roots = [*args.include_dir, Path.cwd(), script_root]
    omit_patterns = list(args.omit)
    if not args.keep_core:
        omit_patterns.append("core.h")

    expander = Expander(
        include_roots,
        markers=args.markers,
        strict=args.strict,
        omit_patterns=omit_patterns,
    )

    try:
        lines = expander.expand(args.input)
    except OSError as exc:
        print(f"expand_include.py: error: {exc}", file=sys.stderr)
        return 1

    text = "".join(lines)
    if args.output is None:
        sys.stdout.write(text)
    else:
        args.output.write_text(text)

    for warning in expander.warnings:
        print(f"expand_include.py: warning: {warning}", file=sys.stderr)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

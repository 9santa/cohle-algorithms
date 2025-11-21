#!/usr/bin/env python3
import os
import sys

HEADER = """#include <bits/stdc++.h>

using namespace std;


"""

def process_cpp(path):
    # Read file
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()

    # If header is already present, skip
    if content.startswith("#include <bits/stdc++.h>"):
        print(f"✓ Already has header: {path}")
        return

    # Write new content with header prepended
    with open(path, "w", encoding="utf-8") as f:
        f.write(HEADER + content)

    print(f"✔ Added header: {path}")


def process_recursive(root):
    for current, dirs, files in os.walk(root):
        for file in files:
            if file.endswith(".cpp"):
                process_cpp(os.path.join(current, file))


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: add_header_recursive.py <directory>")
        sys.exit(1)

    process_recursive(sys.argv[1])


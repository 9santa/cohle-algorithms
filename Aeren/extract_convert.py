#!/usr/bin/env python3
import os
import re
import sys

DELETE_ORIGINAL = True  # set to True to delete .sublime-snippet files after conversion

def convert_file(path):
    with open(path, "r", encoding="utf-8") as f:
        text = f.read()

    match = re.search(r"<content><!\[CDATA\[(.*?)\]\]></content>", text, re.S)
    if not match:
        print(f"⚠ Skipping (no CDATA): {path}")
        return

    cpp_code = match.group(1).strip()
    outpath = os.path.splitext(path)[0] + ".cpp"

    with open(outpath, "w", encoding="utf-8") as f:
        f.write(cpp_code + "\n")

    print(f"✔ Converted: {path} → {outpath}")

    if DELETE_ORIGINAL:
        os.remove(path)
        print(f"🗑 Deleted original: {path}")


def convert_recursive(root):
    for current_path, dirs, files in os.walk(root):
        for file in files:
            if file.endswith(".sublime-snippet"):
                full_path = os.path.join(current_path, file)
                convert_file(full_path)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: convert_snippets_recursive.py <directory>")
        sys.exit(1)

    root_dir = sys.argv[1]
    convert_recursive(root_dir)


#!/usr/bin/env python3
import os
import re

# Replacement mapping: OLD -> NEW
REPLACEMENTS = {
"SceneEditorMeshMesh":"SceneEditorMesh"
}

# Directory to skip
SKIP_DIR = "include"

# File extensions to process
EXTENSIONS = (".c", ".h")

def process_file(filepath):
    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()

    original_content = content

    for old, new in REPLACEMENTS.items():
        # Replace whole words only
        content = content.replace(old, new)

    if content != original_content:
        with open(filepath, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"Updated {filepath}")

def main():
    for root, dirs, files in os.walk("."):
        # Skip the include directory
        if SKIP_DIR in dirs:
            dirs.remove(SKIP_DIR)

        for file in files:
            if file.endswith(EXTENSIONS):
                process_file(os.path.join(root, file))

if __name__ == "__main__":
    main()

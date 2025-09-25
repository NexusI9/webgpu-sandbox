import os
import re

# Directories/files we want to skip
EXCLUDE_DIRS = {"include", os.path.join("backend", "context.c"), os.path.join("backend", "context.h")}

# Regex patterns
remove_decls = [
    r'\b(const\s+)?WGPUQueue\s+\w+\s*;?',   # variable declarations
    r'\b(const\s+)?WGPUDevice\s+\w+\s*;?',  # variable declarations
]

remove_func_args = [
    r'\b(const\s+)?(WGPUQueue|WGPUDevice)\s+\w+\s*,?\s*',  # function arguments
]

remove_struct_fields = [
    r'\s*\.queue\s*=\s*[^,]+,?',   # struct initializer fields
    r'\s*\.device\s*=\s*[^,]+,?',  # struct initializer fields
]

def should_skip(path):
    for ex in EXCLUDE_DIRS:
        if path.endswith(ex) or path.startswith(ex + os.sep):
            return True
    return False

def process_file(path):
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()

    new_content = content

    # Remove standalone declarations
    for pattern in remove_decls:
        new_content = re.sub(pattern, "", new_content)

    # Remove function arguments
    for pattern in remove_func_args:
        new_content = re.sub(pattern, "", new_content)

    # Remove struct initializers (.queue / .device)
    for pattern in remove_struct_fields:
        new_content = re.sub(pattern, "", new_content)

    # Cleanup common artifacts
    new_content = re.sub(r'\(\s*,\s*\)', '()', new_content)  # "(,)" → "()"
    new_content = re.sub(r',\s*\)', ')', new_content)        # ", )" → ")"
    new_content = re.sub(r'\(\s*,', '(', new_content)        # "( ," → "("
    new_content = re.sub(r',\s*,', ',', new_content)         # ",," → ","
    new_content = re.sub(r'\n\s*\n\s*\n+', '\n\n', new_content)  # collapse empty lines

    if new_content != content:
        with open(path, "w", encoding="utf-8") as f:
            f.write(new_content)
        print(f"Cleaned: {path}")

def walk_project(root="."):
    for dirpath, dirnames, filenames in os.walk(root):
        # skip excluded dirs
        if any(dirpath.startswith(ex) for ex in EXCLUDE_DIRS if os.path.isdir(ex)):
            continue

        for filename in filenames:
            if filename.endswith((".c", ".h")):
                path = os.path.join(dirpath, filename)
                if should_skip(path):
                    continue
                process_file(path)

if __name__ == "__main__":
    walk_project(".")

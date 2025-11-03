import os

# Root directory to start from (use "." for current directory)
ROOT_DIR = "."

# File extensions to target
EXTENSIONS = (".c", ".cpp", ".h", ".hpp")

# Replacement rules (order matters to avoid conflicts)
REPLACEMENTS = [
    ("SCENE_RENDERER", "theme_icon_atlas(gui->theme)")
]

def process_file(filepath):
    """Read file, apply replacements, and overwrite if changed."""
    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()

    new_content = content
    for old, new in REPLACEMENTS:
        new_content = new_content.replace(old, new)

    if new_content != content:
        # Backup original file
        backup_path = filepath + ".bak"
        with open(backup_path, "w", encoding="utf-8") as f:
            f.write(content)

        # Write modified file
        with open(filepath, "w", encoding="utf-8") as f:
            f.write(new_content)

        print(f"✔ Updated: {filepath}")
    else:
        print(f"– No changes: {filepath}")

def walk_directory(root):
    """Recursively find and process all target files."""
    for dirpath, _, filenames in os.walk(root):
        for filename in filenames:
            if filename.endswith(EXTENSIONS):
                filepath = os.path.join(dirpath, filename)
                process_file(filepath)

if __name__ == "__main__":
    walk_directory(ROOT_DIR)
    print("\n✅ Replacement complete.")

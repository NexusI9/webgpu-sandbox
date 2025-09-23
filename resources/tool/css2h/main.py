#!/usr/bin/env python3
#
#  css2h
#  Convert respectively each CSS files in the input directory to a .h file 
#  containing the css variables as ENUMS along with a table that map the 
#  values with each enum
#
#

import re
import os



INPUT_DIR = "input"
OUTPUT_DIR = "output"

def hex_to_rgba(hex_str):
    """Convert #RRGGBB or #RRGGBBAA to {r,g,b,a}"""
    hex_str = hex_str.strip()
    if hex_str.startswith("#"):
        hex_str = hex_str[1:]
    if len(hex_str) == 6:
        r = int(hex_str[0:2], 16) / 255
        g = int(hex_str[2:4], 16) / 255
        b = int(hex_str[4:6], 16) / 255
        a = 1.0
    elif len(hex_str) == 8:
        r = int(hex_str[0:2], 16) / 255
        g = int(hex_str[2:4], 16) / 255
        b = int(hex_str[4:6], 16) / 255
        a = int(hex_str[6:8], 16) / 255
    else:
        raise ValueError(f"Invalid hex color: {hex_str}")
    return f"{{ {r:.2f}f, {g:.2f}f, {b:.2f}f, {a:.2f}f }}"

def enum_name(css_name, filename):
    """Convert CSS variable name to C enum style"""
    name = css_name.upper().replace("-", "_")
    prefix = f"THEME_{filename.upper()}_"
    
    if name.startswith(prefix):
        return name
    return  prefix + name

def process_css_file(css_path, header_path):
    colors = []
    basename = os.path.basename(css_path)
    filename = os.path.splitext(basename)[0]
        
    with open(css_path, "r") as f:
        for line in f:
            match = re.match(r"\s*--([\w-]+):\s*#([0-9a-fA-F]{6,8});", line)
            if match:
                name, hex_val = match.groups()
                colors.append((enum_name(name, filename), hex_to_rgba(hex_val)))
    
    os.makedirs(os.path.dirname(header_path), exist_ok=True)
    with open(header_path, "w") as f:        
        # Header
        f.write("// Generated from {} with css2h\n\n".format(os.path.basename(css_path)))
        f.write(f"#ifndef _THEME_{filename.upper()}_H_\n")
        f.write(f"#define _THEME_{filename.upper()}_H_\n\n")
        f.write("#include \"utils/color.h\"\n\n")
        
        f.write(f"#define THEME_{filename.upper()}_COLOR_COUNT {len(colors)}\n\n")
        
        # Enum
        f.write("typedef enum {\n")
        for name, _ in colors:
            f.write(f"    {name},\n")
        f.write("}")
        f.write(f" Theme{filename.capitalize()}Color;\n\n")
        
        # Color array
        f.write(f"static const color theme_{filename.lower()}_color[THEME_{filename.upper()}_COLOR_COUNT] =")
        f.write("{\n")
        for name, rgba in colors:
            f.write(f"    [{name}] = {rgba},\n")
        f.write("};\n")

        # Footer
        f.write("#endif")

def traverse_and_generate(input_dir, output_dir):
    for root, _, files in os.walk(input_dir):
        for file in files:
            if file.endswith(".css"):
                css_path = os.path.join(root, file)
                relative_path = os.path.relpath(css_path, input_dir)
                header_path = os.path.join(output_dir, "theme." + os.path.splitext(relative_path)[0] + ".h")
                process_css_file(css_path, header_path)
                print(f"Generated {header_path}")

if __name__ == "__main__":
    traverse_and_generate(INPUT_DIR, OUTPUT_DIR)

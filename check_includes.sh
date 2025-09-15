#!/usr/bin/env bash
set -e

# ==========================
# CONFIGURATION
# ==========================
C_EXCLUDE="./resources/tool"
DEV_FLAGS="-O2 -g"
MACROS=""

# Include paths (from clangd)
INCLUDE_PATHS="-isystem/usr/local/Cellar/emscripten/4.0.1/libexec/cache/sysroot/include -isystem$(pwd)/include"

COMPILE_JSON="compile_commands.json"
IWYU_REPORT="iwyu_report.txt"

# ==========================
# 1. Find all C files
# ==========================
echo "Searching for C files..."
PRUNE_ARGS=""
for dir in $C_EXCLUDE; do
    PRUNE_ARGS="$PRUNE_ARGS -path $dir -prune -o"
done

C_FILES=$(find . $PRUNE_ARGS -name "*.c" -print)
if [ -z "$C_FILES" ]; then
    echo "No C files found. Check your C_EXCLUDE paths."
    exit 1
fi
echo "Found $(echo "$C_FILES" | wc -l) C files."

# ==========================
# 2. Generate compile_commands.json
# ==========================
echo "Generating $COMPILE_JSON..."
echo "[" > $COMPILE_JSON
for f in $C_FILES; do
    cat <<EOF >> $COMPILE_JSON
{
  "directory": "$(pwd)",
  "command": "emcc $DEV_FLAGS $MACROS $INCLUDE_PATHS -c \"$f\" -o \"${f%.c}.o\"",
  "file": "$f"
},
EOF
done

# Remove trailing comma on last entry
sed -i '' -e '$ s/},/}/' $COMPILE_JSON 2>/dev/null || sed -i -e '$ s/},/}/' $COMPILE_JSON
echo "]" >> $COMPILE_JSON
echo "$COMPILE_JSON generated."

# ==========================
# 3. Run IWYU
# ==========================
echo "Running IWYU..."
iwyu_tool.py -p . > "$IWYU_REPORT" 2>&1 || true
echo "IWYU analysis complete. Report saved to $IWYU_REPORT."

echo "Running Fix Includes..."
cat "$IWYU_REPORT" | fix_includes.py -n -p .
echo "Done."

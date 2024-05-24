#!/bin/bash

ROOT_DIR=$(pwd)

format_dir() {
    local dir="$1"
    find "$dir" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.inc" \) | while read -r file; do
        echo "Formatting $file"
        clang-format -i "$file"
    done
}

# format files in specified directories
for d in "FastCG" "samples"; do
    format_dir "$ROOT_DIR/$d"
done

echo "All sources have been formatted."
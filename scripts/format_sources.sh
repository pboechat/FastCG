#!/usr/bin/env bash

# Format C/C++ sources with clang-format.
# Usage:
#     scripts/format_source.sh               # format default dirs (FastCG, examples)
#     scripts/format_source.sh --staged      # format only staged files, then re-stage
#     scripts/format_source.sh --check       # CI: fail if formatting would change files
#     scripts/format_source.sh [files...]    # format only the given files
set -euo pipefail

CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd -P)"

cd "$REPO_ROOT"

DEFAULT_DIRS=("FastCG" "examples")
EXT_GLOB='*.c|*.cc|*.cpp|*.cxx|*.h|*.hh|*.hpp|*.inc'
CHECK=0
STAGED=0

FILES=()
while (($#)); do
    case "$1" in
        --check) CHECK=1;;
        --staged) STAGED=1;;
        -h|--help)
            sed -n '1,20p' "$0"; exit 0;;
        *) FILES+=("$1");;
    esac
    shift
done

has_clang_fmt() { command -v "$CLANG_FORMAT" >/dev/null 2>&1; }
has_clang_fmt || { echo "error: '$CLANG_FORMAT' not found in PATH" >&2; exit 127; }

collect_files() {
    local -a out=()
    if ((${#FILES[@]})); then
        for f in "${FILES[@]}"; do
            [[ -f "$f" ]] || continue
            case "$f" in
                *.c|*.cc|*.cpp|*.cxx|*.h|*.hh|*.hpp|*.inc) out+=("$f");;
            esac
        done
    elif ((STAGED)); then
        while IFS= read -r -d '' f; do
            if [[ "$f" == FastCG/* || "$f" == examples/* ]]; then
                case "$f" in
                    *.c|*.cc|*.cpp|*.cxx|*.h|*.hh|*.hpp|*.inc) out+=("$f");;
                esac
            fi
        done < <(git diff --cached --name-only --diff-filter=ACMR -z)
    else
        for d in "${DEFAULT_DIRS[@]}"; do
            [[ -d "$d" ]] || continue
            while IFS= read -r -d '' f; do
                out+=("$f")
            done < <(find "$d" -type f \( \
                    -name "*.c"     -o -name "*.cc"    -o -name "*.cpp" -o -name "*.cxx" -o \
                    -name "*.h"     -o -name "*.hh"    -o -name "*.hpp" -o -name "*.inc" \
                \) -print0)
        done
    fi
    printf '%s\0' "${out[@]}"
}

dedup_nul() {
    awk -v RS='\0' -v ORS='\0' '!seen[$0]++'
}

mapfile -d '' FILES_LIST < <(collect_files | dedup_nul)

(( ${#FILES_LIST[@]} )) || { echo "No files to format."; exit 0; }

if ((CHECK)); then
    if "$CLANG_FORMAT" --help 2>/dev/null | grep -q -- '--dry-run'; then
        FAILED=0
        for f in "${FILES_LIST[@]}"; do
            if ! "$CLANG_FORMAT" --dry-run -Werror "$f" 2>/dev/null; then
                echo "Would reformat: $f"
                FAILED=1
            fi
        done
        ((FAILED)) && { echo "Formatting required. Run: scripts/format_source.sh"; exit 2; }
        echo "Formatting OK."
        exit 0
    else
        # Fallback: compare actual output
        FAILED=0
        for f in "${FILES_LIST[@]}"; do
            if ! diff -q <("$CLANG_FORMAT" "$f") "$f" >/dev/null 2>&1; then
                echo "Would reformat: $f"
                FAILED=1
            fi
        done
        ((FAILED)) && { echo "Formatting required. Run: scripts/format_source.sh"; exit 2; }
        echo "Formatting OK."
        exit 0
    fi
else
    # Fix in place
    echo "Formatting ${#FILES_LIST[@]} file(s)..."
    "$CLANG_FORMAT" -i -- "${FILES_LIST[@]}"
    echo "Formatting done."

    # If we’re operating on staged content, re-stage it
    if ((STAGED)); then
        git add -- "${FILES_LIST[@]}" || true
    fi
fi

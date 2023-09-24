#!/bin/bash

# Take source directory, output directory, cmake root path, and exclusion paths as arguments
SOURCE_DIR="$(realpath "$1")"
OUTPUT_DIR="$(realpath "$2")"
CMAKE_ROOT="$3"
shift 3
EXCLUDE_PATHS=("$@")

# Convert each exclusion path to its absolute form
for i in "${!EXCLUDE_PATHS[@]}"; do
    EXCLUDE_PATHS[$i]="$(realpath "${EXCLUDE_PATHS[$i]}")"
done

# Dateiname für temporäre Speicherung der INCLUDE_DIRS
TEMP_FILE="temp_include_dirs.txt"

# Überprüfen, ob die TEMP_FILE bereits existiert
if [ -f "$TEMP_FILE" ]; then
    echo "Reading include directories from $TEMP_FILE..."
    INCLUDE_DIRS=$(cat "$TEMP_FILE")
else
    echo "Extracting include directories and saving to $TEMP_FILE..."

    # Extract include directories from CMake
    INCLUDE_DIRS=$(cmake --trace-expand "$CMAKE_ROOT" 2>&1 | \
                  grep -E 'add_compile_options.*-I|include_directories\(|target_include_directories\(' | \
                  sed -E -n 's/.*-I([^ ]*).*/\1/p;
                             s/include_directories\(([^ )]*)\)/\1/p;
                             s/target_include_directories\([^,]*,[^,]*,([^ )]*)\)/\1/p' | \
                  tr '\n' ' ')
    echo "$INCLUDE_DIRS"
    # Speichern der INCLUDE_DIRS in TEMP_FILE
    echo "$INCLUDE_DIRS" > "$TEMP_FILE"
fi

# Counter for found "FC_REFLECT" occurrences
FC_REFLECT_COUNT=0

# Check if the output directory exists and create it if it doesn't
if [ ! -d "$OUTPUT_DIR" ]; then
    echo "Creating output directory..."
    mkdir -p "$OUTPUT_DIR"
fi

# Display directories at the start
echo "Source directory: $SOURCE_DIR"
echo "Output directory: $OUTPUT_DIR"
echo "-----------------------------"

# Function to process files recursively
process_files() {
    for file in "$1"/*; do
        absolute_file_path=$(realpath "$file")

        # Check for exclusion
        skip_file=false
        for exclude_path in "${EXCLUDE_PATHS[@]}"; do
            if [[ "$absolute_file_path" == *"$exclude_path"* ]]; then
                skip_file=true
                break
            fi
        done

        if [ "$skip_file" = true ]; then
            continue
        fi

        # Check if it's a directory
        if [ -d "$file" ]; then
            echo "Entering directory: $file"
            process_files "$file"
        else
            # Check for FC_REFLECT
            if grep -q "FC_REFLECT" "$file"; then
                echo "Processing file: $file"

                # Increment count
                FC_REFLECT_COUNT=$((FC_REFLECT_COUNT + $(grep -o "FC_REFLECT" "$file" | wc -l)))

                # Output filename
                output_filename=$(echo "$file" | sed "s|$SOURCE_DIR/||" | sed 's/\//_/g')
                clang++ -E "$file" -o "$OUTPUT_DIR/$output_filename" -I$INCLUDE_DIRS
            fi
        fi
    done
}

# Start
process_files "$SOURCE_DIR"

# Display the count
echo "-----------------------------"
echo "Total 'FC_REFLECT' occurrences found: $FC_REFLECT_COUNT"

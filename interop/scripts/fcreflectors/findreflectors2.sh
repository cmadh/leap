#!/bin/bash

# Überprüfung der Anzahl der übergebenen Argumente
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <build_directory> <target_directory> <search_string>"
    exit 1
fi

# Konvertiere relative Pfade zu absoluten Pfaden
BUILD_DIR="$(realpath "$1")"
TARGET_DIR="$(realpath "$2")"
SEARCH_STRING="$3"

# Erstelle das Zielverzeichnis, falls es nicht existiert
mkdir -p "$TARGET_DIR"

# Suche nach Dateien im Build-Verzeichnis, die die Zeichenfolge enthalten
find "$BUILD_DIR" -type f -exec grep -l "$SEARCH_STRING" {} + | while IFS= read -r file; do
    # Kopiere die gefundenen Dateien in das Zielverzeichnis
    cp "$file" "$TARGET_DIR/"
done

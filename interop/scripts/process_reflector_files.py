# coding=utf-8
import os
import subprocess
import sys

# Verzeichnispfad als Argument uebernehmen
if len(sys.argv) < 2:
    print "Bitte geben Sie den Pfad zum Verzeichnis an."
    sys.exit(1)

directory_path = sys.argv[1]

# Ueberpruefen, ob das Verzeichnis existiert
if not os.path.exists(directory_path):
    print "Verzeichnis {0} wurde nicht gefunden.".format(directory_path)
    sys.exit(1)

# Alle Dateien im Verzeichnis auflisten
files = [f for f in os.listdir(directory_path) if os.path.isfile(os.path.join(directory_path, f))]

# Schleife durch die Dateiliste
for filename in files:
    source_file = os.path.join(directory_path, filename)
    output_file = "preprocessed_{0}.cpp".format(filename)

    # clang++-Befehl ausfuehren
    subprocess.call("clang++ -E {0} > {1}".format(source_file, output_file), shell=True)
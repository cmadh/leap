# coding=utf-8
import os
import re
import argparse

def find_files_with_extension(root_dir, extension, exclude_paths):
    matches = []
    for root, dirs, files in os.walk(root_dir):
        dirs[:] = [d for d in dirs if os.path.abspath(os.path.join(root, d)) not in exclude_paths]
        for file in files:
            if file.endswith(extension):
                matches.append(os.path.join(root, file))
    return matches

def extract_classes_or_structs_from_file(filename, root_dir):
    with open(filename, 'r') as f:
        content = f.read()

        matches = re.findall(r"FC_REFLECT\([^,]+\s*,", content)
        full_class_or_struct_names = [re.search(r"\(([^,]+)", match).group(1).strip() for match in matches]

        if not full_class_or_struct_names:
            return []

        classes_or_structs = []
        for full_name in full_class_or_struct_names:
            name = full_name.split("::")[-1]
            pattern = re.compile(r"(class|struct)\s+" + re.escape(name) + r"\s*(:\s*(public|private|protected)?\s+[\w:<>,\s]+)?\s*\{.*?\};", re.DOTALL)
            result = pattern.search(content)
            if result:
                classes_or_structs.append(result.group(0))
            else:
                relative_path = os.path.relpath(filename, root_dir)
                print("Could not find definition for {0} in file {1}".format(full_name, relative_path))

        return classes_or_structs

def main(root_dir, output_dir, exclude_paths):
    root_dir = os.path.abspath(root_dir)
    output_dir = os.path.abspath(output_dir)
    exclude_paths = [os.path.abspath(path) for path in exclude_paths]

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    cpp_files = find_files_with_extension(root_dir, '.cpp', exclude_paths)
    hpp_files = find_files_with_extension(root_dir, '.hpp', exclude_paths)

    all_files = cpp_files + hpp_files

    for filename in all_files:
        extracted_data = extract_classes_or_structs_from_file(filename, root_dir)
        if extracted_data:
            with open(os.path.join(output_dir, os.path.basename(filename)), 'w') as f:
                f.write("\n\n".join(extracted_data))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Extract C++ classes and structs.')
    parser.add_argument('root_dir', type=str, help='The root directory to search in.')
    parser.add_argument('output_dir', type=str, help='The directory where to save the extracted data.')
    parser.add_argument('--exclude_paths', nargs='*', default=[], help='Paths to exclude from the search.')

    args = parser.parse_args()
    main(args.root_dir, args.output_dir, args.exclude_paths)

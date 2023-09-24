configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/find_reflectors.py
        ${CMAKE_CURRENT_BINARY_DIR}/find_reflectors.py
        COPYONLY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/process_reflector_files.py
        ${CMAKE_CURRENT_BINARY_DIR}/process_reflector_files.py
        COPYONLY
)

set_property(SOURCE nodeos_swig.i swig_logger_base.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE nodeos_swig.i swig_logger_base.i PROPERTY COMPILE_OPTIONS
        -namespace ${DOTNET_PROJECT}.NodeosInterop
        -dllimport mizux-cmakeswig-native)

swig_add_library(dotnet_NodeosInterop
        TYPE OBJECT
        LANGUAGE csharp
        OUTPUT_DIR ${DOTNET_PROJECT_DIR}/NodeosInterop
        SOURCES nodeos_swig.i swig_logger_base.i)

set_target_properties(dotnet_NodeosInterop PROPERTIES
        SWIG_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR};${PROJECT_SOURCE_DIR}/nodeos;${PROJECT_SOURCE_DIR}/nodeos/dotnet"
        SWIG_USE_TARGET_INCLUDE_DIRECTORIES ON
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(dotnet_NodeosInterop PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/nodeos
        ${PROJECT_SOURCE_DIR}/nodeos/dotnet
        )

target_link_libraries(dotnet_NodeosInterop PRIVATE nodeos_interop eosio_chain)
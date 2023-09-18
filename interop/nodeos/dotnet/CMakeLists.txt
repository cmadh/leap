set_property(SOURCE nodeos_interop.i interop_logger_base.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE nodeos_interop.i interop_logger_base.i PROPERTY COMPILE_OPTIONS
        -namespace ${DOTNET_PROJECT}.NodeosInterop
        -dllimport io-liquiid-nodeos_interop-native)

message(STATUS "NODEOS_INTEROP_DOTNET_LIBRARY_NAME: ${NODEOS_INTEROP_DOTNET_LIBRARY_NAME}")
message(STATUS "DOTNET_PROJECT_DIR: ${DOTNET_PROJECT_DIR}")
swig_add_library(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME}
        TYPE OBJECT
        LANGUAGE csharp
        OUTPUT_DIR ${DOTNET_PROJECT_DIR}/${NODEOS_INTEROP_DOTNET_LIBRARY_NAME}
        SOURCES nodeos_interop.i interop_logger_base.i)

set_target_properties(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PROPERTIES
        SWIG_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR};${PROJECT_SOURCE_DIR}/nodeos;${PROJECT_SOURCE_DIR}/nodeos/dotnet"
        SWIG_USE_TARGET_INCLUDE_DIRECTORIES ON
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/nodeos
        ${PROJECT_SOURCE_DIR}/nodeos/dotnet
        )

target_link_libraries(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PRIVATE ${NODEOS_INTEROP_LIBRARY_NAME} eosio_chain)
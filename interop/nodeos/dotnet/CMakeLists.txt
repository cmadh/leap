set_property(SOURCE nodeos_interop.i interop_logger_base.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE nodeos_interop.i interop_logger_base.i PROPERTY COMPILE_OPTIONS
        -namespace ${DOTNET_PROJECT}.NodeosInterop
        -dllimport io-liquiid-nodeos_interop-native)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(Boost_USE_STATIC_LIBS OFF CACHE STRING "ON or OFF" )

message(STATUS "NODEOS_INTEROP_DOTNET_LIBRARY_NAME: ${NODEOS_INTEROP_DOTNET_LIBRARY_NAME}")
message(STATUS "DOTNET_PROJECT_DIR: ${DOTNET_PROJECT_DIR}")
swig_add_library(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME}
        TYPE OBJECT
        LANGUAGE csharp
        OUTPUT_DIR ${DOTNET_PROJECT_DIR}/${NODEOS_INTEROP_DOTNET_LIBRARY_NAME}
        SOURCES nodeos_interop.i)# interop_logger_base.i)

#target_compile_definitions(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PRIVATE BOOST_ALL_NO_LIB)

set_target_properties(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PROPERTIES
        SWIG_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR};${PROJECT_SOURCE_DIR}/nodeos;${PROJECT_SOURCE_DIR}/nodeos/dotnet"
        SWIG_USE_TARGET_INCLUDE_DIRECTORIES ON
        POSITION_INDEPENDENT_CODE ON)
#set_target_properties(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PROPERTIES LINK_FLAGS "-e main")

target_include_directories(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/nodeos
        ${PROJECT_SOURCE_DIR}/nodeos/dotnet
        )

target_link_libraries(${NODEOS_INTEROP_DOTNET_LIBRARY_NAME} PRIVATE ${NODEOS_INTEROP_LIBRARY_NAME} eosio_chain)

add_executable(${NODEOS_INTEROP_EXECUTABLE_NAME} ../nodeos_interop_executable.cpp)
target_link_libraries(${NODEOS_INTEROP_EXECUTABLE_NAME} stdc++ ${NODEOS_INTEROP_LIBRARY_NAME} eosio_chain_wrap)

add_custom_command(
        TARGET ${NODEOS_INTEROP_EXECUTABLE_NAME} POST_BUILD
        COMMAND chmod +x $<TARGET_FILE:${NODEOS_INTEROP_EXECUTABLE_NAME}>
        COMMENT "Set permission"
)
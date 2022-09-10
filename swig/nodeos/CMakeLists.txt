cmake_minimum_required(VERSION 3.16)

project(NodeosWrapperProj VERSION 1.0 LANGUAGES CXX)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})


# Set .Net project directory
set(NET_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dotNetSample")

# Add swig flags here
set(CMAKE_SWIG_FLAGS "")


set_property(SOURCE nodeos_swig.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE swig_logger_base.i PROPERTY CPLUSPLUS ON)
#set_source_files_properties(nodeos_swig.i PROPERTIES SWIG_FLAGS "-includeall")
#set_source_files_properties(deep_mind_swig_handler.i PROPERTIES SWIG_FLAGS "-includeall")

swig_add_library(${NODE_EXECUTABLE_NAME}_swig
        TYPE STATIC
        LANGUAGE CSharp
        SOURCES nodeos_swig.cpp nodeos_swig.i swig_logger_base.hpp swig_logger_base.i #nodeos_swig.cpp deep_mind_swig_handler.cpp #deep_mind_swig_handler.i deep_mind_swig_handler.cpp
        OUTPUT_DIR ${NET_PROJECT_DIR}
        OUTFILE_DIR ${NET_PROJECT_DIR}
        )

#set_property(TARGET ${NODE_EXECUTABLE_NAME}_swig PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE)

add_library(${NODE_EXECUTABLE_NAME}_lib
        STATIC
        nodeos_swig.cpp deep_mind_swig_handler.cpp
        )

set_target_properties( ${NODE_EXECUTABLE_NAME}_swig
        PROPERTIES
        # These copy animal.dll on Windows to .Net project directory
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${NET_PROJECT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${NET_PROJECT_DIR}

        # This copies animal.so on Linux to .Net project directory
        LIBRARY_OUTPUT_DIRECTORY ${NET_PROJECT_DIR}

        # Set address of C++ headers
        INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
        )

configure_file(config.hpp.in config.hpp ESCAPE_QUOTES)

target_include_directories(${NODE_EXECUTABLE_NAME}_swig PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${NODE_EXECUTABLE_NAME}_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
  if(APPLE)
    set(whole_archive_flag "-force_load")
    set(no_whole_archive_flag "")
    set(build_id_flag "")
  else()
    set(whole_archive_flag "--whole-archive")
    set(no_whole_archive_flag "--no-whole-archive")
    set(build_id_flag "--build-id")
  endif()
else()
  set(whole_archive_flag "--whole-archive")
  set(no_whole_archive_flag "--no-whole-archive")
  set(build_id_flag "")
endif()

swig_link_libraries( ${NODE_EXECUTABLE_NAME}_swig
        PRIVATE appbase version
        PRIVATE -Wl,${whole_archive_flag} login_plugin               -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} state_history_plugin       -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} trace_api_plugin           -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} chain_api_plugin           -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} net_plugin                 -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} net_api_plugin             -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} txn_test_gen_plugin        -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} db_size_api_plugin         -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} producer_api_plugin        -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} resource_monitor_plugin    -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} test_control_plugin        -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} test_control_api_plugin    -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${build_id_flag}
        PRIVATE chain_plugin http_plugin producer_plugin http_client_plugin eosio_chain
        PRIVATE eosio_chain_wrap fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )

target_link_libraries( ${NODE_EXECUTABLE_NAME}_lib
        PRIVATE appbase version
        PRIVATE -Wl,${whole_archive_flag} login_plugin               -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} state_history_plugin       -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} trace_api_plugin           -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} chain_api_plugin           -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} net_plugin                 -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} net_api_plugin             -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} txn_test_gen_plugin        -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} db_size_api_plugin         -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} producer_api_plugin        -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} resource_monitor_plugin    -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} test_control_plugin        -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} test_control_api_plugin    -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${build_id_flag}
        PRIVATE chain_plugin http_plugin producer_plugin http_client_plugin eosio_chain
        PRIVATE eosio_chain_wrap fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )

include(additionalPlugins)

#copy_bin( ${NODE_EXECUTABLE_NAME} )
#install( TARGETS
#   ${NODE_EXECUTABLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT base
#)
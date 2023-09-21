cmake_minimum_required(VERSION 3.0)
project(just-disable-vac VERSION 0.0.1)
set(compile_options -Ofast -s -fvisibility=hidden -fvisibility-inlines-hidden)
set(link_options "-Ofast -s -fvisibility=hidden -fvisibility-inlines-hidden -ldl")

add_library(just-disable-vac64 SHARED "")
target_compile_options(just-disable-vac64 PRIVATE "${compile_options}")
set_target_properties(just-disable-vac64 PROPERTIES LINK_FLAGS "${link_options}")
set_target_properties(just-disable-vac64 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin64)

add_library(just-disable-vac32 SHARED "")
target_compile_options(just-disable-vac32 PRIVATE -m32 ${compile_options})
set_target_properties(just-disable-vac32 PROPERTIES LINK_FLAGS "-m32 ${link_options}")
set_target_properties(just-disable-vac32 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin32)

target_include_directories(just-disable-vac64 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")
set_target_properties(just-disable-vac64 PROPERTIES OUTPUT_NAME "vpcfs")
set_target_properties(just-disable-vac64 PROPERTIES SUFFIX ".so.0")

target_include_directories(just-disable-vac32 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")
set_target_properties(just-disable-vac32 PROPERTIES OUTPUT_NAME "vpcfs")
set_target_properties(just-disable-vac32 PROPERTIES SUFFIX ".so.0")

set(files "${CMAKE_CURRENT_LIST_DIR}/src/hooks.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/src/fakedev.cpp")
target_sources(just-disable-vac64 PRIVATE ${files})
target_sources(just-disable-vac32 PRIVATE ${files})

add_custom_command(TARGET just-disable-vac64 POST_BUILD
        COMMAND chmod 755 "${CMAKE_BINARY_DIR}/bin64")
add_custom_command(TARGET just-disable-vac32 POST_BUILD
        COMMAND chmod 755 "${CMAKE_BINARY_DIR}/bin32")

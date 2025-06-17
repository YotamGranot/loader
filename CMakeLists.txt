cmake_minimum_required(VERSION 3.15)
project(loader C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(FATAL_ERROR "No build type specified. Please set CMAKE_BUILD_TYPE to Debug, Release, etc.")
endif()

set(LOADER_DEBUG_C_FLAGS "-g" "-g3")
set(LOADER_RELEASE_C_FLAGS "-Os")
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

set(LOADER_C_FLAGS 
  -Wall
  -Wextra
  -Werror
  -no-pie
  "$<$<CONFIG:Debug>:${LOADER_DEBUG_C_FLAGS}>"
  "$<$<CONFIG:Release>:${LOADER_RELEASE_C_FLAGS}>"
)

set(LOADER_SOURCES
  main.c
  src/elf/elf.c
  src/elf/elf.h
  src/elf/elf_internal.h
  src/loader/common.h
  src/loader/loader_status.h
)

set(LOADER_COMPILE_DEFINITIONS 
  "$<$<CONFIG:Debug>:_DEBUG>"
  "$<$<CONFIG:Release>:NDEBUG>"
)

add_executable(loader ${LOADER_SOURCES})

target_compile_options(loader PRIVATE ${LOADER_C_FLAGS})
target_compile_definitions(loader PRIVATE ${LOADER_COMPILE_DEFINITIONS})

target_include_directories(loader PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
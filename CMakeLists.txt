cmake_minimum_required(VERSION 3.16)
project(square_wave_transport
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD     99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release build if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

# Debug vs Release flags
set(CMAKE_C_FLAGS_DEBUG   "-g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

find_package(OpenMP REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})


add_library(fct STATIC
  flux_corr_method.c
)

target_link_libraries(fct PUBLIC
  OpenMP::OpenMP_C
  m        # link math library
)

add_executable(square_transport
  main.c
)
target_link_libraries(square_transport PRIVATE fct)



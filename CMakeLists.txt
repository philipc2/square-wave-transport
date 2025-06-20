cmake_minimum_required(VERSION 3.16)
project(square_wave_transport C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

find_package(OpenMP REQUIRED)

set(M_LIB m)

add_library(fct STATIC
  src/flux_corr_method.c
)
target_include_directories(fct PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(fct PUBLIC
  ${M_LIB}
  OpenMP::OpenMP_C
)

add_executable(square_transport
  src/main.c
)
target_link_libraries(square_transport PRIVATE
  fct
)

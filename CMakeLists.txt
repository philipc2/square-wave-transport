cmake_minimum_required(VERSION 3.16)
project(square_wave_transport C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-O2)

# ─── Compiler selection ────────────────────────────────────────────────────────
if(APPLE)
  # on macOS, prefer brew-installed gcc-14 if no compiler set
  if(NOT CMAKE_C_COMPILER)
    find_program(GCC14_EXECUTABLE gcc-14)
    if(GCC14_EXECUTABLE)
      set(CMAKE_C_COMPILER "${GCC14_EXECUTABLE}" CACHE FILEPATH "C compiler" FORCE)
      message(STATUS "Auto‐detected C compiler on macOS: ${GCC14_EXECUTABLE}")
    endif()
  endif()
else()
  # on Linux (or other), let CMake pick the default gcc/clang
  message(STATUS "Using system C compiler: ${CMAKE_C_COMPILER_ID}")
endif()

# Add common search paths (brew, /usr/local, /usr)
list(APPEND CMAKE_PREFIX_PATH
     "/opt/homebrew"     # Homebrew on Apple Silicon
     "/usr/local"        # Homebrew on Intel macOS / custom installs
     "/usr"              # typical Linux
)

# ─── Find NetCDF ──────────────────────────────────────────────────────────────
# 1) Try the CMake Package (config or module)
find_package(NetCDF QUIET)

if(TARGET NetCDF::NetCDF_C)
  set(NETCDF_LIB_TARGET NetCDF::NetCDF_C)
  message(STATUS "Using import target NetCDF::NetCDF_C")
elseif(TARGET NetCDF::NetCDF)
  set(NETCDF_LIB_TARGET NetCDF::NetCDF)
  message(STATUS "Using import target NetCDF::NetCDF")
else()
  # 2) On Linux, try pkg-config
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(NETCDF_PKG netcdf)
  endif()

  if(NETCDF_PKG_FOUND)
    include_directories(${NETCDF_PKG_INCLUDE_DIRS})
    link_directories(${NETCDF_PKG_LIBRARY_DIRS})
    set(NETCDF_LIB_TARGET ${NETCDF_PKG_LIBRARIES})
    message(STATUS "Found NetCDF via pkg-config: ${NETCDF_PKG_LIBRARIES}")
  else()
    # 3) Fallback to manual search
    message(STATUS "Falling back to manual find_path/find_library for NetCDF")
    find_path(NETCDF_C_INCLUDE_DIR netcdf.h
      PATHS /opt/homebrew/include /usr/local/include /usr/include
    )
    find_library(NETCDF_C_LIBRARY netcdf
      PATHS /opt/homebrew/lib /usr/local/lib /usr/lib
    )
    if(NOT NETCDF_C_INCLUDE_DIR OR NOT NETCDF_C_LIBRARY)
      message(FATAL_ERROR "Could not find netcdf.h or libnetcdf")
    endif()
    include_directories(${NETCDF_C_INCLUDE_DIR})
    set(NETCDF_LIB_TARGET ${NETCDF_C_LIBRARY})
    message(STATUS "Manually found NetCDF include: ${NETCDF_C_INCLUDE_DIR}")
    message(STATUS "Manually found NetCDF lib:     ${NETCDF_C_LIBRARY}")
  endif()
endif()

# ─── Build helper library (transport + I/O) ────────────────────────────────────
add_library(fct STATIC
  src/flux_corrected_transport.c
  src/first_order_transport.c
  src/lax_transport.c
  src/validation.c
  src/netcdf_output.c
)
target_include_directories(fct PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(fct PUBLIC
  ${NETCDF_LIB_TARGET}
  m
)

# ─── Main executable ──────────────────────────────────────────────────────────
add_executable(square_transport src/main.c)
target_link_libraries(square_transport PRIVATE
  fct
  ${NETCDF_LIB_TARGET}
  m
)

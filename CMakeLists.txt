# ======================================================================
#  larcoreobj main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larcoreobj/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(larcoreobj LANGUAGES CXX)

include(CetCMakeEnv)
set(${PROJECT_NAME}_CMAKE_PROJECT_VERSION_STRING 09.90.00-rc1)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

find_package(canvas_root_io REQUIRED EXPORT)
find_package(canvas REQUIRED EXPORT)
find_package(fhiclcpp REQUIRED EXPORT)
find_package(ROOT COMPONENTS GenVector REQUIRED EXPORT)

include(BuildDictionary)

# source
add_subdirectory(larcoreobj)

# tests
add_subdirectory(test)

# ups - table and config files

# packaging utility
cet_cmake_config()

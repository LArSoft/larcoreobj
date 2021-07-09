# ======================================================================
#  larcoreobj main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larcoreobj/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required (VERSION 3.12...3.18 FATAL_ERROR)

project(larcoreobj VERSION 9.24.01.01 LANGUAGES CXX)

find_package(cetmodules REQUIRED)

find_package(canvas_root_io)
find_package(art)

find_package(fhiclcpp)
# This should be added to specific targets, but for now...
link_libraries(fhiclcpp::fhiclcpp)
link_libraries(fhiclcpp::types)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(CetMake)
include(BasicPlugin)

# source
add_subdirectory(larcoreobj)

# tests
add_subdirectory(test)


if($ENV{CETMODULES_FQ_DIR})
add_subdirectory(ups)
endif()

add_subdirectory(cmakeConfig)

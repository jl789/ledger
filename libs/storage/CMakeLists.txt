################################################################################
# F E T C H   S T O R A G E   L I B R A R Y
################################################################################
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(fetch-storage)

# CMake Configuration
include(${FETCH_ROOT_CMAKE_DIR}/BuildTools.cmake)

# Compiler Configuration
setup_compiler()

#-------------------------------------------------------------------------------
# Main Library Target
#-------------------------------------------------------------------------------

setup_library(fetch-storage)
target_link_libraries(fetch-storage PUBLIC fetch-core fetch-crypto fetch-ledger fetch-testing vendor-mio)

#-------------------------------------------------------------------------------
# Example Targets
#-------------------------------------------------------------------------------

if(FETCH_ENABLE_EXAMPLES)

endif(FETCH_ENABLE_EXAMPLES)

add_test_target()
add_benchmark_target()

cmake_minimum_required(VERSION 3.15)
include (FetchContent)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(StreamingCCExperiments)

# Make the default build type Release. If user or another
# project sets a different value than use that
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to default -- Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE
      STRING "Choose the type of build." FORCE)
endif()
message(STATUS "Experiments Build Type ${CMAKE_BUILD_TYPE}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  message(STATUS "Adding GNU compiler flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  message(STATUS "Adding MSVC compiler flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
  message(STATUS "${CMAKE_CXX_COMPILER_ID} not recognized, no flags added")
endif()

# Install GraphStreamingCC Project
FetchContent_Declare(
  GraphStreamingCC
  GIT_REPOSITORY      "https://github.com/GraphStreamingProject/GraphStreamingCC"
  GIT_TAG             "cc_sketch_copy"
  UPDATE_DISCONNECTED OFF

  CMAKE_CACHE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
)

FetchContent_MakeAvailable(GraphStreamingCC)

###############################################################
#########################  Utilities  #########################
###############################################################
# clean gets top logs ready for processing by max_finder
add_executable(top_cleaner
  util/clean.cpp
)

# max_finder gives the maximum memory usage of a log 
# and returns RES/SWAP that gave this
add_executable(top_max_finder
  util/max_finder.cpp
)

###############################################################
########################  Experiments  ########################
###############################################################
# Test the speed and memory consumption of the algorithm
add_executable(speed_experiment
  util/configuration.cpp
  util/insertion_mgr.cpp
  speed_expr/speed_expr.cpp
)
add_dependencies(speed_experiment GraphStreamingCC)
target_link_libraries(speed_experiment PRIVATE GraphStreamingCC)

# Test how insertion rate scales with additional threads
add_executable(parallel_experiment
  util/configuration.cpp
  util/insertion_mgr.cpp
  parallel_expr/parallel_expr.cpp
)
add_dependencies(parallel_experiment GraphStreamingCC)
target_link_libraries(parallel_experiment PRIVATE GraphStreamingCC)

# Test the impact of buffer size upon performace
add_executable(buffersize_experiment
  util/configuration.cpp
  util/insertion_mgr.cpp
  parallel_expr/parallel_expr.cpp
)
add_dependencies(buffersize_experiment GraphStreamingCC)
target_link_libraries(buffersize_experiment PRIVATE GraphStreamingCC)

# Test correctness by running many insertions and many queries
add_executable(cont_test
  cont_expr/cont_expr.cpp)
add_dependencies(cont_test GraphStreamingCC)
target_link_libraries(cont_test PRIVATE GraphStreamingVerifyCC)

#########
# Three executables for the purpose of comparing the performance 
# of CubeSketch with state of the art l0-sketching
#########
# AGM sketching using 64 bit types
add_executable(agm_sketch_native
  sketch_expr/agm_sketch_expr.cpp
  sketch_expr/agm_sketching/src/bucket.cpp
  sketch_expr/agm_sketching/src/int127.cpp
  sketch_expr/agm_sketching/src/agm_sketch.cpp
  sketch_expr/agm_sketching/src/update.cpp
  sketch_expr/agm_sketching/src/util.cpp
)
target_link_libraries(agm_sketch_native PRIVATE xxhash)
target_compile_definitions(agm_sketch_native PRIVATE USE_NATIVE_F)

# AGM sketching using 128 bit types
add_executable(agm_sketch
  sketch_expr/agm_sketch_expr.cpp
  sketch_expr/agm_sketching/src/bucket.cpp
  sketch_expr/agm_sketching/src/int127.cpp
  sketch_expr/agm_sketching/src/agm_sketch.cpp
  sketch_expr/agm_sketching/src/update.cpp
  sketch_expr/agm_sketching/src/util.cpp
)
target_link_libraries(agm_sketch PRIVATE xxhash)

# Sketch_experiment on CubeSketch
add_executable(cube_sketch
  sketch_expr/cube_sketch_expr.cpp
)
add_dependencies(cube_sketch GraphStreamingCC)
target_link_libraries(cube_sketch PRIVATE GraphStreamingCC)

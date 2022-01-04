cmake_minimum_required(VERSION 3.15)
include (ExternalProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(StreamingCCExperiments)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  message("Adding GNU compiler flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  message("Adding MSVC compiler flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
  message("${CMAKE_CXX_COMPILER_ID} not recognized, no flags added")
endif()

message("${CMAKE_MODULE_PATH}")
include("DownloadProject.cmake")

#Find or download xxHash
find_package(xxHash 0.7 CONFIG)
if(NOT xxHash_FOUND)
  if(NOT AUTODOWNLOAD)
    message(FATAL_ERROR "xxHash config not found.\n"
      "Set xxHash_DIR to a directory containing one of \n"
      "  xxHashConfig.cmake\n"
      "  xxhash-config.cmake\n"
      "or use -DAUTODOWNLOAD:BOOL=ON to automatically download")
  else()
    #Download xxHash
    download_project(
      PROJ xxHash
      GIT_REPOSITORY "https://github.com/Cyan4973/xxHash.git"
      GIT_TAG "v0.8.0"
    )

    #xxHash messes with BUILD_SHARED_LIBS if it is empty
    set(SAVED_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")

    add_subdirectory(
      "${xxHash_SOURCE_DIR}/cmake_unofficial"
      "${xxHash_BINARY_DIR}"
      EXCLUDE_FROM_ALL
    )

    #Restore BUILD_SHARED_LIBS
    set(BUILD_SHARED_LIBS "${SAVED_BUILD_SHARED_LIBS}" CACHE BOOL "" FORCE)

    #Add aliases to targets
    add_library(xxHash::xxhash ALIAS xxhash)
  endif()
endif()

#Find or download GTest
find_package(GTest CONFIG)
if(NOT GTest_FOUND)
  if(NOT AUTODOWNLOAD)
    message(FATAL_ERROR "GTest config not found.\n"
      "Set GTest_DIR to a directory containing one of \n"
      "  GTestConfig.cmake\n"
      "  gtest-config.cmake\n"
      "or use -DAUTODOWNLOAD:BOOL=ON to automatically download")
  else()
    #Download googletest
    download_project(
      PROJ googletest
      GIT_REPOSITORY "https://github.com/google/googletest.git"
      GIT_TAG "release-1.10.0"
    )

    #As specified in the googletest docs, need to set
    #gtest_force_shared_crt for windows visual studio.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    add_subdirectory(
      "${googletest_SOURCE_DIR}"
      "${googletest_BINARY_DIR}"
      EXCLUDE_FROM_ALL
    )

    #Add aliases to targets
    add_library(GTest::gtest ALIAS gtest)
    add_library(GTest::gtest_main ALIAS gtest_main)
    add_library(GTest::gmock ALIAS gmock)
    add_library(GTest::gmock_main ALIAS gmock_main)
  endif()
endif()

# Install GraphStreamingCC Project
ExternalProject_Add(
  GraphStreamingCC

  PREFIX          GraphStreamingCC
  GIT_REPOSITORY  "https://github.com/GraphStreamingProject/GraphStreamingCC"
  GIT_TAG         "time_hook"

  UPDATE_DISCONNECTED false
  BUILD_ALWAYS    OFF
  INSTALL_DIR     ${CMAKE_CURRENT_BINARY_DIR}/GraphStreamingCC/prefix

  CMAKE_CACHE_ARGS
        -DAUTODOWNLOAD:BOOL=ON
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>

  BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/GraphStreamingCC/prefix/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/GraphStreamingCC/prefix/include/GutterTree)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/GraphStreamingCC/prefix/include/test)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/GraphStreamingCC/prefix/include/GraphZeppelinCommon)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/GraphStreamingCC/prefix/lib)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/GraphStreamingCC/src/GraphStreamingCC-build/GutterTree/prefix/lib)

###############################################################
#########################  Utilities  #########################
###############################################################
# clean gets top logs ready for processing by max_finder
add_executable(top_cleaner
  util/clean.cpp)

# max_finder gives the maximum memory usage of a log 
# and returns RES/SWAP that gave this
add_executable(top_max_finder
  util/max_finder.cpp)

###############################################################
########################  Experiments  ########################
###############################################################

# Test the speed and memory consumption of the algorithm
add_executable(speed_experiment
  util/configuration.cpp
  util/insertion_mgr.cpp
  speed_expr/speed_expr.cpp)
add_dependencies(speed_experiment GraphStreamingCC)
target_link_libraries(speed_experiment PRIVATE libGraphStreamingCC.a xxHash::xxhash GTest::gtest libGutterTree.a)
#optimize unless DEBUG
# optimize unless debug
if (DEFINED ENV{DEBUG})
  message("Disabling optimizations and enabling debug symbols")
  target_compile_options(speed_experiment PRIVATE -g -fopenmp)
  target_link_options(speed_experiment PRIVATE -fopenmp)
else ()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(speed_experiment PRIVATE -O3 -fopenmp)
    target_link_options(speed_experiment PRIVATE -fopenmp)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(speed_experiment PRIVATE /O2)
  endif()
endif()

# Test how insertion rate scales with additional threads
add_executable(parallel_experiment
  util/configuration.cpp
  util/insertion_mgr.cpp
  parallel_expr/parallel_expr.cpp)
add_dependencies(parallel_experiment GraphStreamingCC)
target_link_libraries(parallel_experiment PRIVATE libGraphStreamingCC.a xxHash::xxhash GTest::gtest libGutterTree.a)
# optimize unless debug
if (DEFINED ENV{DEBUG})
  message("Disabling optimizations and enabling debug symbols")
  target_compile_options(parallel_experiment PRIVATE -g -fopenmp)
  target_link_options(parallel_experiment PRIVATE -fopenmp)
else ()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(parallel_experiment PRIVATE -O3 -fopenmp)
    target_link_options(parallel_experiment PRIVATE -fopenmp)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(parallel_experiment PRIVATE /O2)
  endif()
endif()

# Test the impact of buffer size upon performace
add_executable(buffersize_experiment
  util/configuration.cpp
  util/insertion_mgr.cpp
  parallel_expr/parallel_expr.cpp)
add_dependencies(buffersize_experiment GraphStreamingCC)
target_link_libraries(buffersize_experiment PRIVATE libGraphStreamingCC.a xxHash::xxhash GTest::gtest libGutterTree.a)
# optimize unless debug
if (DEFINED ENV{DEBUG})
  message("Disabling optimizations and enabling debug symbols")
  target_compile_options(buffersize_experiment PRIVATE -g -fopenmp)
  target_link_options(buffersize_experiment PRIVATE -fopenmp)
else ()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(buffersize_experiment PRIVATE -O3 -fopenmp)
    target_link_options(buffersize_experiment PRIVATE -fopenmp)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(buffersize_experiment PRIVATE /O2)
  endif()
endif()

# Test correctness by running many insertions and many queries
add_executable(cont_test
  cont_expr/cont_expr.cpp)
add_dependencies(cont_test GraphStreamingCC)
target_link_libraries(cont_test PRIVATE libGraphStreamingCC.a xxHash::xxhash GTest::gtest libGutterTree.a)
# target_compile_definitions(cont_test PRIVATE VERIFY_SAMPLES_F)
# optimize unless debug
if (DEFINED ENV{DEBUG})
  message("Disabling optimizations and enabling debug symbols")
  target_compile_options(cont_test PRIVATE -g -fopenmp)
  target_link_options(cont_test PRIVATE -fopenmp)
else ()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(cont_test PRIVATE -O3 -fopenmp)
    target_link_options(cont_test PRIVATE -fopenmp)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(cont_test PRIVATE /O2)
  endif()
endif()

# Compare the performance of CubeSketch with state of the art l0-sketching
#add_executable(sketch_experiment)

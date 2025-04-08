cmake_minimum_required(VERSION 3.0.0)
project(acc_sim)

set(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall")

include_directories (
    inc
    ./3rd/googletest/googletest/include
    ./3rd/DRAMsim3-master/ext/headers
)

link_directories (
    ./3rd/googletest/lib
)

# trace CPU, .etc
aux_source_directory (src/core SRCS)
aux_source_directory (src/memory SRCS)
aux_source_directory (src/noc SRCS)
add_library (src_lib STATIC ${SRCS} src/Initialization.cpp src/top.cpp)

# message(WARNING "ddd ${SRCS}")

# build dramsim3 library
add_library(format INTERFACE)
target_include_directories(format INTERFACE ./3rd/DRAMsim3-master/ext/fmt/include)
target_compile_definitions(format INTERFACE FMT_HEADER_ONLY=1)
# Main DRAMSim cpp
add_library(dramsim3_lib SHARED
    ./3rd/DRAMsim3-master/src/bankstate.cc
    ./3rd/DRAMsim3-master/src/channel_state.cc
    ./3rd/DRAMsim3-master/src/command_queue.cc
    ./3rd/DRAMsim3-master/src/common.cc
    ./3rd/DRAMsim3-master/src/configuration.cc
    ./3rd/DRAMsim3-master/src/controller.cc
    ./3rd/DRAMsim3-master/src/dram_system.cc
    ./3rd/DRAMsim3-master/src/hmc.cc
    ./3rd/DRAMsim3-master/src/refresh.cc
    ./3rd/DRAMsim3-master/src/simple_stats.cc
    ./3rd/DRAMsim3-master/src/timing.cc
    ./3rd/DRAMsim3-master/src/memory_system.cc
    ./3rd/DRAMsim3-master/src/6_HBM_Interface.cpp
)
target_link_libraries(dramsim3_lib PRIVATE format)

# build acc simulator
add_executable(acc_sim src/main.cpp)
target_link_libraries (acc_sim src_lib dramsim3_lib)

# build test
aux_source_directory (test TESTS)

foreach (v ${TESTS})
    string (REGEX MATCH "test/.*" relative_path ${v})
    string (REGEX REPLACE "test/" "" target_name ${relative_path})
    string (REGEX REPLACE ".cpp" "" target_name ${target_name})
    add_executable (test_${target_name} test/${target_name}.cpp)
    target_link_libraries (test_${target_name} gtest pthread src_lib dramsim3_lib)
endforeach()
# set(target_name "10_Vertex_RAM_Controller_Test")
# add_executable (test_${target_name} test/${target_name}.cpp)
# target_link_libraries (test_${target_name} gtest pthread src_lib dramsim3_lib)
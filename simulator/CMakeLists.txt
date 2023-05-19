
cmake_minimum_required(VERSION 3.0.0)
project(dramsim3)

set (CMAKE_BUILD_TYPE "Debug")
# set(default_build_type "Release")


add_library(inih INTERFACE)
target_include_directories(inih INTERFACE ./3rd/DRAMsim3-master/ext/headers)

add_library(format INTERFACE)
target_include_directories(format INTERFACE ./3rd/DRAMsim3-master/ext/fmt/include)
target_compile_definitions(format INTERFACE FMT_HEADER_ONLY=1)

# argparsing library, only used in main program not the library
add_library(args INTERFACE)
target_include_directories(args INTERFACE ./3rd/DRAMsim3-master/ext/headers)

add_library(json INTERFACE)
target_include_directories(json INTERFACE ./3rd/DRAMsim3-master/ext/headers)

# Main DRAMSim Lib
add_library(dramsim3 SHARED
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
)


target_include_directories(dramsim3 INTERFACE ./3rd/DRAMsim3-master/src)
target_compile_options(dramsim3 PRIVATE -Wall)
target_link_libraries(dramsim3 PRIVATE inih format)
set_target_properties(dramsim3 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# trace CPU, .etc
add_executable(dramsim3main ./src/main.cpp ./src/Initialization.cpp ./src/Network/noc.cpp ./src/Network/Hyperx_Network_96_channels.cpp
               ./src/Mem/3_Offset_Uram.cpp ./src/Mem/mem.cpp ./3rd/DRAMsim3-master/src/6_HBM_Interface.cpp ./src/Compute_Core/2_RD_Active_Vertex_Offset_container.cpp
               ./src/Compute_Core/5_Combine_Edge_Rqst.cpp ./src/Compute_Core/core_pipe.cpp ./src/Compute_Core/7_Schedule.cpp
               ./src/Compute_Core/9_Backend_Core_Process_container.cpp ./src/Compute_Core/10_Vertex_BRAM.cpp
                ./src/Compute_Core/5_2_Rd_First_Edge_Bram.cpp ./src/Compute_Core/1_RD_Active_Vertex.cpp
               ./src/Compute_Core/4_RD_Active_Vertex_Edge.cpp ./src/Compute_Core/11.Apply_Iteration_End.cpp
               ./src/Global/global_pipe.cpp ./src/Global/global.cpp)
target_include_directories(dramsim3main PRIVATE ./inc)
target_link_libraries(dramsim3main PRIVATE dramsim3 args)
target_compile_options(dramsim3main PRIVATE)
set_target_properties(dramsim3main PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

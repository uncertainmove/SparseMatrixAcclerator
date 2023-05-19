# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo1)
link_directories(../3rd/googletest/lib)
set(CMAKE_CXX_FLAGS "-L../inc -L../3rd/googletest/lib -lgmock_main -lgmock -lgtest_main -lgtest -lpthread")
# 指定生成目标 g++ 2_RD_Active_Vertex_Offset_Test.cpp ../src/Compute_Core/2_RD_Active_Vertex_Offset_container.cpp -I../3rd/googletest/googletest/include -L../inc -L../3rd/googletest/lib -lgmock_main -lgmock -lgtest_main -lgtest -lpthread  -o test
add_executable(Demo 2_RD_Active_Vertex_Offset_Test.cpp ../src/Compute_Core/2_RD_Active_Vertex_Offset_container.cpp)

target_include_directories( Demo PUBLIC ../3rd/googletest/googletest/include )

# target_link_libraries( Demo /home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/3rd/googletest/lib/libgmock_main.a
#                             /home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/3rd/googletest/lib/libgmock.a
#                             /home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/3rd/googletest/lib/libgtest_main.a
#                             /home/sunsss/acc/SparseMatrixAccelerator_now_use/SparseMatrixAcclerator/simulator/3rd/googletest/lib/libgtest.a)


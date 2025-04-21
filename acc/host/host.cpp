// This is a generated file. Use and modify at your own risk.
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
Vendor: Xilinx
Associated Filename: main.c
#Purpose: This example shows a basic vector add +1 (constant) by manipulating
#         memory inplace.
*******************************************************************************/
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <bits/stdc++.h>
#include <fstream>
#ifdef _WINDOWS
#include <io.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/opencl.h>
#include <CL/cl_ext.h>
#include "xclhal2.h"

////////////////////////////////////////////////////////////////////////////////

#define NUM_WORKGROUPS (1)
#define WORKGROUP_SIZE (256)
#define MAX_LENGTH 8192
#define MEM_ALIGNMENT 4096
#if defined(VITIS_PLATFORM) && !defined(TARGET_DEVICE)
#define STR_VALUE(arg)      #arg
#define GET_STRING(name) STR_VALUE(name)
#define TARGET_DEVICE GET_STRING(VITIS_PLATFORM)
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////

cl_uint load_file_to_memory(const char *filename, char **result)
{
    cl_uint size = 0;
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        *result = NULL;
        return -1; // -1 means file opening fail
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *result = (char *)malloc(size+1);
    if (size != fread(*result, sizeof(char), size, f)) {
        free(*result);
        return -2; // -2 means file reading fail
    }
    fclose(f);
    (*result)[size] = 0;
    return size;
}

int main(int argc, char** argv)
{

    cl_int err;                            // error code returned from api calls
    cl_uint check_status = 0;

    cl_platform_id platform_id;         // platform id
    cl_device_id device_id;             // compute device id
    cl_context context;                 // compute context
    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute programs
    cl_kernel kernel;                   // compute kernel


    cl_uint *h_data_off, *h_data_edge_0, *h_data_edge_1;                                // host memory for input vector
    char cl_platform_vendor[1001];
    char target_device_name[1001] = TARGET_DEVICE;

    // ./exer xclbin off.txt edge_0.txt edge_1.txt debug_res.txt res.txt vertex_num edge_num iteration_num
    if (argc != 11) {
        printf("Usage: %s xclbin off.txt edge_0.txt edge_1.txt debug_res.txt res.txt vertex_num edge_0_num edge_1_num iteration_num\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *off_file = argv[2];
    cout << "offset file: " << off_file << endl;
    char *edge_0_file = argv[3];
    cout << "edge_0 file: " << edge_0_file << endl;
    char *edge_1_file = argv[4];
    cout << "edge_1 file: " << edge_1_file << endl;
    char *debug_res_file = argv[5];
    cout << "debug res file: " << debug_res_file << endl;
    char *res_file = argv[6];
    cout << "res file: " << res_file << endl;
    int vertex_num = atoi(argv[7]);
    cout << "vertex num: " << vertex_num << endl;
    int edge_0_num = atoi(argv[8]);
    cout << "edge 0 num: " << edge_0_num << endl;
    int edge_1_num = atoi(argv[9]);
    cout << "edge 1 num: " << edge_1_num << endl;
    int iteration_num = atoi(argv[10]);
    cout << "iteration num: " << iteration_num << endl;

    cl_uint* h_axi00_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT, vertex_num * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi00_ptr0;                         // device memory used for a vector
    cl_mem d_axi01_ptr0;                         // device memory used for a vector
    cl_mem d_axi02_ptr0;                         // device memory used for a vector
    cl_uint* h_axi03_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT, vertex_num * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi03_ptr0;                         // device memory used for a vector
    cl_mem d_axi04_ptr0;                         // device memory used for a vector
    cl_mem d_axi05_ptr0;                         // device memory used for a vector
    cl_mem d_axi06_ptr0;                         // device memory used for a vector
    cl_mem d_axi07_ptr0;                         // device memory used for a vector
    cl_mem d_axi08_ptr0;                         // device memory used for a vector
    cl_mem d_axi09_ptr0;                         // device memory used for a vector

    // Fill our data sets with pattern
    h_data_off = (cl_uint*)aligned_alloc(MEM_ALIGNMENT, (vertex_num * 2) * sizeof(cl_uint*));
    h_data_edge_0 = (cl_uint*)aligned_alloc(MEM_ALIGNMENT, edge_0_num * sizeof(cl_uint*));
    h_data_edge_1 = (cl_uint*)aligned_alloc(MEM_ALIGNMENT, (edge_1_num + 1) * sizeof(cl_uint*));

    cout << "Start Load Graph Offset" << endl;
    ifstream in_off;
    in_off.open(off_file);
    if (!in_off) {
      cout << "[ERROR] Failed to open " << off_file << endl;
      exit(-1);
    }
    for(cl_uint i = 0; i < vertex_num * 2; i++) {
        in_off >> h_data_off[i];
    }
    in_off.close();
    cout << "Complete Load Graph Offset" << endl;
    cout << "Start Load Graph Edge 0" << endl;
    ifstream in_edge_0;
    in_edge_0.open(edge_0_file);
    if (!in_edge_0) {
      cout << "[ERROR] Failed to open " << edge_0_file << endl;
      exit(-1);
    }
    for(cl_uint i = 0; i < edge_0_num; i++) {
        in_edge_0 >> h_data_edge_0[i];
    }
    in_edge_0.close();
    cout << "Complete Load Graph Edge 0" << endl;
    cout << "Start Load Graph Edge 1" << endl;
    ifstream in_edge_1;
    in_edge_1.open(edge_1_file);
    if (!in_edge_1) {
      cout << "[ERROR] Failed to open " << edge_1_file << endl;
      exit(-1);
    }
    for(cl_uint i = 0; i < edge_1_num; i++) {
        in_edge_1 >> h_data_edge_1[i];
    }
    in_edge_1.close();
    cout << "Complete Load Graph Edge 1" << endl;

    // Get all platforms and then select Xilinx platform
    cl_platform_id platforms[16];       // platform id
    cl_uint platform_count;
    cl_uint platform_found = 0;
    err = clGetPlatformIDs(16, platforms, &platform_count);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to find an OpenCL platform!\n");
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    printf("INFO: Found %d platforms\n", platform_count);

    // Find Xilinx Plaftorm
    for (cl_uint iplat=0; iplat<platform_count; iplat++) {
        err = clGetPlatformInfo(platforms[iplat], CL_PLATFORM_VENDOR, 1000, (void *)cl_platform_vendor,NULL);
        if (err != CL_SUCCESS) {
            printf("ERROR: clGetPlatformInfo(CL_PLATFORM_VENDOR) failed!\n");
            printf("ERROR: Test failed\n");
            return EXIT_FAILURE;
        }
        if (strcmp(cl_platform_vendor, "Xilinx") == 0) {
            printf("INFO: Selected platform %d from %s\n", iplat, cl_platform_vendor);
            platform_id = platforms[iplat];
            platform_found = 1;
        }
    }
    if (!platform_found) {
        printf("ERROR: Platform Xilinx not found. Exit.\n");
        return EXIT_FAILURE;
    }

    // Get Accelerator compute device
    cl_uint num_devices;
    cl_uint device_found = 0;
    cl_device_id devices[16];  // compute device id
    char cl_device_name[1001];
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, 16, devices, &num_devices);
    printf("INFO: Found %d devices\n", num_devices);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to create a device group!\n");
        printf("ERROR: Test failed\n");
        return -1;
    }

    //iterate all devices to select the target device.
    for (cl_uint i=0; i<num_devices; i++) {
        err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 1024, cl_device_name, 0);
        if (err != CL_SUCCESS) {
            printf("ERROR: Failed to get device name for device %d!\n", i);
            printf("ERROR: Test failed\n");
            return EXIT_FAILURE;
        }
        printf("CL_DEVICE_NAME %s\n", cl_device_name);
        if(strcmp(cl_device_name, target_device_name) == 0 || 1) {
            device_id = devices[i];
            device_found = 1;
            printf("Selected %s as the target device\n", cl_device_name);
        }
    }

    if (!device_found) {
        printf("ERROR:Target device %s not found. Exit.\n", target_device_name);
        return EXIT_FAILURE;
    }

    // Create a compute context
    //
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (!context) {
        printf("ERROR: Failed to create a compute context!\n");
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }

    // Create a command commands
    commands = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
    if (!commands) {
        printf("ERROR: Failed to create a command commands!\n");
        printf("ERROR: code %i\n",err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }

    cl_int status;

    // Create Program Objects
    // Load binary from disk
    unsigned char *kernelbinary;
    char *xclbin = argv[1];

    //------------------------------------------------------------------------------
    // xclbin
    //------------------------------------------------------------------------------
    printf("INFO: loading xclbin %s\n", xclbin);
    cl_uint n_i0 = load_file_to_memory(xclbin, (char **) &kernelbinary);
    if (n_i0 < 0) {
        printf("ERROR: failed to load kernel from xclbin: %s\n", xclbin);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }

    size_t n0 = n_i0;

    // Create the compute program from offline
    program = clCreateProgramWithBinary(context, 1, &device_id, &n0,
                                        (const unsigned char **) &kernelbinary, &status, &err);
    free(kernelbinary);

    if ((!program) || (err!=CL_SUCCESS)) {
        printf("ERROR: Failed to create compute program from binary %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    // Build the program executable
    //
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        printf("ERROR: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }

    // Create the compute kernel in the program we wish to run
    //
    kernel = clCreateKernel(program, "delta_pr_accelerator", &err);
    if (!kernel || err != CL_SUCCESS) {
        printf("ERROR: Failed to create compute kernel!\n");
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }

    // Create structs to define memory bank mapping
    cl_mem_ext_ptr_t mem_ext;
    mem_ext.obj = NULL;
    mem_ext.param = kernel;


    mem_ext.flags = 3;
    d_axi00_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * (vertex_num * 2), &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 4;
    d_axi01_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * edge_0_num,  &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 5;
    d_axi02_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * (edge_1_num + 1), &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 6;
    d_axi03_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * vertex_num, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 7;
    d_axi04_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * edge_0_num, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 8;
    d_axi05_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * (edge_1_num + 1), &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 9;
    d_axi06_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * edge_0_num, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 10;
    d_axi07_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * (edge_1_num + 1), &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 11;
    d_axi08_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * edge_0_num, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    mem_ext.flags = 12;
    d_axi09_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * (edge_1_num + 1), &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }
    if (!(d_axi00_ptr0&&d_axi01_ptr0&&d_axi02_ptr0&&d_axi03_ptr0&&d_axi04_ptr0&&d_axi05_ptr0&&d_axi06_ptr0&&d_axi07_ptr0&&d_axi08_ptr0&&d_axi09_ptr0)) {
        printf("ERROR: Failed to allocate device memory!\n");
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    cout << "Complete Create Buffer" << endl;
    err = clEnqueueWriteBuffer(commands, d_axi00_ptr0, CL_TRUE, 0, sizeof(cl_uint) * (vertex_num * 2), h_data_off, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi00_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi01_ptr0, CL_TRUE, 0, sizeof(cl_uint) * edge_0_num, h_data_edge_0, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi01_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi02_ptr0, CL_TRUE, 0, sizeof(cl_uint) * (edge_1_num + 1), h_data_edge_1, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi02_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi03_ptr0, CL_TRUE, 0, sizeof(cl_uint) * vertex_num, h_data_off, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi03_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi04_ptr0, CL_TRUE, 0, sizeof(cl_uint) * edge_0_num, h_data_edge_0, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi04_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi05_ptr0, CL_TRUE, 0, sizeof(cl_uint) * (edge_1_num + 1), h_data_edge_1, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi05_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi06_ptr0, CL_TRUE, 0, sizeof(cl_uint) * edge_0_num, h_data_edge_0, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi06_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi07_ptr0, CL_TRUE, 0, sizeof(cl_uint) * (edge_1_num + 1), h_data_edge_1, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi07_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi08_ptr0, CL_TRUE, 0, sizeof(cl_uint) * edge_0_num, h_data_edge_0, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi08_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    err = clEnqueueWriteBuffer(commands, d_axi09_ptr0, CL_TRUE, 0, sizeof(cl_uint) * (edge_1_num + 1), h_data_edge_1, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi09_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    cout << "Complete Write Buffer" << endl;

    // Set the arguments to our compute kernel
    // cl_uint vector_length = MAX_LENGTH;
    err = 0;
    cl_uint d_iteration_num = iteration_num;
    err |= clSetKernelArg(kernel, 0, sizeof(cl_uint), &d_iteration_num); // Not used in example RTL logic.
    cl_uint d_vertex_num = vertex_num;
    err |= clSetKernelArg(kernel, 1, sizeof(cl_uint), &d_vertex_num); // Not used in example RTL logic.
    cl_uint d_edge_num = edge_0_num + edge_1_num;
    err |= clSetKernelArg(kernel, 2, sizeof(cl_uint), &d_edge_num); // Not used in example RTL logic.
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &d_axi00_ptr0); 
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &d_axi01_ptr0); 
    err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &d_axi02_ptr0); 
    err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &d_axi03_ptr0); 
    err |= clSetKernelArg(kernel, 7, sizeof(cl_mem), &d_axi04_ptr0); 
    err |= clSetKernelArg(kernel, 8, sizeof(cl_mem), &d_axi05_ptr0); 
    err |= clSetKernelArg(kernel, 9, sizeof(cl_mem), &d_axi06_ptr0); 
    err |= clSetKernelArg(kernel, 10, sizeof(cl_mem), &d_axi07_ptr0); 
    err |= clSetKernelArg(kernel, 11, sizeof(cl_mem), &d_axi08_ptr0); 
    err |= clSetKernelArg(kernel, 12, sizeof(cl_mem), &d_axi09_ptr0); 

    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to set kernel arguments! %d\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    cout << "Complete Set Args" << endl;

    size_t global[1];
    size_t local[1];
    // Execute the kernel over the entire range of our 1d input data set
    // using the maximum number of work group items for this device

    global[0] = 1;
    local[0] = 1;
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, (size_t*)&global, (size_t*)&local, 0, NULL, NULL);
    if (err) {
        printf("ERROR: Failed to execute kernel! %d\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    cout << "Call Execute Kernel" << endl;

    clFinish(commands);
    cout << "Finish Execute Kernel" << endl;


    // Read back the results from the device to verify the output
    //
    cl_event readevent;

    err = 0;
    err |= clEnqueueReadBuffer( commands, d_axi00_ptr0, CL_TRUE, 0, sizeof(cl_uint) * 1, h_axi00_ptr0_output, 0, NULL, &readevent );
    // err |= clEnqueueReadBuffer( commands, d_axi01_ptr0, CL_TRUE, 0, sizeof(cl_uint) * vertex_num, h_axi01_ptr0_output, 0, NULL, &readevent );
    // err |= clEnqueueReadBuffer( commands, d_axi02_ptr0, CL_TRUE, 0, sizeof(cl_uint) * vertex_num, h_axi02_ptr0_output, 0, NULL, &readevent );
    err |= clEnqueueReadBuffer( commands, d_axi03_ptr0, CL_TRUE, 0, sizeof(cl_uint) * vertex_num, h_axi03_ptr0_output, 0, NULL, &readevent );

    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to read output array! %d\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    clWaitForEvents(1, &readevent);

    cout << "Total Cycle   : " << h_axi00_ptr0_output[0] << endl;
    float frequency = 126;
    float performance = 1.0 * d_edge_num * d_iteration_num * frequency / h_axi00_ptr0_output[0] / 1000;
    cout << "FPGA Frequency: " << frequency << "MHz" << endl;
    cout << "Vertex Num    : " << d_vertex_num << endl;
    cout << "Edge Num      : " << d_edge_num << endl;
    cout << "Iteration Num : " << d_iteration_num << endl;
    cout << "PR Performace : " << performance << "GTEPs" << endl;
    cout << "Start Store Graph Result" << endl;
    ofstream out_res;
    out_res.open(res_file);
    if (!out_res) {
      cout << "[ERROR] Failed to open " << res_file << endl;
      exit(-1);
    }
    for (int i = 0; i < vertex_num; i++) {
      out_res << i << " " << hex << h_axi03_ptr0_output[i] << endl;
    }
    out_res.close();
    cout << "Complete Store Graph Result" << endl;
    cout << "Start Check Graph Result" << endl;
    FILE *in_debug_res = fopen(debug_res_file, "r");
    if (!in_debug_res) {
      cout << "[ERROR] Failed to open " << debug_res_file << endl;
      exit(-1);
    }
    int debug_res_id, debug_res_data;
    int error_counter = 0;
    check_status = 0;
    for (int i = 0; i < vertex_num; i++) {
      fscanf(in_debug_res, "%d %x", &debug_res_id, &debug_res_data);
      // cout << "id: " << debug_res_id << ", data: " << debug_res_data << endl;
      float ret_pr_v = *(float *)&h_axi03_ptr0_output[i];
      float debug_pr_v = *(float *)&debug_res_data;
      float divation = ret_pr_v - debug_pr_v > 0 ? ret_pr_v - debug_pr_v : debug_pr_v - ret_pr_v;
      float delta = debug_pr_v - ret_pr_v;
      if (divation / debug_pr_v < 0 || divation / debug_pr_v > 0.005) {
        error_counter++;
        check_status = 1;
        printf("id: %d, debug_res: %f, acc_res: %f, delta: %x, err: %f.\n", i, debug_pr_v, *(int *)&delta, ret_pr_v, divation / debug_pr_v);
      }
      if (error_counter > 20) {
        cout << "Too many errors found. Exiting check of result." << endl;
        break;
      }
    }

    // Check Results
    //--------------------------------------------------------------------------
    // Shutdown and cleanup
    //-------------------------------------------------------------------------- 
    clReleaseMemObject(d_axi00_ptr0);
    free(h_axi00_ptr0_output);

    clReleaseMemObject(d_axi01_ptr0);

    clReleaseMemObject(d_axi02_ptr0);

    clReleaseMemObject(d_axi03_ptr0);
    free(h_axi03_ptr0_output);

    free(h_data_off);
    free(h_data_edge_0);
    free(h_data_edge_1);
    clReleaseProgram(program);

     clReleaseKernel(kernel); 

    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    if (check_status) {
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    } else {
        printf("INFO: Test completed successfully.\n");
        return EXIT_SUCCESS;
    }


} // end of main

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
    const cl_uint number_of_words = 4096; // 16KB of data


    cl_platform_id platform_id;         // platform id
    cl_device_id device_id;             // compute device id
    cl_context context;                 // compute context
    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute programs
    cl_kernel kernel;                   // compute kernel


    cl_uint* h_data;                                // host memory for input vector
    char cl_platform_vendor[1001];
    char target_device_name[1001] = TARGET_DEVICE;

    cl_uint* h_axi00_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi00_ptr0;                         // device memory used for a vector

    cl_uint* h_axi01_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi01_ptr0;                         // device memory used for a vector

    cl_uint* h_axi02_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi02_ptr0;                         // device memory used for a vector

    cl_uint* h_axi03_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi03_ptr0;                         // device memory used for a vector

    cl_uint* h_axi04_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi04_ptr0;                         // device memory used for a vector

    cl_uint* h_axi05_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi05_ptr0;                         // device memory used for a vector

    cl_uint* h_axi06_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi06_ptr0;                         // device memory used for a vector

    cl_uint* h_axi07_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi07_ptr0;                         // device memory used for a vector

    cl_uint* h_axi08_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi08_ptr0;                         // device memory used for a vector

    cl_uint* h_axi09_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi09_ptr0;                         // device memory used for a vector

    cl_uint* h_axi10_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi10_ptr0;                         // device memory used for a vector

    cl_uint* h_axi11_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi11_ptr0;                         // device memory used for a vector

    cl_uint* h_axi12_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi12_ptr0;                         // device memory used for a vector

    cl_uint* h_axi13_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi13_ptr0;                         // device memory used for a vector

    cl_uint* h_axi14_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi14_ptr0;                         // device memory used for a vector

    cl_uint* h_axi15_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi15_ptr0;                         // device memory used for a vector

    cl_uint* h_axi16_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi16_ptr0;                         // device memory used for a vector

    cl_uint* h_axi17_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi17_ptr0;                         // device memory used for a vector

    cl_uint* h_axi18_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi18_ptr0;                         // device memory used for a vector

    cl_uint* h_axi19_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi19_ptr0;                         // device memory used for a vector

    cl_uint* h_axi20_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi20_ptr0;                         // device memory used for a vector

    cl_uint* h_axi21_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi21_ptr0;                         // device memory used for a vector

    cl_uint* h_axi22_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi22_ptr0;                         // device memory used for a vector

    cl_uint* h_axi23_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi23_ptr0;                         // device memory used for a vector

    cl_uint* h_axi24_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi24_ptr0;                         // device memory used for a vector

    cl_uint* h_axi25_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi25_ptr0;                         // device memory used for a vector

    cl_uint* h_axi26_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi26_ptr0;                         // device memory used for a vector

    cl_uint* h_axi27_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi27_ptr0;                         // device memory used for a vector

    cl_uint* h_axi28_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi28_ptr0;                         // device memory used for a vector

    cl_uint* h_axi29_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi29_ptr0;                         // device memory used for a vector

    cl_uint* h_axi30_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi30_ptr0;                         // device memory used for a vector

    cl_uint* h_axi31_ptr0_output = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*)); // host memory for output vector
    cl_mem d_axi31_ptr0;                         // device memory used for a vector

    if (argc != 2) {
        printf("Usage: %s xclbin\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Fill our data sets with pattern
    h_data = (cl_uint*)aligned_alloc(MEM_ALIGNMENT,MAX_LENGTH * sizeof(cl_uint*));
    for(cl_uint i = 0; i < MAX_LENGTH; i++) {
        h_data[i]  = i;
        h_axi00_ptr0_output[i] = 0; 
        h_axi01_ptr0_output[i] = 0; 
        h_axi02_ptr0_output[i] = 0; 
        h_axi03_ptr0_output[i] = 0; 
        h_axi04_ptr0_output[i] = 0; 
        h_axi05_ptr0_output[i] = 0; 
        h_axi06_ptr0_output[i] = 0; 
        h_axi07_ptr0_output[i] = 0; 
        h_axi08_ptr0_output[i] = 0; 
        h_axi09_ptr0_output[i] = 0; 
        h_axi10_ptr0_output[i] = 0; 
        h_axi11_ptr0_output[i] = 0; 
        h_axi12_ptr0_output[i] = 0; 
        h_axi13_ptr0_output[i] = 0; 
        h_axi14_ptr0_output[i] = 0; 
        h_axi15_ptr0_output[i] = 0; 
        h_axi16_ptr0_output[i] = 0; 
        h_axi17_ptr0_output[i] = 0; 
        h_axi18_ptr0_output[i] = 0; 
        h_axi19_ptr0_output[i] = 0; 
        h_axi20_ptr0_output[i] = 0; 
        h_axi21_ptr0_output[i] = 0; 
        h_axi22_ptr0_output[i] = 0; 
        h_axi23_ptr0_output[i] = 0; 
        h_axi24_ptr0_output[i] = 0; 
        h_axi25_ptr0_output[i] = 0; 
        h_axi26_ptr0_output[i] = 0; 
        h_axi27_ptr0_output[i] = 0; 
        h_axi28_ptr0_output[i] = 0; 
        h_axi29_ptr0_output[i] = 0; 
        h_axi30_ptr0_output[i] = 0; 
        h_axi31_ptr0_output[i] = 0; 

    }

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
        if(strcmp(cl_device_name, target_device_name) == 0) {
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
    kernel = clCreateKernel(program, "acc_kernel", &err);
    if (!kernel || err != CL_SUCCESS) {
        printf("ERROR: Failed to create compute kernel!\n");
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }

    // Create structs to define memory bank mapping
    cl_mem_ext_ptr_t mem_ext;
    mem_ext.obj = NULL;
    mem_ext.param = kernel;


    mem_ext.flags = 1;
    d_axi00_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 2;
    d_axi01_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 3;
    d_axi02_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 4;
    d_axi03_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 5;
    d_axi04_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 6;
    d_axi05_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 7;
    d_axi06_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 8;
    d_axi07_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 9;
    d_axi08_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 10;
    d_axi09_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 11;
    d_axi10_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 12;
    d_axi11_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 13;
    d_axi12_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 14;
    d_axi13_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 15;
    d_axi14_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 16;
    d_axi15_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 17;
    d_axi16_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 18;
    d_axi17_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 19;
    d_axi18_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 20;
    d_axi19_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 21;
    d_axi20_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 22;
    d_axi21_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 23;
    d_axi22_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 24;
    d_axi23_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 25;
    d_axi24_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 26;
    d_axi25_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 27;
    d_axi26_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 28;
    d_axi27_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 29;
    d_axi28_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 30;
    d_axi29_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 31;
    d_axi30_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    mem_ext.flags = 32;
    d_axi31_ptr0 = clCreateBuffer(context,  CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,  sizeof(cl_uint) * number_of_words, &mem_ext, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Return code for clCreateBuffer flags=" << mem_ext.flags << ": " << err << std::endl;
    }


    if (!(d_axi00_ptr0&&d_axi01_ptr0&&d_axi02_ptr0&&d_axi03_ptr0&&d_axi04_ptr0&&d_axi05_ptr0&&d_axi06_ptr0&&d_axi07_ptr0&&d_axi08_ptr0&&d_axi09_ptr0&&d_axi10_ptr0&&d_axi11_ptr0&&d_axi12_ptr0&&d_axi13_ptr0&&d_axi14_ptr0&&d_axi15_ptr0&&d_axi16_ptr0&&d_axi17_ptr0&&d_axi18_ptr0&&d_axi19_ptr0&&d_axi20_ptr0&&d_axi21_ptr0&&d_axi22_ptr0&&d_axi23_ptr0&&d_axi24_ptr0&&d_axi25_ptr0&&d_axi26_ptr0&&d_axi27_ptr0&&d_axi28_ptr0&&d_axi29_ptr0&&d_axi30_ptr0&&d_axi31_ptr0)) {
        printf("ERROR: Failed to allocate device memory!\n");
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi00_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi00_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi01_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi01_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi02_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi02_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi03_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi03_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi04_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi04_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi05_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi05_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi06_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi06_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi07_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi07_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi08_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi08_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi09_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi09_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi10_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi10_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi11_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi11_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi12_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi12_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi13_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi13_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi14_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi14_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi15_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi15_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi16_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi16_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi17_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi17_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi18_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi18_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi19_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi19_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi20_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi20_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi21_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi21_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi22_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi22_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi23_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi23_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi24_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi24_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi25_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi25_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi26_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi26_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi27_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi27_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi28_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi28_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi29_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi29_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi30_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi30_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueWriteBuffer(commands, d_axi31_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to write to source array h_data: d_axi31_ptr0: %d!\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }


    // Set the arguments to our compute kernel
    // cl_uint vector_length = MAX_LENGTH;
    err = 0;
    cl_uint d_root_id = 0;
    err |= clSetKernelArg(kernel, 0, sizeof(cl_uint), &d_root_id); // Not used in example RTL logic.
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_axi00_ptr0); 
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_axi01_ptr0); 
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &d_axi02_ptr0); 
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &d_axi03_ptr0); 
    err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &d_axi04_ptr0); 
    err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &d_axi05_ptr0); 
    err |= clSetKernelArg(kernel, 7, sizeof(cl_mem), &d_axi06_ptr0); 
    err |= clSetKernelArg(kernel, 8, sizeof(cl_mem), &d_axi07_ptr0); 
    err |= clSetKernelArg(kernel, 9, sizeof(cl_mem), &d_axi08_ptr0); 
    err |= clSetKernelArg(kernel, 10, sizeof(cl_mem), &d_axi09_ptr0); 
    err |= clSetKernelArg(kernel, 11, sizeof(cl_mem), &d_axi10_ptr0); 
    err |= clSetKernelArg(kernel, 12, sizeof(cl_mem), &d_axi11_ptr0); 
    err |= clSetKernelArg(kernel, 13, sizeof(cl_mem), &d_axi12_ptr0); 
    err |= clSetKernelArg(kernel, 14, sizeof(cl_mem), &d_axi13_ptr0); 
    err |= clSetKernelArg(kernel, 15, sizeof(cl_mem), &d_axi14_ptr0); 
    err |= clSetKernelArg(kernel, 16, sizeof(cl_mem), &d_axi15_ptr0); 
    err |= clSetKernelArg(kernel, 17, sizeof(cl_mem), &d_axi16_ptr0); 
    err |= clSetKernelArg(kernel, 18, sizeof(cl_mem), &d_axi17_ptr0); 
    err |= clSetKernelArg(kernel, 19, sizeof(cl_mem), &d_axi18_ptr0); 
    err |= clSetKernelArg(kernel, 20, sizeof(cl_mem), &d_axi19_ptr0); 
    err |= clSetKernelArg(kernel, 21, sizeof(cl_mem), &d_axi20_ptr0); 
    err |= clSetKernelArg(kernel, 22, sizeof(cl_mem), &d_axi21_ptr0); 
    err |= clSetKernelArg(kernel, 23, sizeof(cl_mem), &d_axi22_ptr0); 
    err |= clSetKernelArg(kernel, 24, sizeof(cl_mem), &d_axi23_ptr0); 
    err |= clSetKernelArg(kernel, 25, sizeof(cl_mem), &d_axi24_ptr0); 
    err |= clSetKernelArg(kernel, 26, sizeof(cl_mem), &d_axi25_ptr0); 
    err |= clSetKernelArg(kernel, 27, sizeof(cl_mem), &d_axi26_ptr0); 
    err |= clSetKernelArg(kernel, 28, sizeof(cl_mem), &d_axi27_ptr0); 
    err |= clSetKernelArg(kernel, 29, sizeof(cl_mem), &d_axi28_ptr0); 
    err |= clSetKernelArg(kernel, 30, sizeof(cl_mem), &d_axi29_ptr0); 
    err |= clSetKernelArg(kernel, 31, sizeof(cl_mem), &d_axi30_ptr0); 
    err |= clSetKernelArg(kernel, 32, sizeof(cl_mem), &d_axi31_ptr0); 

    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to set kernel arguments! %d\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }

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

    clFinish(commands);


    // Read back the results from the device to verify the output
    //
    cl_event readevent;

    err = 0;
    err |= clEnqueueReadBuffer( commands, d_axi00_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi00_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi01_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi01_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi02_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi02_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi03_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi03_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi04_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi04_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi05_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi05_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi06_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi06_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi07_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi07_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi08_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi08_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi09_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi09_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi10_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi10_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi11_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi11_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi12_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi12_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi13_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi13_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi14_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi14_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi15_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi15_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi16_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi16_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi17_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi17_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi18_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi18_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi19_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi19_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi20_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi20_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi21_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi21_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi22_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi22_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi23_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi23_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi24_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi24_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi25_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi25_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi26_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi26_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi27_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi27_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi28_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi28_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi29_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi29_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi30_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi30_ptr0_output, 0, NULL, &readevent );

    err |= clEnqueueReadBuffer( commands, d_axi31_ptr0, CL_TRUE, 0, sizeof(cl_uint) * number_of_words, h_axi31_ptr0_output, 0, NULL, &readevent );


    if (err != CL_SUCCESS) {
        printf("ERROR: Failed to read output array! %d\n", err);
        printf("ERROR: Test failed\n");
        return EXIT_FAILURE;
    }
    clWaitForEvents(1, &readevent);
    // Check Results

    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi00_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m00_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi00_ptr0_output[i], h_axi00_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi00_ptr0_input[i], h_axi00_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi01_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m01_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi01_ptr0_output[i], h_axi01_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi01_ptr0_input[i], h_axi01_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi02_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m02_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi02_ptr0_output[i], h_axi02_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi02_ptr0_input[i], h_axi02_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi03_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m03_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi03_ptr0_output[i], h_axi03_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi03_ptr0_input[i], h_axi03_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi04_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m04_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi04_ptr0_output[i], h_axi04_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi04_ptr0_input[i], h_axi04_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi05_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m05_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi05_ptr0_output[i], h_axi05_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi05_ptr0_input[i], h_axi05_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi06_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m06_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi06_ptr0_output[i], h_axi06_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi06_ptr0_input[i], h_axi06_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi07_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m07_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi07_ptr0_output[i], h_axi07_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi07_ptr0_input[i], h_axi07_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi08_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m08_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi08_ptr0_output[i], h_axi08_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi08_ptr0_input[i], h_axi08_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi09_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m09_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi09_ptr0_output[i], h_axi09_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi09_ptr0_input[i], h_axi09_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi10_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m10_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi10_ptr0_output[i], h_axi10_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi10_ptr0_input[i], h_axi10_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi11_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m11_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi11_ptr0_output[i], h_axi11_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi11_ptr0_input[i], h_axi11_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi12_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m12_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi12_ptr0_output[i], h_axi12_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi12_ptr0_input[i], h_axi12_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi13_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m13_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi13_ptr0_output[i], h_axi13_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi13_ptr0_input[i], h_axi13_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi14_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m14_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi14_ptr0_output[i], h_axi14_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi14_ptr0_input[i], h_axi14_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi15_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m15_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi15_ptr0_output[i], h_axi15_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi15_ptr0_input[i], h_axi15_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi16_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m16_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi16_ptr0_output[i], h_axi16_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi16_ptr0_input[i], h_axi16_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi17_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m17_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi17_ptr0_output[i], h_axi17_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi17_ptr0_input[i], h_axi17_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi18_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m18_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi18_ptr0_output[i], h_axi18_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi18_ptr0_input[i], h_axi18_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi19_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m19_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi19_ptr0_output[i], h_axi19_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi19_ptr0_input[i], h_axi19_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi20_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m20_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi20_ptr0_output[i], h_axi20_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi20_ptr0_input[i], h_axi20_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi21_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m21_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi21_ptr0_output[i], h_axi21_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi21_ptr0_input[i], h_axi21_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi22_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m22_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi22_ptr0_output[i], h_axi22_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi22_ptr0_input[i], h_axi22_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi23_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m23_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi23_ptr0_output[i], h_axi23_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi23_ptr0_input[i], h_axi23_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi24_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m24_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi24_ptr0_output[i], h_axi24_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi24_ptr0_input[i], h_axi24_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi25_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m25_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi25_ptr0_output[i], h_axi25_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi25_ptr0_input[i], h_axi25_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi26_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m26_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi26_ptr0_output[i], h_axi26_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi26_ptr0_input[i], h_axi26_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi27_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m27_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi27_ptr0_output[i], h_axi27_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi27_ptr0_input[i], h_axi27_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi28_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m28_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi28_ptr0_output[i], h_axi28_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi28_ptr0_input[i], h_axi28_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi29_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m29_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi29_ptr0_output[i], h_axi29_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi29_ptr0_input[i], h_axi29_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi30_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m30_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi30_ptr0_output[i], h_axi30_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi30_ptr0_input[i], h_axi30_ptr0_output[i]);
    }


    for (cl_uint i = 0; i < number_of_words; i++) {
        if ((h_data[i] + 1) != h_axi31_ptr0_output[i]) {
            printf("ERROR in acc_kernel::m31_axi - array index %d (host addr 0x%03x) - input=%d (0x%x), output=%d (0x%x)\n", i, i*4, h_data[i], h_data[i], h_axi31_ptr0_output[i], h_axi31_ptr0_output[i]);
            check_status = 1;
        }
      //  printf("i=%d, input=%d, output=%d\n", i,  h_axi31_ptr0_input[i], h_axi31_ptr0_output[i]);
    }


    //--------------------------------------------------------------------------
    // Shutdown and cleanup
    //-------------------------------------------------------------------------- 
    clReleaseMemObject(d_axi00_ptr0);
    free(h_axi00_ptr0_output);

    clReleaseMemObject(d_axi01_ptr0);
    free(h_axi01_ptr0_output);

    clReleaseMemObject(d_axi02_ptr0);
    free(h_axi02_ptr0_output);

    clReleaseMemObject(d_axi03_ptr0);
    free(h_axi03_ptr0_output);

    clReleaseMemObject(d_axi04_ptr0);
    free(h_axi04_ptr0_output);

    clReleaseMemObject(d_axi05_ptr0);
    free(h_axi05_ptr0_output);

    clReleaseMemObject(d_axi06_ptr0);
    free(h_axi06_ptr0_output);

    clReleaseMemObject(d_axi07_ptr0);
    free(h_axi07_ptr0_output);

    clReleaseMemObject(d_axi08_ptr0);
    free(h_axi08_ptr0_output);

    clReleaseMemObject(d_axi09_ptr0);
    free(h_axi09_ptr0_output);

    clReleaseMemObject(d_axi10_ptr0);
    free(h_axi10_ptr0_output);

    clReleaseMemObject(d_axi11_ptr0);
    free(h_axi11_ptr0_output);

    clReleaseMemObject(d_axi12_ptr0);
    free(h_axi12_ptr0_output);

    clReleaseMemObject(d_axi13_ptr0);
    free(h_axi13_ptr0_output);

    clReleaseMemObject(d_axi14_ptr0);
    free(h_axi14_ptr0_output);

    clReleaseMemObject(d_axi15_ptr0);
    free(h_axi15_ptr0_output);

    clReleaseMemObject(d_axi16_ptr0);
    free(h_axi16_ptr0_output);

    clReleaseMemObject(d_axi17_ptr0);
    free(h_axi17_ptr0_output);

    clReleaseMemObject(d_axi18_ptr0);
    free(h_axi18_ptr0_output);

    clReleaseMemObject(d_axi19_ptr0);
    free(h_axi19_ptr0_output);

    clReleaseMemObject(d_axi20_ptr0);
    free(h_axi20_ptr0_output);

    clReleaseMemObject(d_axi21_ptr0);
    free(h_axi21_ptr0_output);

    clReleaseMemObject(d_axi22_ptr0);
    free(h_axi22_ptr0_output);

    clReleaseMemObject(d_axi23_ptr0);
    free(h_axi23_ptr0_output);

    clReleaseMemObject(d_axi24_ptr0);
    free(h_axi24_ptr0_output);

    clReleaseMemObject(d_axi25_ptr0);
    free(h_axi25_ptr0_output);

    clReleaseMemObject(d_axi26_ptr0);
    free(h_axi26_ptr0_output);

    clReleaseMemObject(d_axi27_ptr0);
    free(h_axi27_ptr0_output);

    clReleaseMemObject(d_axi28_ptr0);
    free(h_axi28_ptr0_output);

    clReleaseMemObject(d_axi29_ptr0);
    free(h_axi29_ptr0_output);

    clReleaseMemObject(d_axi30_ptr0);
    free(h_axi30_ptr0_output);

    clReleaseMemObject(d_axi31_ptr0);
    free(h_axi31_ptr0_output);



    free(h_data);
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

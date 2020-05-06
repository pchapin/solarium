
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

const char *programSource =
    "__kernel\n"
    "void vecadd(__global int *A,\n"
    "            __global int *B,\n"
    "            __global int *C)\n"
    "{\n"
    "    int idx = get_global_id( 0 );\n"
    "    C[idx] = A[idx] + B[idx];\n"
    "}\n";


int main( )
{
    // This code executes on the OpenCL host.

    const int    elements        = 2048;  // Elements in each array.
          cl_uint platform_count = 0;     // How many OpenCL platforms are avaiable.

    // Compute the size of the data.
    size_t datasize = sizeof(int) * elements;

    // Allocate space forthe input/outout host data.
    int *A = (int *)malloc( datasize );
    int *B = (int *)malloc( datasize );
    int *C = (int *)malloc( datasize );

    // Initialize the input data.
    for( int i = 0; i < elements; ++i ) {
        A[i] = i;
        B[i] = i;
    }

    // Use this to check the output of each API call.
    cl_int status;

    // Get the first platform.
    cl_platform_id platform;
    status = clGetPlatformIDs( 1, &platform, &platform_count );
    printf( "%d OpenCL platforms available.\n", (int)platform_count );
    if( status != CL_SUCCESS ) fprintf( stderr, "Error getting platform IDs!\n" );

    // Get the first device.
    cl_device_id device;
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL );
    if( status != CL_SUCCESS ) fprintf( stderr, "Error getting device IDs!\n" );

    // Create a context and associate it with the device.
    cl_context context = clCreateContext( NULL, 1, &device, NULL, NULL, &status );
    if( status != CL_SUCCESS ) fprintf( stderr, "Error creating context!\n" );

    // Create a command queue and associate it with the device.
    cl_command_queue cmdQueue = clCreateCommandQueue( context, device, 0, &status );
    if( status != CL_SUCCESS ) fprintf( stderr, "Error creating the command queue!\n" );

    // Allocate two input buffers and one output buffer.
    cl_mem bufA = clCreateBuffer( context, CL_MEM_READ_ONLY,  datasize, NULL, &status );
    cl_mem bufB = clCreateBuffer( context, CL_MEM_READ_ONLY,  datasize, NULL, &status );
    cl_mem bufC = clCreateBuffer( context, CL_MEM_WRITE_ONLY, datasize, NULL, &status );

    // Transfer data from host arrays into the input buffers.
    status = clEnqueueWriteBuffer( cmdQueue, bufA, CL_FALSE, 0, datasize, A, 0, NULL, NULL );
    status = clEnqueueWriteBuffer( cmdQueue, bufB, CL_FALSE, 0, datasize, B, 0, NULL, NULL );

    // Create a program with source code.
    cl_program program =
        clCreateProgramWithSource( context, 1, (const char **)&programSource, NULL, &status );
    if( status != CL_SUCCESS ) fprintf( stderr, "Error creating program object!\n" );

    // Build the program for the device.
    status = clBuildProgram( program, 1, &device, NULL, NULL, NULL );
    if( status != CL_SUCCESS ) {
        switch( status ) {
        case CL_COMPILER_NOT_AVAILABLE:
            fprintf( stderr, "Error building program! Compiler not available.\n" );
            break;
        case CL_BUILD_PROGRAM_FAILURE:
            fprintf( stderr, "Error building program! Build failure.\n" );
            break;
        default:
            fprintf( stderr, "Error building program! *unknown reason*\n" );
            break;
        }
    }

    // Create the vector addition kernel.
    cl_kernel kernel = clCreateKernel( program, "vecadd", &status );
    if( status != CL_SUCCESS ) fprintf( stderr, "Error creating kernel!\n" );

    // Set the kernel arguments.
    status = clSetKernelArg( kernel, 0, sizeof(cl_mem), &bufA );
    status = clSetKernelArg( kernel, 1, sizeof(cl_mem), &bufB );
    status = clSetKernelArg( kernel, 2, sizeof(cl_mem), &bufC );

    // Define an index space of work items for execution.
    // A work group size is not required, but can be used.
    size_t indexSpaceSize[1], workGroupSize[1];
    indexSpaceSize[0] = elements;
    workGroupSize[0] = 256;

    // Execute the kernel.
    status =
        clEnqueueNDRangeKernel(
          cmdQueue, kernel, 1, NULL, indexSpaceSize, workGroupSize, 0, NULL, NULL);
    if( status != CL_SUCCESS ) fprintf( stderr, "Error queuing kernel execution!\n" );

    // Read the device output buffer to the host output arrays.
    status = clEnqueueReadBuffer( cmdQueue, bufC, CL_TRUE, 0, datasize, C, 0, NULL, NULL );

    for( int i = 0; i < elements; ++i ) {
        printf( "C[%4d] = %d\n", i, C[i] );
    }

    // Free OpenCL resources.
    clReleaseKernel( kernel );
    clReleaseProgram( program );
    clReleaseCommandQueue( cmdQueue );
    clReleaseMemObject( bufA );
    clReleaseMemObject( bufB );
    clReleaseMemObject( bufC );
    clReleaseContext( context );

    // Free host resources.
    free( A );
    free( B );
    free( C );

    return 0;
}

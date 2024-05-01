/*
 * This program uses OpenCL to add two vectors of integers of length 2048 elements. It is an
 * edited version of the sample program in "Hetergeneous Computing with OpenCL 2.0" by Kaeli,
 * Mistry, Schaa, and Zhang (copyright 2015, published by Morgan Kaufmann,
 * ISBN=978-0-12-801414-1), Section 3.6.
 */

#include <stdio.h>
#include <stdlib.h>

#define CL_TARGET_OPENCL_VERSION 220  // We want version 2.2 (for now).
#include <CL/cl.h>

// Arbitrary limits defined by this program. We'll ignore platforms/devices beyond these limits.
#define MAX_PLATFORMS            4
#define MAX_DEVICES_PER_PLATFORM 8

const char *program_source =
    "__kernel\n"
    "void vecadd(__global int *A,\n"
    "            __global int *B,\n"
    "            __global int *C)\n"
    "{\n"
    "    int index = get_global_id( 0 );\n"
    "    C[index] = A[index] + B[index];\n"
    "}\n";


int main( void )
{
    // This code executes on the OpenCL host.

    const int     elements       = 2048;  // Elements in each array.
          cl_uint platform_count = 0;     // How many OpenCL platforms are avaiable?
          cl_uint device_count   = 0;     // How many devices on the one (and only) platform of interest?
          int     return_code    = EXIT_SUCCESS;

    // Compute the size of the data.
    size_t datasize = sizeof(int) * elements;

    // Allocate space for the input/outout host data. Notice that we assume these allocations
    // succeed (which is resonable for small sizes).
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

    // =====

    // How many platforms?
    cl_platform_id platforms[MAX_PLATFORMS];
    status = clGetPlatformIDs( MAX_PLATFORMS, platforms, &platform_count );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error getting platform IDs!\n" );
        return_code = EXIT_FAILURE;
        goto release_host_memory;
    }
    printf( "OpenCL: %d platform%s found.\n", (int)platform_count, (platform_count == 1) ? "" : "s" );
    if( platform_count == 0 ) {
        fprintf( stderr, "*** At least one platform required!\n" );
        goto release_host_memory;
    }

    // =====

    // In this sample program, we only try to use one platform. In a more aggressive program,
    // multiple platforms might be used if available.
    //
    // How many devices on the first platform?
    cl_device_id devices[MAX_DEVICES_PER_PLATFORM];
    status = clGetDeviceIDs( platforms[0], CL_DEVICE_TYPE_ALL, MAX_DEVICES_PER_PLATFORM, devices, &device_count );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error getting device IDs!\n" );
        return_code = EXIT_FAILURE;
        goto release_host_memory;
    }
    printf( "OpenCL: %d device%s found on the first platform.\n", (int)device_count, (device_count == 1) ? "" : "s" );
    if( device_count == 0 ) {
        fprintf( stderr, "*** At least one device required!\n" );
        goto release_host_memory;
    }

    // =====

    // In this sample program, we will only try to use one device. In a more aggressive
    // program, multiple devices might be used if available.
    //
    // First define a list of property values that includes which platform we want to use.
    cl_context_properties properties[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0],
        0
    };
    // Now create a context for the first device on the platform of interest. We don't use a callback function.
    cl_context context = clCreateContext( properties, 1, &devices[0], NULL, NULL, &status );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error creating the context!\n" );
        return_code = EXIT_FAILURE;
        goto release_host_memory;
    }
    printf( "OpenCL: context created successfully for the first device on the first platform.\n" );

    // =====

    // Create a command queue and associate it with the device.
    cl_command_queue cmdQueue = clCreateCommandQueueWithProperties( context, devices[0], NULL, &status );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error creating the command queue!\n" );
        return_code = EXIT_FAILURE;
        goto release_context;
    }
    printf( "OpenCL: command queue created successfully for the device.\n" );

    // =====

    // Allocate two input buffers and one output buffer (on the device associated with the given
    // context). For brevity, we assume these allocations all succeed, and we ignore the
    // returned status. In a more taxing environment, it is possible the allocations will fail
    // because the device(s) may have insufficient memory.
    //
    cl_mem bufA = clCreateBuffer( context, CL_MEM_READ_ONLY,  datasize, NULL, &status );
    cl_mem bufB = clCreateBuffer( context, CL_MEM_READ_ONLY,  datasize, NULL, &status );
    cl_mem bufC = clCreateBuffer( context, CL_MEM_WRITE_ONLY, datasize, NULL, &status );
    printf( "OpenCL: buffers created successfully on the device.\n" );

    // =====

    // Next, enqueue commands to copy data from host memory to device memory. The device will
    // process queued commands asychronously (and possibly with other options depending on how
    // the queue was created). Notice that these operations are non-blocking (because of
    // CL_FALSE), which means they may return before the memory pointed at by `A` and `B` has
    // been fully transferred. For brevity, we assume these operations both succeed, and we
    // ignore the error status.
    status = clEnqueueWriteBuffer( cmdQueue, bufA, CL_FALSE, 0, datasize, A, 0, NULL, NULL );
    status = clEnqueueWriteBuffer( cmdQueue, bufB, CL_FALSE, 0, datasize, B, 0, NULL, NULL );
    printf( "OpenCL: queued commands to transfer data to the device successfully.\n" );

    // =====

    // Establish the program using source code.
    cl_program program = clCreateProgramWithSource( context, 1, &program_source, NULL, &status );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error creating program object!\n" );
        return_code = EXIT_FAILURE;
        goto release_buffers;
    }
    printf( "OpenCL: device program successfully loaded into context.\n" );

    // =====

    // Build the program for the device.
    status = clBuildProgram( program, 1, &devices[0], NULL, NULL, NULL );
    if( status != CL_SUCCESS ) {
        switch( status ) {
        case CL_COMPILER_NOT_AVAILABLE:
            fprintf( stderr, "*** Error building program! Compiler not available.\n" );
            break;
        case CL_BUILD_PROGRAM_FAILURE:
            fprintf( stderr, "*** Error building program! Build failure.\n" );
            break;
        default:
            fprintf( stderr, "*** Error building program! *unknown reason*\n" );
            break;
        }
        goto release_buffers;
    }
    printf( "OpenCL: device program successfully compiled.\n" );

    // =====

    // Create the vector addition kernel from the compiled device program.
    cl_kernel kernel = clCreateKernel( program, "vecadd", &status );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error creating kernel!\n" );
        goto release_program;
    }
    printf( "OpenCL: device kernel successfully created from compiled device program.\n" );

    // =====

    // Set the kernel arguments. We assume these steps succeed.
    status = clSetKernelArg( kernel, 0, sizeof(cl_mem), &bufA );
    status = clSetKernelArg( kernel, 1, sizeof(cl_mem), &bufB );
    status = clSetKernelArg( kernel, 2, sizeof(cl_mem), &bufC );
    printf( "OpenCL: device kernel arguments successfully set.\n" );

    // =====

    // Define an index space of work items for execution. A work group size is not required, but
    // can be used.
    size_t indexSpaceSize[1], workGroupSize[1];
    indexSpaceSize[0] = elements;
    workGroupSize[0]  = 256;

    // Queue a command to execute the kernel on the device.
    status = clEnqueueNDRangeKernel( cmdQueue, kernel, 1, NULL, indexSpaceSize, workGroupSize, 0, NULL, NULL);
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error queuing kernel execution!\n" );
        goto release_kernel;
    }
    printf( "OpenCL: queued the kernel execution command successfully.\n" );

    // =====

    // Queue a command to retrieve the result. Note that this is a blocking command (CL_TRUE) so
    // it will wait until there is something to read before this function returns.
    status = clEnqueueReadBuffer( cmdQueue, bufC, CL_TRUE, 0, datasize, C, 0, NULL, NULL );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error retrieving computed result!\n" );
        goto release_kernel;
    }
    printf( "OpenCL: retrieved the computed result successfully.\n");

    // =====

    // Display the answer.
    //for( int i = 0; i < elements; ++i ) {
    //    printf( "C[%4d] = %d\n", i, C[i] );
    //}

    // Free OpenCL resources.
 release_kernel:
    clReleaseKernel( kernel );

 release_program:
    clReleaseProgram( program );

 release_buffers:
    clReleaseMemObject( bufC );
    clReleaseMemObject( bufB );
    clReleaseMemObject( bufA );
    clReleaseCommandQueue( cmdQueue );

 release_context:
    clReleaseContext( context );

 release_host_memory:
    free( C );
    free( B );
    free( A );
    return return_code;
}

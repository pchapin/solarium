/*
 * This program uses OpenCL to add two vectors of integers of length 2048 elements. It is a
 * heavily edited version of the sample program in "Hetergeneous Computing with OpenCL 2.0" by
 * Kaeli, Mistry, Schaa, and Zhang (copyright 2015, published by Morgan Kaufmann,
 * ISBN=978-0-12-801414-1), Section 3.6.
 */

// I should be using strcat_s, but I don't want to bother.
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CL_TARGET_OPENCL_VERSION 220  // We want version 2.2 (for now).
#include <CL/cl.h>

// Arbitrary limits defined by this program.
#define PLATFORM_INFO_STRING_LENGTH 128
#define DEVICE_INFO_STRING_LENGTH   128

const char *program_source =
    "__kernel\n"
    "void vecadd(__global int *A,\n"
    "            __global int *B,\n"
    "            __global int *C)\n"
    "{\n"
    "    int index = get_global_id( 0 );\n"
    "    C[index] = A[index] + B[index];\n"
    "}\n";


// These numbers are intended to be indexes into the platform and device arrays returned by
// clGetPlatformIDs and clGetDeviceIDs. The special values of -1 means no platform (or device).
struct DeviceNumbers {
    int platform;
    int device;
};

// These are the actual OpenCL identifiers needed by the result of the API. They are NULL if no
// platform (or device) has been selected or if certain errors are encountered.
struct DeviceIDs {
    cl_platform_id platform;
    cl_device_id device;
};


// Searches the command line looking for -p and -d options to select platform and device.
struct DeviceNumbers analyze_command_line( int argc, char **argv )
{
    struct DeviceNumbers result = { -1, -1 };  // No platform, no device.

    while( *++argv ) {
        if( **argv == '-' ) {
            // We are looking at an option switch.
            switch( *++*argv ) {
            case 'p':
                result.platform = atoi( ++*argv );
                break;
                
            case 'd':
                result.device = atoi( ++*argv );
                break;
                
            default:
                fprintf( stderr, "*** Unknown option: '%c' ignored!\n", **argv );
                break;
            }
        }
        else {
            // We are looking at an ordinary command line argument.
            fprintf( stderr, "*** Unexpected command line argument: \"%s\" ignored!\n", *argv );
        }
    }

    return result;
}


char *device_type_breakout( cl_device_type device_type )
{
    static char workspace[128+1];

    workspace[0] = '\0';
    if( device_type == CL_DEVICE_TYPE_CUSTOM ) {
        return "CL_DEVICE_TYPE_CUSTOM";
    }
    if( device_type & CL_DEVICE_TYPE_CPU ) {
        strcat( workspace, "CL_DEVICE_TYPE_CPU " );
    }
    if( device_type & CL_DEVICE_TYPE_GPU ) {
        strcat( workspace, "CL_DEVICE_TYPE_GPU " );
    }
    if( device_type & CL_DEVICE_TYPE_ACCELERATOR ) {
        strcat( workspace, "CL_DEVICE_TYPE_ACCELERATOR " );
    }
    if( device_type & CL_DEVICE_TYPE_DEFAULT ) {
        strcat( workspace, "CL_DEVICE_TYPE_DEFAULT ");
    }
    return workspace;
}


struct DeviceIDs select_device( cl_platform_id platform, int desired_device )
{
    cl_uint device_count;
    cl_int  status;
    char    device_info[DEVICE_INFO_STRING_LENGTH + 1];
    cl_device_type device_type;
    cl_uint compute_units;
    cl_bool compiler_available;

    struct DeviceIDs ids = { platform, NULL };   // No device (yet).
    
    // How many devices?
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_ALL, 0, NULL, &device_count );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error getting device count!\n" );
        return ids;
    }
    printf( "OpenCL: %d device%s found.\n", (int)device_count, (device_count == 1) ? "" : "s" );

    if( desired_device != -1 && desired_device >= (int)device_count ) {
        fprintf( stderr, "*** Desired device (%d) does not exist!\n", desired_device );
        return ids;
    }

    // Get device IDs.
    cl_device_id *devices =
        (cl_device_id *)malloc( device_count * sizeof( cl_device_id ) );
    
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_ALL, device_count, devices, NULL );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error getting device IDs!\n" );
        return ids;
    }

    // Save the selected device ID to return later.
    if( desired_device != -1 ) {
        ids.device = devices[desired_device];
    }

    // Print header.
    printf( "\n" );
    if( desired_device == -1 ) {
        printf( "Device List\n"
                "-----------\n\n" );
    }
    else {
        printf( "Selected Device\n"
                "---------------\n\n" );
    }
    
    // Display information about the devices.
    for( cl_uint device_index = 0; device_index < device_count; ++device_index ) {
        if( desired_device == -1 || (int)device_index == desired_device ) {
            clGetDeviceInfo(
                devices[device_index],
                CL_DEVICE_NAME,
                DEVICE_INFO_STRING_LENGTH + 1,
                device_info,
                NULL );
            printf( "%2d. NAME           : %s\n", (int)device_index, device_info );

            clGetDeviceInfo(
                devices[device_index],
                CL_DEVICE_VENDOR,
                DEVICE_INFO_STRING_LENGTH + 1,
                device_info,
                NULL );
            printf( "    VENDOR         : %s\n", device_info );

            clGetDeviceInfo(
                devices[device_index],
                CL_DRIVER_VERSION,
                DEVICE_INFO_STRING_LENGTH + 1,
                device_info,
                NULL );
            printf( "    DRIVER VERSION : %s\n", device_info );        

            clGetDeviceInfo(
                devices[device_index],
                CL_DEVICE_TYPE,
                sizeof( cl_device_type ),
                &device_type,
                NULL );
            printf( "    TYPE           : %s\n", device_type_breakout( device_type ) );        

            clGetDeviceInfo(
                devices[device_index],
                CL_DEVICE_MAX_COMPUTE_UNITS,
                sizeof( cl_uint ),
                &compute_units,
                NULL );
            printf( "    COMPUTE UNITS  : %d\n", (int)compute_units );        

            clGetDeviceInfo(
                devices[device_index],
                CL_DEVICE_COMPILER_AVAILABLE,
                sizeof( cl_bool ),
                &compiler_available,
                NULL );
            printf( "    COMPILER       : %s\n", (compiler_available == CL_TRUE) ? "Yes" : "No" );        
            printf( "\n" );
        }
    }
    free( devices );
    return ids;
}


struct DeviceIDs select_platform_and_device( struct DeviceNumbers dn )
{
    cl_uint platform_count;
    cl_int  status;
    char    platform_info[PLATFORM_INFO_STRING_LENGTH + 1];

    struct DeviceIDs ids = { NULL, NULL };

    // How many platforms?
    status = clGetPlatformIDs( 0, NULL, &platform_count );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error getting platform count!\n" );
        return ids;
    }
    printf( "OpenCL: %d platform%s found.\n", (int)platform_count, (platform_count == 1) ? "" : "s" );

    if( dn.platform != -1 && dn.platform >= (int)platform_count ) {
        fprintf( stderr, "*** Desired platform (%d) does not exist!\n", dn.platform );
        return ids;
    }

    // Get platform IDs.
    cl_platform_id *platforms =
        (cl_platform_id *)malloc( platform_count * sizeof( cl_platform_id ) );
    
    status = clGetPlatformIDs( platform_count, platforms, NULL );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error getting platform IDs!\n" );
        return ids;
    }

    // Save the selected platform ID to return later.
    if( dn.platform != -1 ) {
        ids.platform = platforms[dn.platform];
    }

    // Print header.
    printf( "\n" );
    if( dn.platform == -1 ) {
        printf( "Platform List\n"
                "-------------\n\n" );
    }
    else {
        printf( "Selected Platform\n"
                "-----------------\n\n" );
    }
    
    // Display information about the platforms.
    for( cl_uint platform_index = 0; platform_index < platform_count; ++platform_index ) {
        if( dn.platform == -1 || (int)platform_index == dn.platform ) {
            clGetPlatformInfo(
                platforms[platform_index],
                CL_PLATFORM_NAME,
                PLATFORM_INFO_STRING_LENGTH + 1,
                platform_info,
                NULL );
            printf( "%2d. NAME    : %s\n", (int)platform_index, platform_info );

            clGetPlatformInfo(
                platforms[platform_index],
                CL_PLATFORM_VENDOR,
                PLATFORM_INFO_STRING_LENGTH + 1,
                platform_info,
                NULL );
            printf( "    VENDOR  : %s\n", platform_info );

            clGetPlatformInfo(
                platforms[platform_index],
                CL_PLATFORM_VERSION,
                PLATFORM_INFO_STRING_LENGTH + 1,
                platform_info,
                NULL );
            printf( "    VERSION : %s\n", platform_info );        

            clGetPlatformInfo(
                platforms[platform_index],
                CL_PLATFORM_PROFILE,
                PLATFORM_INFO_STRING_LENGTH + 1,
                platform_info,
                NULL );
            printf( "    PROFILE : %s\n", platform_info );        
            printf( "\n" );
        }
    }
    free( platforms );

    if( dn.platform == -1 ) {
        return ids;
    }
    return select_device( ids.platform, dn.device );
}


int main( int argc, char **argv )
{
    // This code executes on the OpenCL host.

    struct DeviceIDs ids = select_platform_and_device( analyze_command_line( argc, argv ) );
    if( ids.platform == NULL ) {
        fprintf( stderr, "*** Use the -p command line option to select a platform number!\n" );
        return EXIT_FAILURE;
    }
    if( ids.device == NULL ) {
        fprintf( stderr, "*** Use the -d command line option to select a device number!\n" );
        return EXIT_FAILURE;
    }

    const int     elements       = 2048;  // Elements in each array.
          cl_int  status;                 // Use this to check the output of each API call.
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

    // =====

    // First define a list of property values that includes which platform we want to use.
    cl_context_properties properties[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)ids.platform,
        0
    };
    // Now create a context for the first device on the platform of interest.
    cl_context context = clCreateContext( properties, 1, &ids.device, NULL, NULL, &status );
    if( status != CL_SUCCESS ) {
        fprintf( stderr, "*** Error creating the context!\n" );
        return_code = EXIT_FAILURE;
        goto release_host_memory;
    }
    printf( "OpenCL: context created successfully for the first device on the first platform.\n" );

    // =====

    // Create a command queue and associate it with the device.
    cl_command_queue cmdQueue = clCreateCommandQueueWithProperties( context, ids.device, NULL, &status );
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
    status = clBuildProgram( program, 1, &ids.device, NULL, NULL, NULL );
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

    // Check the answer.
    int correct = 1;
    for( int i = 0; i < elements; ++i ) {
        if( C[i] != 2 * i ) correct = 0;
    }
    if( correct ) {
        printf( "Correct answer computed.\n" );
    }
    else {
        printf( "*** Incorrect answer computed!\n" );
    }

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

#include <string>
#include <iostream>
#include "cl_manager.hpp"

using namespace Raytracer;
using namespace std;

Manager::~Manager() {
    if (kernel  != nullptr) clReleaseKernel(kernel);
    if (program != nullptr) clReleaseProgram(program);
    if (queue   != nullptr) clReleaseCommandQueue(queue);
    if (context != nullptr) clReleaseContext(context);
}

void Manager::initialize() {
    /* get platforms */
    cl_uint platform_count = 0;
    checkError(clGetPlatformIDs(0, nullptr, &platform_count),
               "Could not get platform count");
    if (log_level == VERBOSE)
        cout << "Found " << platform_count << " OpenCL platforms" << endl;
    if (platform_count == 0)
        fail("Could not find any OpenCL Platforms");
    std::vector<cl_platform_id> platforms(platform_count);
    checkError(clGetPlatformIDs(platform_count, &platforms[0], nullptr),
               "Could not get platforms");
    if (log_level == VERBOSE)
        for (unsigned int i = 0; i < platform_count; i++) {
            cout << "  Platform #" << i << endl;
            printPlatform(platforms[i]);
        }
    /* choose platform */
    if (platform_choice < platform_count) {
        platform = platforms[platform_choice];
        if (log_level == VERBOSE)
            cout << "Chose platform #" << platform_choice << endl;
    } else {
        platform = platforms[0];
        if (log_level == VERBOSE)
            cout << "Platform choice invalid: defaulted to platform #0" << endl;
    }
    /* get devices */
    cl_uint device_count = 0;
    cl_device_type type = 0;
    if (log_level == VERBOSE)
        switch(device_type) {
            case GPU:
                type = CL_DEVICE_TYPE_GPU;
                cout << "Querying GPU devices" << endl;
                break;
            case CPU:
                type = CL_DEVICE_TYPE_CPU;
                cout << "Querying CPU devices" << endl;
                break;
            case ANY:
                type = CL_DEVICE_TYPE_ALL;
                cout << "Querying all devices" << endl;
                break;
        };
    checkError(clGetDeviceIDs(platform, type, 0, nullptr, &device_count),
               "Could not get device count");
    if (device_count == 0)
        fail("Could not find any OpenCL devices");
    std::vector<cl_device_id> devices(device_count);
    checkError(clGetDeviceIDs(platform, type, device_count, &devices[0], nullptr),
               "Could not get devices");
    if (log_level == VERBOSE)
        for (unsigned int i = 0; i < device_count; i++) {
            cout << "  Device #" << i << endl;
            printDevice(devices[i]);
        }
    /* choose device */
    if (device_choice < device_count) {
        device = devices[device_choice];
        if (log_level == VERBOSE)
            cout << "Chose device #" << device_choice << endl;
    } else { 
        device = devices[0];
        if (log_level == VERBOSE)
            cout << "Device choice invalid: defaulted to device #0" << endl;
    }
    /* create context */
    cl_int status = 0;
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &status);
    if (status != CL_SUCCESS)
        fail("Could not create device context");
    if (log_level == VERBOSE)
        cout << "Created device context" << endl;
    /* create command queue */
    queue = clCreateCommandQueueWithProperties(context, device, NULL, &status);
    if (status != CL_SUCCESS)
        fail("Could not create command queue");
    if (log_level == VERBOSE)
        cout << "Created command queue" << endl;
}

void Manager::checkError(cl_int result, const string &message) {
    if (result != CL_SUCCESS && log_level != SILENT)
            cerr << "OpenCL Error: " << message << endl;
}
void Manager::fail(const string &message) {
    if (log_level != SILENT)
        cerr << message << endl;
    exit(EXIT_FAILURE);
}

void Manager::printPlatform(cl_platform_id id) {
    char buf[2048];
    clGetPlatformInfo(id, CL_PLATFORM_VERSION, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
    clGetPlatformInfo(id, CL_PLATFORM_NAME, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
}
void Manager::printDevice(cl_device_id id) {
    char buf[2048];
    clGetDeviceInfo(id, CL_DEVICE_NAME, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
    clGetDeviceInfo(id, CL_DEVICE_OPENCL_C_VERSION, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
}

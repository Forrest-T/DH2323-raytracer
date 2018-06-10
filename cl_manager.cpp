#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "cl_manager.hpp"
#include "structs.hpp"

using namespace Raytracer;
using namespace std;

CL_Manager::~CL_Manager() {
    if (queue   != nullptr) clReleaseCommandQueue(queue);
    if (context != nullptr) clReleaseContext(context);
}

void CL_Manager::initialize() {
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
            cerr << "Platform choice invalid: defaulted to platform #0" << endl;
    }
    /* get devices */
    cl_uint device_count = 0;
    cl_device_type type = 0;
    switch(device_type) {
        case GPU: type = CL_DEVICE_TYPE_GPU; break;
        case CPU: type = CL_DEVICE_TYPE_CPU; break;
        case ANY: type = CL_DEVICE_TYPE_ALL; break;
    };
    if (log_level == VERBOSE)
        switch(device_type) {
            case GPU: cout << "Querying GPU devices" << endl; break;
            case CPU: cout << "Querying CPU devices" << endl; break;
            case ANY: cout << "Querying all devices" << endl; break;
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
            cerr << "Device choice invalid: defaulted to device #0" << endl;
    }
    /* create context */
    cl_int status = 0;
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &status);
    checkError(status, "Could not create device context");
    if (log_level == VERBOSE)
        cout << "Creat device context succeeded" << endl;
    /* create command queue */
    queue = clCreateCommandQueueWithProperties(context, device, NULL, &status);
    checkError(status, "Could not create command queue");
    if (log_level == VERBOSE)
        cout << "Create command queue succeeded" << endl;
}

void CL_Manager::checkError(cl_int result, const string &message) {
    if (result != CL_SUCCESS)
        fail(message);
}
void CL_Manager::fail(const string &message) {
    if (log_level != SILENT)
        cerr << message << endl;
    exit(EXIT_FAILURE);
}

void CL_Manager::printPlatform(cl_platform_id id) {
    char buf[2048];
    clGetPlatformInfo(id, CL_PLATFORM_VERSION, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
    clGetPlatformInfo(id, CL_PLATFORM_NAME, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
}

void CL_Manager::printDevice(cl_device_id id) {
    char buf[2048];
    clGetDeviceInfo(id, CL_DEVICE_NAME, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
    clGetDeviceInfo(id, CL_DEVICE_OPENCL_C_VERSION, sizeof(buf), &buf, nullptr);
    cout << "    " << buf << endl;
}

cl_program CL_Manager::createProgram(std::vector<std::string> filepaths) {
    /* read source */
    std::stringstream source_strs;
    for (const string &filepath : filepaths) {
        std::ifstream file(filepath);
        if (!file.good()) {
            cerr << "Unable to read file: " << filepath << endl;
            continue;
        }
        source_strs << file.rdbuf();
        file.close();
    }
    string source_str = source_strs.str();
    const char *source = source_str.c_str();

    /* create program */
    cl_int status = 0;
    cl_program out = clCreateProgramWithSource(context, 1, &source, nullptr, &status);
    if (status != CL_SUCCESS)
        fail("Read program source failed");
    status = clBuildProgram(out, 1, &device, "-Werror", nullptr, nullptr);
    if (status != CL_SUCCESS) {
        char buf[4096];
        clGetProgramBuildInfo(out, device, CL_PROGRAM_BUILD_LOG, sizeof(buf), &buf, nullptr);
            fail(string("Failed to build program:")+buf);
    }
    if (log_level == VERBOSE)
        cout << "Build program succeeded" << endl;
    return out;
}

cl_kernel CL_Manager::createKernel(cl_program p, const std::string &entry) {
    cl_int status = 0;
    cl_kernel out = clCreateKernel(p, entry.c_str(), &status);
    if (status != CL_SUCCESS)
        fail("Create kernel failed, error code "+std::to_string(status));
    if (log_level == VERBOSE)
        cout << "Create Kernel succeeded" << endl;
    return out;
}

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/cl2.hpp>

void getInfo() {
    cl_platform_id *platform_ids = NULL;
    cl_device_id device_id = NULL;
    cl_uint num_devices = 0;
    cl_uint num_platforms = 0;
    char buf[1024];

    clGetPlatformIDs(0, NULL, &num_platforms);
    printf("number of available platforms: %d\n",num_platforms);
    platform_ids = new cl_platform_id[num_platforms];
    clGetPlatformIDs(num_platforms, platform_ids, &num_platforms);
    for (unsigned int i = 0; i < num_platforms; i++) {
        clGetPlatformInfo(platform_ids[i], CL_PLATFORM_VERSION, sizeof(buf), &buf, NULL);
        printf("  platform #%d: %s\n",i,buf);
    }
    clGetPlatformInfo(platform_ids[0], CL_PLATFORM_VERSION, sizeof(buf), &buf, NULL);
    printf("chosen platform: %s\n",buf);
    const char *info_type[3] = {"profile", "version", "name"};
    cl_platform_info platform_info[3] = {CL_PLATFORM_PROFILE,
                                        CL_PLATFORM_VERSION,
                                        CL_PLATFORM_NAME};

    for (int i = 0 ; i < 3; i++) {
        clGetPlatformInfo(platform_ids[0], platform_info[i], sizeof(buf), &buf, NULL);
        printf("  %s: %s\n",info_type[i],buf);
    }

    clGetDeviceIDs(platform_ids[0], CL_DEVICE_TYPE_ALL, 1, &device_id, &num_devices);
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(buf), buf, NULL);
    printf("number of devices: %d\n",num_devices);
    printf("device name: %s\n",buf);
}

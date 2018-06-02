#ifndef CL_MANAGER_HPP_
#define CL_MANAGER_HPP_

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200
#include <CL/cl2.hpp>

#include <string>
#include <vector>

namespace Raytracer {
    enum log {SILENT, ERROR, VERBOSE};
    enum device {CPU, GPU, ANY};

    class CL_Manager {
    private:
        void checkError(cl_int result, const std::string &message);
        void fail(const std::string &message);
        void printPlatform(cl_platform_id id);
        void printDevice(cl_device_id id);
    public:
        /* settings */
        enum log log_level = ERROR;
        enum device device_type = ANY;
        unsigned int platform_choice = 0;
        unsigned int device_choice = 0;

        /* fields */
        cl_platform_id platform = nullptr;
        cl_device_id device = nullptr;
        cl_context context = nullptr;
        cl_command_queue queue = nullptr;

        /* subroutines */
        ~CL_Manager();
        void initialize();
        cl_program createProgram(std::vector<std::string>);
        cl_kernel createKernel(cl_program, const std::string &);
    };
} // namespace CL
#endif

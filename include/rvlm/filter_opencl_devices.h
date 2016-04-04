#ifndef FILTER_OPENCL_DEVICES_H_
#define FILTER_OPENCL_DEVICES_H_
#include <CL/cl.h>

int
find_opencl_device(
        cl_platform_id *out_platform,
        cl_device_id   *out_device,
        char const     *predicate_expr);

int
find_opencl_device_ext(
        cl_platform_id *out_platform,
        cl_device_id   *out_device,
        char const     *predicate_expr,
        void           *(reallocator)(void *p, size_t n, void* arg),
        void           *arg);

#endif

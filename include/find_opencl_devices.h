#ifndef FILTER_OPENCL_DEVICES_H_
#define FILTER_OPENCL_DEVICES_H_
#include <CL/cl.h>

int find_opencl_device(
		cl_platform_id *out_platform,
		cl_device_id   *out_device,
		char const     *predicate_expr);

int find_opencl_device_ext(
		cl_platform_id *out_platform,
		cl_device_id   *out_device,
		char const     *predicate_expr,
		void           *(*realloc)(void *p, size_t n, void* arg),
		void           *realloc_arg);

int find_opencl_devices(
		cl_platform_id *target_buf_platforms,
		cl_device_id   *target_buf_devices,
		size_t          target_buf_count,
		size_t         *out_actual_count,
		char const     *predicate_expr);

int find_opencl_devices_ext(
		cl_platform_id *target_buf_platforms,
		cl_device_id   *target_buf_devices,
		size_t          target_buf_count,
		size_t         *out_actual_count,
		char const     *predicate_expr,
		void           *(*realloc)(void *p, size_t n, void *arg),
		void           *realloc_arg);

#endif


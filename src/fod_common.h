#ifndef FOD_COMMON_H_
#define FOD_COMMON_H_

#include <stddef.h>
#include <CL/cl.h>

typedef unsigned long long fod_longest_uint;

typedef void *(*fod_reallocator)(void *p, size_t n, void *arg);

struct fod_opencl {

    cl_int (*get_platform_ids)(
	    cl_uint num_entries,
  	    cl_platform_id *platforms,
  	    cl_uint *num_platforms);

    cl_int (*get_device_ids)(
            cl_platform_id platform,
            cl_device_type device_type,
            cl_uint        num_entries,
            cl_device_id  *devices,
  	    cl_uint       *num_devices);

    cl_int (*get_platform_info)(
	    cl_platform_id   platform,
  	    cl_platform_info param_name,
  	    size_t           param_value_size,
            void            *param_value,
  	    size_t          *param_value_size_ret);
    
    cl_int (*get_device_info)(
	    cl_device_id   device,
            cl_device_info param_name,
  	    size_t         param_value_size,
  	    void          *param_value,
  	    size_t        *param_value_size_ret);
};

struct fod_opencl *fod_std_opencl(void);

void *fod_std_realloc(void *p, size_t n, void *arg);

char *fod_strdup(
		char const *str,
		fod_reallocator   realloc,
		void             *realloc_arg);

int fod_substring_to_uint(
        fod_longest_uint *out,
	char const       *start,
	char const       *end);

char *fod_substring_duplicate_and_unquote(
	char const       *start,
	char const       *end,
	fod_reallocator   realloc,
	void             *realloc_arg);

#endif

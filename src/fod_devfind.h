#ifndef FOD_DEVFIND_H_
#define FOD_DEVFIND_H_

#include "fod_common.h"

int fod_find_devices(
        cl_platform_id    *target_buf_platforms,
        cl_device_id      *target_buf_devices,
        size_t             target_buf_count,
        size_t            *out_actual_count,
        char const        *predicate_expr,
        fod_reallocator    realloc,
        void              *realloc_arg,
        struct fod_opencl *cl);

#endif

#ifndef FOD_TOKENS_H_
#define FOD_TOKENS_H_

#include <CL/cl.h>
#include "fod_common.h"
#include "fod_parser.h"

union fod_token {
    fod_longest_uint number;
    char const      *string;
    int              boolean;
};

#endif

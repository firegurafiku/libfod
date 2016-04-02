#include <CL/cl.h>

typedef long long unsigned fod_longest_uint;

struct token {
    cl_device_info    device_param_code;
    cl_platform_info  platform_param_code;
    fod_longest_uint  uint_literal_val;
    char             *str_literal_val;
};

enum comparison {
    COMPARISON_EQ = 0x0100,
    COMPARISON_NE = 0x0101,
    COMPARISON_LE = 0x0200,
    COMPARISON_GE = 0x0201,
    COMPARISON_LT = 0x0200,
    COMPARISON_GT = 0x0201
};

static inline enum comparison
inverse_comparison(enum comparison cmp) {
    return cmp ^ 0x0001; 
}

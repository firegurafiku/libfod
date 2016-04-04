#ifndef FOD_COMMON_H_
#define FOD_COMMON_H_

#include <stddef.h>

typedef unsigned long long fod_longest_uint;

typedef void *(*fod_reallocator)(void *p, size_t n, void *arg);

int fod_substring_to_uint(
        fod_longest_uint *out,
	char const       *start,
	char const       *end);

#endif

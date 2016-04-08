#ifndef FOD_COMMON_H_
#define FOD_COMMON_H_

#include <stddef.h>
#include <CL/cl.h>

typedef unsigned long long fod_longest_uint;

typedef void *(*fod_reallocator)(void *p, size_t n, void *arg);

void *fod_std_realloc(void *p, size_t n, void *arg);

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

#include "fod_common.h"
#include "stdlib.h"

void *fod_std_realloc(void *p, size_t n, void *arg) {
    (void)arg;
    return realloc(p, n);
}

int fod_substring_to_uint(
        fod_longest_uint *out,
	char const       *start,
	char const       *end) {

    fod_longest_uint res = 0;
    char const *c = start;

    /** We're not really going to support THAT long numbers. Nineteen
        digits are enough for all purposes of this century. */
    if (end - start > 19)
	return 0;
    
    // http://stackoverflow.com/a/868508/1447225
    while (c < end && '0' <= *c && *c <= '9') {
	res = res*10 + (unsigned)(*c - '0');
	++c;
    }

    if (c != end)
	return 0;

    *out = res;
    return 1;
}

char *fod_substring_duplicate_and_unquote(
	char const       *start,
	char const       *end,
	fod_reallocator   realloc,
	void             *realloc_arg) {

    char const *c =NULL;
    char *dst = NULL;

    if (!start || !end || end - start < 2)
	return NULL;
    
    if (*start != *(end-1))
	return NULL;

    if (*start != '"' && *start != '\'')
	return NULL;

    ++start;
    --end;

    size_t delta = 0;
    for (c = start; c < end; ) {
        if (*c != '\\')
	    ++c;
	else {
	    if (c == end - 1)
		return NULL;

	    switch (*(c+1)) {
		case '\\':
		case '\'':
		case '"':
		case 'a':
		case 'b':
		case 'f':
		case 'n':
		case 'r':
		case 't':
		case 'v':
		case '?':
		case '0':
		    c += 2;
		    ++delta;
		    break;

		default:
		    return NULL;
	    }
	}
    }

    size_t n = (size_t)(end - start) - delta + 1;
    char *p = realloc(NULL, n, realloc_arg);
    
    if (!p)
	return p;

    dst = p;
    for (c = start; c < end; ) {
        if (*c != '\\') {
	    *dst = *c;
	    ++c;
	} else {
	    if (c == end - 1)
		return NULL;

	    switch (*(c+1)) {
		case '\\': *dst = '\\'; break;
		case '\'': *dst = '\''; break;
		case '"':  *dst = '"';  break;
		case 'a':  *dst = '\a'; break;
		case 'b':  *dst = '\b'; break;
		case 'f':  *dst = '\f'; break;
		case 'n':  *dst = '\n'; break;
		case 'r':  *dst = '\r'; break;
		case 't':  *dst = '\t'; break;
		case 'v':  *dst = '\v'; break;
		case '?':  *dst = '?';  break;
		case '0':  *dst = '\0'; break;
                default: ; /* IMPOSSIBLE */
	    }
	    
	    c += 2;
	}
	
	++dst;
    }
    
    
    *dst = '\0';
    return p;
}

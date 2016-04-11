#ifndef FOD_PARSER_H_
#define FOD_PARSER_H_

#include "fod_common.h"
#include "fod_parser_gen.h"

union fod_token {
    fod_longest_uint number;
    char            *string;
    int              boolean;
};

struct fod_parser {
    void            *lemon_parser;
    fod_reallocator  realloc;
    void            *realloc_arg;
	int              destruct_tokens;
    int              result;
    int              is_error;
};

int fod_parser_init(
        struct fod_parser *out,
	fod_reallocator    realloc,
	void              *realloc_arg);

void fod_parser_close(struct fod_parser *parser);

void fod_parser_consume_token(
	struct fod_parser     *parser,
	int                    major,
	union fod_token const *minor);

#endif

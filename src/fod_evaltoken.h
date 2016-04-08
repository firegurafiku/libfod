#ifndef FOD_EVALTOK_H_
#define FOD_EVALTOK_H_

#include "fod_common.h"
#include "fod_lexer.h"
#include "fod_parser.h"

int fod_evaluate_lexeme_to_token(
	fod_reallocator                realloc,
	void                          *realloc_param,
	struct fod_opencl             *cl,
	cl_platform_id                 platform_id,
	cl_device_id                   device_id,
        enum fod_lexeme_major          lex_major,
	struct fod_lexeme_minor const *lex_minor,
        int                           *out_token_major,
        union fod_token               *out_token_minor);

int fod_evaluate_lexeme_to_token_noaction(
        enum fod_lexeme_major          lex_major,
	struct fod_lexeme_minor const *lex_minor,
        int                           *out_token_major,
        union fod_token               *out_token_minor);
    
#endif

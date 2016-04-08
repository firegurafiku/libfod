#include "fod_evaltoken.h"

static int get_device_number(
        struct fod_opencl *cl,
	cl_device_id       device,
	cl_device_info     param,
	void              *out_buf,
	size_t             buf_size) {

    cl_int result;
    size_t ret_size;
    result = cl->get_device_info(device, param,
		   buf_size, &out_buf, &ret_size);

    return (result == CL_SUCCESS && ret_size == buf_size);
}

static char *get_device_string_alloc(
        struct fod_opencl *cl,
	cl_device_id       device,
	cl_device_info     param,
	fod_reallocator    realloc,
	void              *realloc_arg) {

    size_t required_size = 0;
    cl_int errcode;
    char  *result = 0;

    errcode = cl->get_device_info(device, param, 0, NULL, &required_size);
    if (errcode != CL_SUCCESS)
	goto e_failure;

    result = realloc(NULL, required_size + 1, realloc_arg);
    if (result == NULL)
	goto e_failure;

    errcode = cl->get_device_info(device, param, required_size, NULL, NULL);
    if (errcode != CL_SUCCESS)
	goto e_failure;

    result[required_size] = '\0';
    return result;

e_failure:
    if (result)
	realloc(result, 0, realloc_arg);

    return NULL;
}

int fod_evaluate_lexeme_to_token_noaction(
        enum fod_lexeme_major          lex_major,
	struct fod_lexeme_minor const *lex_minor,
        int                           *out_token_major,
        union fod_token               *out_token_minor) {

    int okay;
    switch (lex_major) {
	
	/* Simple tokens (no additional data). */
	
	case LEX_EOF:          *out_token_major = 0;                goto e_success;
	case LEX_PAREN_LEFT:   *out_token_major = TOK_PAREN_LEFT;   goto e_success;
	case LEX_PAREN_RIGHT:  *out_token_major = TOK_PAREN_RIGHT;  goto e_success;
	case LEX_OPERATOR_NOT: *out_token_major = TOK_OPERATOR_NOT; goto e_success;
	case LEX_OPERATOR_AND: *out_token_major = TOK_OPERATOR_AND; goto e_success;
	case LEX_OPERATOR_OR:  *out_token_major = TOK_OPERATOR_OR;  goto e_success;
	case LEX_OPERATOR_EQ:  *out_token_major = TOK_OPERATOR_EQ;  goto e_success;
	case LEX_OPERATOR_NE:  *out_token_major = TOK_OPERATOR_NE;  goto e_success;
	case LEX_OPERATOR_LT:  *out_token_major = TOK_OPERATOR_LT;  goto e_success;
	case LEX_OPERATOR_GT:  *out_token_major = TOK_OPERATOR_GT;  goto e_success;
	case LEX_OPERATOR_LE:  *out_token_major = TOK_OPERATOR_LE;  goto e_success;
	case LEX_OPERATOR_GE:  *out_token_major = TOK_OPERATOR_GE;  goto e_success;

        /* Tokens for literals (just copy their data). */

	case LEX_LITERAL_NUMBER:
	    *out_token_major = TOK_NUMBER;
	    out_token_minor->number = lex_minor->literal_number;
	    goto e_success;

	case LEX_LITERAL_STRING:
	    *out_token_major = TOK_STRING;
	    out_token_minor->string = lex_minor->literal_string;
	    goto e_success;

	case LEX_PARAM_BOOL:
	    *out_token_major = TOK_BOOLEAN;
	    out_token_minor->boolean = !0;
	    break;
	
	case LEX_PARAM_UINT:
	case LEX_PARAM_ULONG:
	case LEX_PARAM_SIZE_T:
	    *out_token_major = TOK_NUMBER;
	    out_token_minor->number = 0;
	    break;
	    
	case LEX_PARAM_FP_CONFIG:
	    *out_token_major = TOK_BOOLEAN;
	    out_token_minor->boolean = !0;
	    break;
	    
        case LEX_PARAM_STRING:
	    *out_token_major = TOK_STRING;
	    out_token_minor->string = "";
	    break;
	    
        default:
	    goto e_failure;
    }

e_success:
    return 1;
    
e_failure:
    return 0;
}

int fod_evaluate_lexeme_to_token(
	fod_reallocator                realloc,
	void                          *realloc_arg,
	struct fod_opencl             *cl,
	cl_platform_id                 platform,
	cl_device_id                   device,
        enum fod_lexeme_major          lex_major,
	struct fod_lexeme_minor const *lex_minor,
        int                           *out_token_major,
        union fod_token               *out_token_minor) {

    union  {
	cl_device_fp_config as_fpconfig;
	cl_bool             as_clbool;
	cl_uint             as_cluint;
	cl_ulong            as_clulong;
	size_t              as_sizeti;
    } value;

    char *str;
    int okay;
    
    switch (lex_major) {
	
	/* Simple tokens (no additional data). */
	
	case LEX_EOF:          *out_token_major = 0;                goto e_success;
	case LEX_PAREN_LEFT:   *out_token_major = TOK_PAREN_LEFT;   goto e_success;
	case LEX_PAREN_RIGHT:  *out_token_major = TOK_PAREN_RIGHT;  goto e_success;
	case LEX_OPERATOR_NOT: *out_token_major = TOK_OPERATOR_NOT; goto e_success;
	case LEX_OPERATOR_AND: *out_token_major = TOK_OPERATOR_AND; goto e_success;
	case LEX_OPERATOR_OR:  *out_token_major = TOK_OPERATOR_OR;  goto e_success;
	case LEX_OPERATOR_EQ:  *out_token_major = TOK_OPERATOR_EQ;  goto e_success;
	case LEX_OPERATOR_NE:  *out_token_major = TOK_OPERATOR_NE;  goto e_success;
	case LEX_OPERATOR_LT:  *out_token_major = TOK_OPERATOR_LT;  goto e_success;
	case LEX_OPERATOR_GT:  *out_token_major = TOK_OPERATOR_GT;  goto e_success;
	case LEX_OPERATOR_LE:  *out_token_major = TOK_OPERATOR_LE;  goto e_success;
	case LEX_OPERATOR_GE:  *out_token_major = TOK_OPERATOR_GE;  goto e_success;

        /* Tokens for literals (just copy their data). */

	case LEX_LITERAL_NUMBER:
	    *out_token_major = TOK_NUMBER;
	    out_token_minor->number = lex_minor->literal_number;
	    goto e_success;

	case LEX_LITERAL_STRING:
	    *out_token_major = TOK_STRING;
	    out_token_minor->string = lex_minor->literal_string;
	    goto e_success;

	/* Tokens for parameters. To obtain their values quering the device is
           needed, as well as additional memory allocation for strings. */

	case LEX_PARAM_BOOL:
	    okay = get_device_number(cl, device, lex_minor->device_param_code,
				                   &value, sizeof(cl_bool));
	    if (!okay)
		goto e_failure;

	    *out_token_major = TOK_BOOLEAN;
	    out_token_minor->boolean = (value.as_clbool != CL_FALSE);
	    break;
	
	case LEX_PARAM_UINT:
	    okay = get_device_number(cl, device, lex_minor->device_param_code,
				                   &value, sizeof(cl_uint));
	    if (!okay)
		goto e_failure;

	    *out_token_major = TOK_NUMBER;
	    out_token_minor->number = value.as_cluint;
	    break;
	    
	case LEX_PARAM_ULONG:
	    okay = get_device_number(cl, device, lex_minor->device_param_code,
				                   &value, sizeof(cl_ulong));
	    if (!okay)
		goto e_failure;

	    *out_token_major = TOK_NUMBER;
	    out_token_minor->number = value.as_clulong;
	    break;
	    
	case LEX_PARAM_SIZE_T:
	    okay = get_device_number(cl, device, lex_minor->device_param_code,
				                   &value, sizeof(size_t));
	    if (!okay)
		goto e_failure;

	    *out_token_major = TOK_NUMBER;
	    out_token_minor->number = value.as_sizeti;
	    break;
	    
	case LEX_PARAM_FP_CONFIG:
	    okay = get_device_number(cl, device, lex_minor->device_param_code,
				       &value, sizeof(cl_device_fp_config));
	    if (!okay)
		goto e_failure;

	    *out_token_major = TOK_BOOLEAN;
	    out_token_minor->boolean =
		        (value.as_fpconfig & lex_minor->fp_config_mask != 0);
	    break;
	    
        case LEX_PARAM_STRING:
	    str = get_device_string_alloc(cl, device, lex_minor->device_param_code, realloc, realloc_arg);
	    if (str == NULL)
		goto e_failure;

	    *out_token_major = TOK_STRING;
	    out_token_minor->string = str;
	    break;
	    
        default:
	    goto e_failure;
    }

e_success:
    return 1;
    
e_failure:
    return 0;
}

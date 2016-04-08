#include "fod_common.h"
#include "fod_lexer.h"
#include <string.h>
#include <stdio.h>

%% machine fod_lexer;
%% write data;

void fod_lexer_init(
	struct fod_lexer *lex,
	char const       *input,
	fod_reallocator   realloc,
	void             *realloc_arg) {

    lex->realloc             = realloc;
    lex->realloc_arg         = realloc_arg;
    lex->input               = input;
    lex->input_end           = input + strlen(input);
    lex->is_eof              = 0;
    lex->is_error            = 0;
    lex->current_token_start = NULL;
    lex->current_token_end   = NULL;
    lex->current_position    = input;
    lex->current_state       = fod_lexer_start;
    lex->current_action      = 0;
}

void fod_lexer_close(struct fod_lexer *lex) {
    (void)lex; /* Actually, nothing. */
}

int fod_lexer_tokenize(
        struct fod_lexer        *lex,
        enum fod_lexeme_major   *out_major,
        struct fod_lexeme_minor *out_minor) {
    
    /* Don't use ragel's 'write init;' construct. Instead, perform manual
       state restoration. */
    char const *p   = lex->current_position;
    char const *pe  = lex->input_end;
    char const *ts  = lex->current_token_start;
    char const *te  = lex->current_token_end;
    char const *eof = pe;
    
    int cs  = lex->current_state;
    int act = lex->current_action;
    
    /* In Lemon, valid token major code is never zero, 'cause zero
       means 'EOF'. Let's return it in case of errors. */
    *out_major = 0;
    
    %%{
    number = digit+;
    single_quoted_string = "'" ([^']|"\\'")* "'";
    double_quoted_string = '"' ([^"]|'\\"')* '"';
    main := |*
        # Skip any whitespaces which are not in string literal.
        space+ => { printf("SPACES\n"); };

        # Parse 
        '=='  => { *out_major = LEX_OPERATOR_EQ; printf("==\n"); fbreak; };
        '!='  => { *out_major = LEX_OPERATOR_NE; printf("!=\n"); fbreak; };
        '<='  => { *out_major = LEX_OPERATOR_LE; printf("<=\n"); fbreak; };
        '>='  => { *out_major = LEX_OPERATOR_GE; printf(">=\n"); fbreak; };
        '<'   => { *out_major = LEX_OPERATOR_LT; printf("<\n");  fbreak; };
        '>'   => { *out_major = LEX_OPERATOR_GT; printf(">\n");  fbreak; };
        '('   => { *out_major = LEX_PAREN_LEFT;  printf("(\n");  fbreak; };
        ')'   => { *out_major = LEX_PAREN_RIGHT; printf(")\n");  fbreak; };

        # 
        '&&'  => { *out_major = LEX_OPERATOR_AND; printf("&&\n"); fbreak; };
        '||'  => { *out_major = LEX_OPERATOR_OR;  printf("||\n"); fbreak; };
        '!'   => { *out_major = LEX_OPERATOR_NOT; printf("!\n");  fbreak; };

        # Unsigned integer literal.
        number => {
	    fod_longest_uint res;
	    if (!fod_substring_to_uint(&res, ts, te))
	       fbreak;
	    
            *out_major = LEX_LITERAL_NUMBER;
            out_minor->literal_number = res;
            fbreak;
        };

        single_quoted_string |
        double_quoted_string => {
	    char *str = fod_substring_duplicate_and_unquote(ts, te,
                                            lex->realloc, lex->realloc_arg);
	    if (str == NULL)
	        fbreak;

            *out_major = LEX_LITERAL_STRING;
            out_minor->literal_string = str;
            fbreak;
        };

        # Parameters returning cl_bool.
	
        'cl_device_available'
	| 'device_available'
	| 'deviceAvailable'
	| 'available' => {
                *out_major = LEX_PARAM_BOOL;
                out_minor->device_param_code = CL_DEVICE_AVAILABLE;
	        fbreak;
        };

        'cl_device_compiler_available'
	| 'device_compiler_available'
	| 'deviceCompilerAvailable'
	| 'compiler_available'
	| 'compilerAvailable' => {
                *out_major = LEX_PARAM_BOOL;
                out_minor->device_param_code = CL_DEVICE_COMPILER_AVAILABLE;
	        fbreak;
        };

        'cl_device_endian_little'
	| 'device_endian_little'
	| 'endian_little'
	| 'little_endian'
	| 'deviceEndianLittle'
	| 'deviceLittleEndian'
	| 'endianLittle'
	| 'littleEndian' => {
                *out_major = LEX_PARAM_BOOL;
                out_minor->device_param_code = CL_DEVICE_ENDIAN_LITTLE;
	        fbreak;
        };

	'cl_device_error_correction_support'
	| 'device_error_correction_support'
	| 'device_error_correction'
	| 'error_correction_support'
	| 'error_correction'
	| 'deviceErrorCorrectionSupport'
	| 'deviceErrorCorrection'
	| 'errorCorrectionSupport'
	| 'errorCorrection' => {
                *out_major = LEX_PARAM_BOOL;
                out_minor->device_param_code = CL_DEVICE_ERROR_CORRECTION_SUPPORT;
	        fbreak;
	};

	'cl_exec_kernel'
	| 'exec_kernel'
	| 'device_exec_kernel'
	| 'execKernel'
	| 'deviceExecKernel' => {
                *out_major = LEX_PARAM_BOOL;
                out_minor->device_param_code = CL_EXEC_KERNEL;
	        fbreak;
        };

	'cl_exec_native_kernel'
	| 'exec_native_kernel'
	| 'device_exec_native_kernel'
	| 'execNativeKernel'
	| 'deviceExecNativeKernel' => {
                *out_major = LEX_PARAM_BOOL;
                out_minor->device_param_code = CL_EXEC_NATIVE_KERNEL;
	        fbreak;
        };

	'cl_device_image_support'
	| 'device_image_support'
	| 'image_support'
	| 'deviceImageSupport'
	| 'imageSupport' => {
                *out_major = LEX_PARAM_BOOL;
                out_minor->device_param_code = CL_DEVICE_IMAGE_SUPPORT;
	        fbreak;
        };
	
        # Parameters returning cl_uint.

	'cl_device_address_bits'
	| 'device_address_bits'
	| 'address_bits'
	| 'deviceAddressBits'
	| 'addressBits'
	=> {
                *out_major = LEX_PARAM_UINT;
                out_minor->device_param_code = CL_DEVICE_ADDRESS_BITS;
	        fbreak;
        };
	
	'cl_device_global_mem_cacheline_size'
	| 'device_global_mem_cacheline_size'
	| 'global_mem_cacheline_size'
	| 'deviceglobalmemcachelinesize'
	| 'globalmemcachelinesize'
	=> {
                *out_major = LEX_PARAM_UINT;
                out_minor->device_param_code = CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE;
	        fbreak;
        };

	'cl_device_max_clock_frequency'
	| 'device_max_clock_frequency'
	| 'max_clock_frequency'
	| 'deviceMaxClockFrequency'
	| 'maxClockFrequency'
	=> {
                *out_major = LEX_PARAM_UINT;
                out_minor->device_param_code = CL_DEVICE_MAX_CLOCK_FREQUENCY;
	        fbreak;
        };
	
	'cl_device_max_compute_units'
	| 'device_max_compute_units'
	| 'max_compute_units'
	| 'deviceMaxComputeUnits'
	| 'maxComputeUnits'
	=> {
                *out_major = LEX_PARAM_UINT;
                out_minor->device_param_code = CL_DEVICE_MAX_COMPUTE_UNITS;
	        fbreak;
        };
	
	'cl_device_max_constant_args'
	| 'device_max_constant_args'
	| 'max_constant_args'
	| 'deviceMaxConstantArgs'
	| 'maxConstantArgs'
	=> {
                *out_major = LEX_PARAM_UINT;
                out_minor->device_param_code = CL_DEVICE_MAX_CONSTANT_ARGS;
	        fbreak;
        };
	
	'cl_device_max_read_image_args'
	| 'device_max_read_image_args'
	| 'max_read_image_args'
	| 'deviceMaxReadImageArgs'
	| 'maxReadImageArgs'
	=> {
                *out_major = LEX_PARAM_UINT;
                out_minor->device_param_code = CL_DEVICE_MAX_READ_IMAGE_ARGS;
	        fbreak;
        };
	
	'cl_device_global_mem_size'
	| 'device_global_mem_size'
	| 'global_mem_size'
	| 'deviceGlobalMemSize'
	| 'globalMemSize'
	=> {
                *out_major = LEX_PARAM_UINT;
                out_minor->device_param_code = CL_DEVICE_GLOBAL_MEM_SIZE;
	        fbreak;
        };
	
        # Parameters returning cl_uint.

	'cl_device_local_mem_size'
	| 'device_local_mem_size'
	| 'local_mem_size'
	| 'deviceLocalMemSize'
	| 'localMemSize'
	=> {
                *out_major = LEX_PARAM_ULONG;
                out_minor->device_param_code = CL_DEVICE_LOCAL_MEM_SIZE;
	        fbreak;
        };
	
	'cl_device_max_constant_buffer_size'
	| 'device_max_constant_buffer_size'
	| 'max_constant_buffer_size'
	| 'deviceMaxConstantBufferSize'
	| 'maxConstantBufferSize'
	=> {
                *out_major = LEX_PARAM_ULONG;
                out_minor->device_param_code = CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE;
	        fbreak;
        };
	
	'cl_device_max_mem_alloc_size'
	| 'device_max_mem_alloc_size'
	| 'max_mem_alloc_size'
	| 'deviceMaxMemAllocSize'
	| 'maxMemAllocSize'
	=> {
                *out_major = LEX_PARAM_ULONG;
                out_minor->device_param_code = CL_DEVICE_MAX_MEM_ALLOC_SIZE;
	        fbreak;
        };
	
	'cl_device_global_mem_cache_size'
	| 'device_global_mem_cache_size'
	| 'global_mem_cache_size'
	| 'deviceGlobalMemCacheSize'
	| 'globalMemCacheSize'
	=> {
                *out_major = LEX_PARAM_ULONG;
                out_minor->device_param_code = CL_DEVICE_GLOBAL_MEM_CACHE_SIZE;
	        fbreak;
        };

	# Parameters returning size_t.

	# Parameters returning string.

    *|;

    write exec;

    }%%

    if (*out_major == 0) {
        if (p == pe)
	    lex->is_eof = 1;
	else
	    lex->is_error = 1;
	    
        return 0;    
    }

    /* No need to save 'pe' here. */
    lex->current_state       = cs;
    lex->current_position    = p;
    lex->current_token_start = ts;
    lex->current_token_end   = te;
    lex->current_action      = act;
    return 1;
}

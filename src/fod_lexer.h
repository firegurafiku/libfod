#ifndef FOD_LEXER_H_
#define FOD_LEXER_H_

#include "fod_common.h"

enum fod_lexeme_major {
    LEX_EOF,
    LEX_PAREN_LEFT,
    LEX_PAREN_RIGHT,
    LEX_OPERATOR_NOT,
    LEX_OPERATOR_AND,
    LEX_OPERATOR_OR,
    LEX_OPERATOR_EQ,
    LEX_OPERATOR_NE,
    LEX_OPERATOR_LT,
    LEX_OPERATOR_GT,
    LEX_OPERATOR_LE,
    LEX_OPERATOR_GE,
    LEX_LITERAL_NUMBER,
    LEX_LITERAL_STRING,
    LEX_PARAM_UINT,
    LEX_PARAM_ULONG,
    LEX_PARAM_SIZE_T,
    LEX_PARAM_BOOL,
    LEX_PARAM_FP_CONFIG,
    LEX_PARAM_STRING
};

struct fod_lexeme_minor {
    fod_longest_uint    literal_number;
    char               *literal_string;
    cl_device_info      device_param_code;
    cl_platform_info    platform_param_code;
    cl_device_fp_config fp_config_mask;
};

struct fod_lexer {

    /* Memory reallocator and its closure argument. */
    fod_reallocator realloc;
    void           *realloc_arg;
    
    /* Original input string. */
    char const *input;
    char const *input_end;

    /* Flags telling why lexer has stopped. */
    int is_eof;
    int is_error;

    /* Lexer state kept between 'fod_lexer_tokenize' calls. */
    char const *current_position;
    char const *current_token_start;
    char const *current_token_end;
    int current_state;
    int current_action;
};

void fod_lexer_reset(
        struct fod_lexer *lex,
        char const *input,
        fod_reallocator realloc,
        void *realloc_arg);

int fod_lexer_tokenize(
        struct fod_lexer        *lex,
        enum fod_lexeme_major   *out_major,
        struct fod_lexeme_minor *out_minor);


#endif

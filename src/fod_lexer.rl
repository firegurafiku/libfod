#include "fod_common.h"
#include "fod_tokens.h"
#include "fod_parser.h"
#include "fod_lexer.h"
#include "string.h"
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
        struct fod_lexer *lex,
	int              *out_major,
	union fod_token  *out_minor) {
    
    /* Don't use ragel's 'write init;' construct. Instead, perform manual
       state restoration. */
    char const *p   = lex->current_position;
    char const *pe  = lex->input_end;
    char const *ts  = lex->current_token_start;
    char const *te  = lex->current_token_end;
    char const *eof = pe;
    
    int cs  = lex->current_state;
    int act = lex->current_action;
    
    if (p >= pe) {
        lex->is_eof = 1;
        return 0;
    }

    /* In Lemon, valid token major code is never zero, 'cause zero
       means 'EOF'. Let's return it in case of errors. */
    *out_major = 0;
    
    %%{
    number = digit+;
    single_quoted_string = "'" ([^']|"'")* "'";
    double_quoted_string = '"' ([^"]|'"')* '"';
    main := |*
        # Skip any whitespaces which are not in string literal.
        space+ => { printf("SPACES\n"); };

        # Parse 
        '=='  => { *out_major = TOK_OPERATOR_EQ; printf("==\n"); fbreak; };
        '!='  => { *out_major = TOK_OPERATOR_NE; printf("!=\n"); fbreak; };
        '<='  => { *out_major = TOK_OPERATOR_LE; printf("<=\n"); fbreak; };
        '>='  => { *out_major = TOK_OPERATOR_GE; printf(">=\n"); fbreak; };
        '<'   => { *out_major = TOK_OPERATOR_LT; printf("<\n");  fbreak; };
        '>'   => { *out_major = TOK_OPERATOR_GT; printf(">\n");  fbreak; };
        '('   => { *out_major = TOK_PAREN_LEFT;  printf("(\n");  fbreak; };
        ')'   => { *out_major = TOK_PAREN_RIGHT; printf(")\n");  fbreak; };

        # 
        '&&'  => { *out_major = TOK_OPERATOR_AND; printf("&&\n"); fbreak; };
        '||'  => { *out_major = TOK_OPERATOR_OR;  printf("||\n"); fbreak; };
        '!'   => { *out_major = TOK_OPERATOR_NOT; printf("!\n");  fbreak; };

        # Unsigned integer literal.
        number => {
	    fod_longest_uint res;
	    if (!fod_substring_to_uint(&res, ts, te))
	       fbreak;
	    
            *out_major = TOK_LITERAL_UINT;
            out_minor->uint_literal_val = res;
            fbreak;
        };

        # single_quoted_string |
        # double_quoted_string => {
        #   *out_major = TOK_LITERAL_STR;
        #   out_minor->str_literal_val = fod_duplicate_string(ts, te);
        # fbreak;
        # };

        # 'cl_device_available' |
        # 'device_available'    |
        # 'deviceAvailable'     |   
        # 'available' => {
        #     *out_major = TOK_PARAM_BOOL;
        #       out_minor->device_param_code = CL_DEVICE_AVAILABLE;
        # };

    *|;

    write exec;

    }%%

    if (*out_major == 0) {
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

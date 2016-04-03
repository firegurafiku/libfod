#include "fod_tokens.h"
#include "fod_parser.h"
#include "fod_lexer.h"

void fod_lexer_init(struct fod_lexer *out_lex, char const *input) {
    
}

void fod_lexer_close(struct fod_lexer *lex) {

}

int fod_lexer_tokenize(struct fod_lexer *lex, int *out_major, union fod_token *out_minor) {
    %% machine fod_lexer;
    %% write data;
    
    /* Don't use ragel's 'write init;' construct. Instead, perform manual
       state restoration. */
    char const *p   = lex->current_pos;
    char const *pe  = lex->input_end;
    char const *ts  = lex->current_token_start;
    char const *te  = lex->current_token_end;
    char const *eof = pe;
    
    int cs  = lex->current_state;
    int act = lex->current_action;

    int error = 0;
    
    if (p >= pe) {
        return 0;
    }

    *out_major = 0;
    
    %%{

    number = digit+;
    single_quoted_string = "'" ([^']|"'")* "'";
    double_quoted_string = '"' ([^"]|'"')* '"';
	
    main := |*
        # Skip any whitespaces which are not in string literal.
        space+ => {};

        # Parse 
        '=='  => { *out_major = TOK_OPERATOR_EQ; fbreak; };
        '!='  => { *out_major = TOK_OPERATOR_NE; fbreak; };
        '<='  => { *out_major = TOK_OPERATOR_LE; fbreak; };
        '>='  => { *out_major = TOK_OPERATOR_GE; fbreak; };
        '<'   => { *out_major = TOK_OPERATOR_LT; fbreak; };
        '>'   => { *out_major = TOK_OPERATOR_GT; fbreak; };
        '('   => { *out_major = TOK_PAREN_LEFT;  fbreak; };
        ')'   => { *out_major = TOK_PAREN_RIGHT; fbreak; };

        # 
        '&&'  => { *out_major = TOK_OPERATOR_AND; fbreak; };
        '||'  => { *out_major = TOK_OPERATOR_OR;  fbreak; };
        '!'   => { *out_major = TOK_OPERATOR_NOT; fbreak; };

        # Unsigned integer literal.
        number => {
        *out_major = TOK_LITERAL_UINT;
        out_minor->uint_literal_val = fod_strtoul(ts, te);
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

    if (*out_major == 0)
        return 0;

    /* No need to save 'pe' here. */
    lex->current_state       = cs;
    lex->current_pos         = p;
    lex->current_token_start = ts;
    lex->current_token_end   = te;
    lex->current_action      = act;
    return 1;
}

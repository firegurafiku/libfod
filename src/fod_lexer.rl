%%{
machine fod_lexer;

# Describe what is 'ts', 'te', 'alloc' and 'reclaim'

main := |*

    # Skip any whitespaces which are not in string literal.
    space => {};

    # Parse 
    '=='  => { parse(TOK_OPERATOR_EQ);  };
    '!='  => { parse(TOK_OPERATOR_NE);  };
    '<='  => { parse(TOK_OPERATOR_LE);  };
    '>='  => { parse(TOK_OPERATOR_GE);  };
    '<'   => { parse(TOK_OPERATOR_LT);  };
    '>'   => { parse(TOK_OPERATOR_GT);  };
    '('   => { parse(TOK_PAREN_LEFT); };
    ')'   => { parse(TOK_PAREN_RIGHT); };

    # 
    '&&'  => { parse(TOK_OPERATOR_AND); };
    '||'  => { parse(TOK_OPERATOR_OR);  };
    '!'   => { param(TOK_OPERATOR_NOT); };

    # Unsigned integer literal.
    digit+ => { parse(TOK_LITERAL_UINT, atol(ts, te); };
    
    '"' ([^"]|'"')* '"' => {
        parse(TOK_LITERAL_STR, fod_duplicate_string(ts, te));
    };
    
    "'" ([^']|"'")* "'" => {
        parse(TOK_LITERAL_STR, fod_duplicate_string(alloc, ts, te));
    };

    'cl_device_available'
    | 'device_available'
    | 'deviceAvailable'
    | 'available'
    => { parse(TOK_PARAM_BOOL, CL_DEVICE_AVAILABLE); };
*|;
}%%

#include "fod_parser_impl.h"

// alloc
// reclaim
void fod_lexer(char const *input, void (*parse)()) {
    %% write data;
    %% write init;

    char const *p = input;
    char const *pe = data + len;
    char *eof = pe;
    %% write exec;
}
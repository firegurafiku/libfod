%name querylang_parser
%start_symbol main

//%extra_argument {void (*reclaim)(void *)}
%include{
    #include "fod_parser_impl.h"
}

%left TOK_OPERATOR_OR.
%left TOK_OPERATOR_AND.
%left TOK_OPERATOR_EQ
      TOK_OPERATOR_NE
      TOK_OPERATOR_LE
      TOK_OPERATOR_GE
      TOK_OPERATOR_LT
      TOK_OPERATOR_GT.
%left TOK_OPERATOR_NOT.

%token_type {Token}

//%type TOK_PARAM_UINT   {cl_device_info}
//%type TOK_LITERAL_UINT {fod_longest_uint}
//%type TOK_LITERAL_STR  {char *}

// Awesome feature!
// %token_destructor { reclaim($$); }

%type main {int}
main(R) ::= expression(A). { R = A; } 

%type expression {int}
expression(R) ::= comparison(A).                                { R = A; }
expression(R) ::= TOK_PAREN_LEFT expression(A) TOK_PAREN_RIGHT. { R = A; }
expression(R) ::= TOK_OPERATOR_NOT expression(A).               { R = !A; }
expression(R) ::= expression(A) TOK_OPERATOR_AND expression(B). { R = A && B; }
expression(R) ::= expression(A) TOK_OPERATOR_OR  expression(B). { R = A || B; }

%type operator_uint {enum comparison}
operator_uint(R) ::= TOK_OPERATOR_EQ. { R = COMPARISON_EQ; }
operator_uint(R) ::= TOK_OPERATOR_NE. { R = COMPARISON_NE; }
operator_uint(R) ::= TOK_OPERATOR_LE. { R = COMPARISON_LE; }
operator_uint(R) ::= TOK_OPERATOR_GE. { R = COMPARISON_GE; }
operator_uint(R) ::= TOK_OPERATOR_LT. { R = COMPARISON_LT; }
operator_uint(R) ::= TOK_OPERATOR_GT. { R = COMPARISON_GT; }

%type comparison {int}
comparison(R) ::= PARAM_UINT(A) operator_uint(X) PARAM_UINT(B). {
    R = fod_compare_uint_pp(X, A.device_param_code, B.device_param_code);
}

comparison(R) ::= PARAM_UINT(A) operator_uint(X) LITERAL_UINT(B). {
    R = fod_compare_uint_pl(X, A.device_param_code, B.uint_literal_val);
}

comparison(R) ::= LITERAL_UINT(A) operator_uint(X) PARAM_UINT(B). {
    R = fod_compare_uint_pl(inverse_comparison(X),
			    B.device_param_code, A.uint_literal_val);
}

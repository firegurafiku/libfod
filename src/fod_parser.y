%name fod_parser
%start_symbol main
%stack_size 20

%include{
    #include "fod_tokens.h"
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

%token_type {union fod_token}

%type main {int}
main(R) ::= expression(A). { R = A; }

%type expression {int}
expression(R) ::= comparison(A).                                { R = A; }
expression(R) ::= TOK_PAREN_LEFT expression(A) TOK_PAREN_RIGHT. { R = A; }
expression(R) ::= TOK_OPERATOR_NOT expression(A).               { R = !A; }
expression(R) ::= expression(A) TOK_OPERATOR_AND expression(B). { R = A && B; }
expression(R) ::= expression(A) TOK_OPERATOR_OR  expression(B). { R = A || B; }

%type comparison {int}
comparison(R) ::= TOK_BOOLEAN(A). { R = A.boolean; }
comparison(R) ::= TOK_NUMBER(A) TOK_OPERATOR_EQ TOK_NUMBER(B). { R = (A.number == B.number); }
comparison(R) ::= TOK_NUMBER(A) TOK_OPERATOR_NE TOK_NUMBER(B). { R = (A.number != B.number); }
comparison(R) ::= TOK_NUMBER(A) TOK_OPERATOR_LT TOK_NUMBER(B). { R = (A.number <  B.number); }
comparison(R) ::= TOK_NUMBER(A) TOK_OPERATOR_GT TOK_NUMBER(B). { R = (A.number >  B.number); }
comparison(R) ::= TOK_NUMBER(A) TOK_OPERATOR_LE TOK_NUMBER(B). { R = (A.number <= B.number); }
comparison(R) ::= TOK_NUMBER(A) TOK_OPERATOR_GE TOK_NUMBER(B). { R = (A.number >= B.number); }
comparison(R) ::= TOK_STRING(A) TOK_OPERATOR_EQ TOK_STRING(B). { R = strcmp(A.string, B.string) == 0; }
comparison(R) ::= TOK_STRING(A) TOK_OPERATOR_NE TOK_STRING(B). { R = strcmp(A.string, B.string) != 0; }

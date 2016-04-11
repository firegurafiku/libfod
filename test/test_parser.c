#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "fod_common.h"
#include "fod_parser.h"

extern void test_parser_valid1(void **st) {
    (void)st;

    // 1 <= 5 && "aa" == "aa"
    int maj;
    union fod_token min;
    
    struct fod_parser parser;
    
    fod_parser_init(&parser, fod_std_realloc, NULL);
    parser.destruct_tokens = 1;

    maj = TOK_NUMBER;
    min.number = 1;
    fod_parser_consume_token(&parser, maj, &min);
    assert_false(parser.is_error);
    
    maj = TOK_OPERATOR_LE;
    fod_parser_consume_token(&parser, maj, &min);
    assert_false(parser.is_error);
    
    maj = TOK_NUMBER;
    min.number = 5;
    fod_parser_consume_token(&parser, maj, &min);
    assert_false(parser.is_error);
    
    maj = TOK_OPERATOR_AND;
    fod_parser_consume_token(&parser, maj, &min);
    assert_false(parser.is_error);
    
    maj = TOK_STRING;
    min.string = fod_strdup("aa", fod_std_realloc, NULL);
    fod_parser_consume_token(&parser, maj, &min);
    assert_false(parser.is_error);
    
    maj = TOK_OPERATOR_EQ;
    fod_parser_consume_token(&parser, maj, &min);
    assert_false(parser.is_error);

    maj = TOK_STRING;
    min.string = fod_strdup("aa", fod_std_realloc, NULL);
    fod_parser_consume_token(&parser, maj, &min);
    assert_false(parser.is_error);

    fod_parser_consume_token(&parser, 0, &min);
    assert_false(parser.is_error);
    assert_true(parser.result); // Expression evaluates to true!

    fod_parser_close(&parser);
}

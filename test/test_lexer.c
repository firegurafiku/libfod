#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fod_lexer.h"

extern void test_lexer_valid1(void **st) {

    struct fod_lexer lex;
    fod_lexer_init(&lex, "   ((!!=&&||>>===<<===  ", NULL, NULL);

    int expected[] = {
	TOK_PAREN_LEFT,   TOK_PAREN_LEFT,  TOK_OPERATOR_NOT, TOK_OPERATOR_NE,
	TOK_OPERATOR_AND, TOK_OPERATOR_OR, TOK_OPERATOR_GT,  TOK_OPERATOR_GE,
	TOK_OPERATOR_EQ,  TOK_OPERATOR_LT,  TOK_OPERATOR_LE, TOK_OPERATOR_EQ };

    int n = sizeof(expected)/sizeof(*expected);
    int res;
    int maj;
    union fod_token min;
    
    int i;
    for (i = 0; i < n; ++i) {
        res = fod_lexer_tokenize(&lex, &maj, &min);
        assert_true(res);
	assert_false(lex.is_eof);
	assert_false(lex.is_error);
        assert_int_equal(maj, expected[i]);
    }
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_false(res);
    assert_true(lex.is_eof);
    assert_false(lex.is_error);
}

extern void test_lexer_valid2(void **st) {

    struct fod_lexer lex;
    fod_lexer_init(&lex, "12345 1234567890123456789", NULL, NULL);

    int res;
    int maj;
    union fod_token min;
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, TOK_LITERAL_UINT);
    assert_true(min.uint_literal_val == 12345u);
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, TOK_LITERAL_UINT);
    assert_true(min.uint_literal_val == 1234567890123456789);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_false(res);
    assert_true(lex.is_eof);
    assert_false(lex.is_error);
}

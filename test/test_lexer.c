#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fod_common.h"
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

extern void test_lexer_valid3(void **st) {

    char const *expression =
	"''   '\\\"'   \"'\"    \"\\\"\"   \n"
	"\"\\\"they're\\\" is not \\\"their\\\"\"  \f\v"
	"\t '[\\n\\t\\f\\v\\a\\b\\0\\r]'  ";
	
    struct fod_lexer lex;
    fod_lexer_init(&lex, expression, fod_std_realloc, NULL);

    char const *expected[] = {
        "",
	"\"",
	"'",
	"\"",
	"\"they're\" is not \"their\"",
	"[\n\t\f\v\a\b\0\r]"
    };

    int n = sizeof(expected)/sizeof(*expected);
    int i;
    int res;
    int maj;
    union fod_token min;

    for (i=0; i<n; ++i) {
	res = fod_lexer_tokenize(&lex, &maj, &min);
	assert_true(res);
	assert_false(lex.is_eof);
	assert_false(lex.is_error);
	assert_int_equal(maj, TOK_LITERAL_STR);
	assert_non_null(min.uint_literal_val);
	assert_string_equal(min.uint_literal_val, expected[i]);

	/* Free memory before it's track is lost. */
	fod_std_realloc(min.uint_literal_val, 0, NULL);
    }
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_false(res);
    assert_true(lex.is_eof);
    assert_false(lex.is_error);

    fod_lexer_close(&lex);
}

extern void test_lexer_valid_identifiers(void **st) {

    char const *expression = " !device_available && deviceAvailable  ";

    struct fod_lexer lex;
    fod_lexer_init(&lex, expression, fod_std_realloc, NULL);

    int res;
    int maj;
    union fod_token min;

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, TOK_OPERATOR_NOT);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, TOK_PARAM_BOOL);
    assert_int_equal(min.device_param_code, CL_DEVICE_AVAILABLE);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, TOK_OPERATOR_AND);
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, TOK_PARAM_BOOL);
    assert_int_equal(min.device_param_code, CL_DEVICE_AVAILABLE);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_false(res);
    assert_true(lex.is_eof);
    assert_false(lex.is_error);

    fod_lexer_close(&lex);
}

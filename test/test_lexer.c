#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fod_common.h"
#include "fod_lexer.h"

extern void test_lexer_valid1(void **st) {
    (void)st;

    struct fod_lexer lex;
    fod_lexer_init(&lex, "   ((!!=&&||>>===<<===  ", NULL, NULL);

    int expected[] = {
	LEX_PAREN_LEFT,   LEX_PAREN_LEFT,  LEX_OPERATOR_NOT, LEX_OPERATOR_NE,
	LEX_OPERATOR_AND, LEX_OPERATOR_OR, LEX_OPERATOR_GT,  LEX_OPERATOR_GE,
	LEX_OPERATOR_EQ,  LEX_OPERATOR_LT,  LEX_OPERATOR_LE, LEX_OPERATOR_EQ };

    int n = sizeof(expected)/sizeof(*expected);
    int res;
    enum fod_lexeme_major maj;
    struct fod_lexeme_minor min;
    
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
    (void)st;

    struct fod_lexer lex;
    fod_lexer_init(&lex, "12345 1234567890123456789", NULL, NULL);

    int res;
    enum fod_lexeme_major maj;
    struct fod_lexeme_minor min;
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, LEX_LITERAL_NUMBER);
    assert_true(min.literal_number == 12345u);
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, LEX_LITERAL_NUMBER);
    assert_true(min.literal_number == 1234567890123456789);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_false(res);
    assert_true(lex.is_eof);
    assert_false(lex.is_error);
}

extern void test_lexer_valid3(void **st) {
    (void)st;

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
    enum fod_lexeme_major maj;
    struct fod_lexeme_minor min;

    for (i=0; i<n; ++i) {
	res = fod_lexer_tokenize(&lex, &maj, &min);
	assert_true(res);
	assert_false(lex.is_eof);
	assert_false(lex.is_error);
	assert_int_equal(maj, LEX_LITERAL_STRING);
	assert_non_null(min.literal_string);
	assert_string_equal(min.literal_string, expected[i]);

	/* Free memory before it's track is lost. */
	fod_std_realloc(min.literal_string, 0, NULL);
    }
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_false(res);
    assert_true(lex.is_eof);
    assert_false(lex.is_error);

    fod_lexer_close(&lex);
}

extern void test_lexer_valid_identifiers(void **st) {
    (void)st;
    
    char const *expression = " !device_available && deviceAvailable  ";

    struct fod_lexer lex;
    fod_lexer_init(&lex, expression, fod_std_realloc, NULL);

    int res;
    enum fod_lexeme_major maj;
    struct fod_lexeme_minor min;

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, LEX_OPERATOR_NOT);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, LEX_PARAM_BOOL);
    assert_int_equal(min.device_param_code, CL_DEVICE_AVAILABLE);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, LEX_OPERATOR_AND);
    
    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_true(res);
    assert_false(lex.is_eof);
    assert_false(lex.is_error);
    assert_int_equal(maj, LEX_PARAM_BOOL);
    assert_int_equal(min.device_param_code, CL_DEVICE_AVAILABLE);

    res = fod_lexer_tokenize(&lex, &maj, &min);
    assert_false(res);
    assert_true(lex.is_eof);
    assert_false(lex.is_error);

    fod_lexer_close(&lex);
}

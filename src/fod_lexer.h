#ifndef FOD_LEXER_H_
#define FOD_LEXER_H_

#include "fod_common.h"
#include "fod_tokens.h"

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

void fod_lexer_init(
        struct fod_lexer *lex,
	char const       *input,
	fod_reallocator   realloc,
	void             *realloc_arg);

int fod_lexer_tokenize(
        struct fod_lexer *lex,
        int              *out_major,
        union fod_token  *out_minor);

void fod_lexer_close(struct fod_lexer *lex);

#endif

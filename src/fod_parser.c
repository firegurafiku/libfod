#include "fod_parser.h"

// thread them!
static fod_reallocator saved_realloc;
static void *saved_arg;

static void *kinda_malloc(size_t n) {
    return saved_realloc(NULL, n, saved_arg);
}

static void kinda_free(void *p) {
    saved_realloc(p, 0, saved_arg);
}

void *fod_lemon_parser_Alloc(void *(*malloc)(size_t));
void  fod_lemon_parser_Free(void *yyp, void (*free)(void *));
void  fod_lemon_parser_(void *yyp, int yymajor, union fod_token yyminor, struct fod_parser *state);

int fod_parser_init(
        struct fod_parser *out,
        fod_reallocator    realloc,
        void              *realloc_arg) {

    void *p = NULL;
    saved_realloc = realloc;
    saved_arg     = realloc_arg;

    p = fod_lemon_parser_Alloc(kinda_malloc);

    out->realloc = realloc;
    out->realloc_arg = realloc_arg;
    out->lemon_parser = p;
    out->result = 0;
    out->is_error = 0;
    return (p != NULL);
}

void fod_parser_close(struct fod_parser *parser) {

    fod_lemon_parser_Free(parser->lemon_parser, kinda_free);
    parser->lemon_parser = NULL;
}

void fod_parser_consume_token(
	struct fod_parser *parser,
	int major,
	union fod_token const *minor) {

    // Copy arg!
    fod_lemon_parser_(parser->lemon_parser, major, *minor, parser);
}

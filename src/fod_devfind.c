#include "fod_common.h"
#include "fod_lexer.h"
#include "fod_parser.h"
#include "fod_evaltoken.h"
#include "fod_devfind.h"
#include "find_opencl_devices.h"

int find_opencl_device(
        cl_platform_id *out_platform,
        cl_device_id   *out_device,
        char const     *predicate_expr) {

    return find_opencl_devices(out_platform, out_device, 1,
                          NULL, predicate_expr);
}

int find_opencl_device_ext(
        cl_platform_id *out_platform,
        cl_device_id   *out_device,
        char const     *predicate_expr,
        void           *(*realloc)(void *p, size_t n, void* arg),
        void           *realloc_arg) {

    return find_opencl_devices_ext(out_platform, out_device, 1, NULL,
               predicate_expr, realloc, realloc_arg);
}

int find_opencl_devices(
        cl_platform_id *target_buf_platforms,
        cl_device_id   *target_buf_devices,
        size_t          target_buf_count,
        size_t         *out_actual_count,
        char const     *predicate_expr) {

    return find_opencl_devices_ext(
               target_buf_platforms,
               target_buf_devices,
               target_buf_count,
               out_actual_count,
               predicate_expr,
               fod_std_realloc,
               NULL);
}

int find_opencl_devices_ext(
        cl_platform_id *target_buf_platforms,
        cl_device_id   *target_buf_devices,
        size_t          target_buf_count,
        size_t         *out_actual_count,
        char const     *predicate_expr,
        void           *(*realloc)(void *p, size_t n, void *arg),
        void           *realloc_arg) {

    return fod_find_devices(
               target_buf_platforms,
               target_buf_devices,
               target_buf_count,
               out_actual_count,
               predicate_expr,
               realloc,
               realloc_arg,
               fod_std_opencl());
}

int fod_find_devices(
        cl_platform_id    *target_buf_platforms,
        cl_device_id      *target_buf_devices,
        size_t             target_buf_count,
        size_t            *out_actual_count,
        char const        *predicate_expr,
        fod_reallocator    realloc,
        void              *realloc_arg,
        struct fod_opencl *cl) {

    size_t ilex = 0;
    size_t iplat = 0;
    size_t idev = 0;
    size_t ifound = 0;

    int retval = 0;
    int ok = 0;

    size_t lexemes_count = 0;
    struct fod_lexer lex;
    enum fod_lexeme_major lexmaj;
    struct fod_lexeme_minor lexmin;
    enum fod_lexeme_major *lexemes_maj = NULL;
    struct fod_lexeme_minor *lexemes_min = NULL;

    struct fod_parser parser;
    int tokmaj;
    union fod_token tokmin;

    cl_platform_id *platforms = NULL;
    cl_device_id   *devices   = NULL;
    cl_uint platforms_count = 0;
    cl_uint max_devices_count = 0;
    cl_int clerror = CL_SUCCESS;

    /* Before doing actual OpenCL queries, check that 'predicate_expr' can be
       tokenized and parsed to ensure its syntax is correct. Also count the
       number of lexemes the expression consists of.

       Note that running lexer with 'realloc' set to NULL means "do not
       allocate string literals; return NULL instead". */

    fod_lexer_reset(&lex, predicate_expr, NULL, NULL);
    ok = fod_parser_init(&parser, realloc, realloc_arg);
    if (!ok)
        goto e_failure;

    parser.destruct_tokens = 0;

    lexemes_count = 0;
    while (fod_lexer_tokenize(&lex, &lexmaj, &lexmin)) {
        fod_evaluate_lexeme_to_token_noaction(lexmaj, &lexmin, &tokmaj, &tokmin);
        fod_parser_consume_token(&parser, tokmaj, &tokmin);
        if (parser.is_error)
            goto e_failure;

        ++lexemes_count;
    }

    fod_parser_consume_token(&parser, 0, &tokmin);
    if (parser.is_error)
        goto e_failure;

    fod_parser_close(&parser);

    if (lex.is_error)
        goto e_failure;

    /* Because the total number of lexemes is already known, allocate enough
       space to store all of them and do tokenization only once for all
       platforms and devices being checked. */

    lexemes_maj = realloc(NULL, sizeof(*lexemes_maj)*lexemes_count, realloc_arg);
    if (lexemes_maj == NULL)
        goto e_failure;

    lexemes_min = realloc(NULL, sizeof(*lexemes_min)*lexemes_count, realloc_arg);
    if (lexemes_min == NULL)
        goto e_failure;

    ilex = 0;
    fod_lexer_reset(&lex, predicate_expr, realloc, realloc_arg);
    while (fod_lexer_tokenize(&lex, &lexmaj, &lexmin)) {
        lexemes_maj[ilex] = lexmaj;
        lexemes_min[ilex] = lexmin;
        ++ilex;
    }

    /* Now, determine the number of platforms, and the number of devices on
       the most populated platform. Then allocate enough memory for them. */

    clerror = cl->get_platform_ids(0, NULL, &platforms_count);
    if (clerror != CL_SUCCESS)
        goto e_failure;

    platforms = realloc(NULL, sizeof(*platforms) * platforms_count, realloc_arg);
    if (platforms == NULL)
        goto e_failure;

    clerror = cl->get_platform_ids(platforms_count, platforms, &platforms_count);
    if (clerror != CL_SUCCESS)
        goto e_failure;

    max_devices_count = 0;
    for (iplat = 0; iplat < platforms_count; ++iplat) {
        cl_uint x;
        clerror = cl->get_device_ids(
                platforms[iplat], CL_DEVICE_TYPE_ALL, 0, NULL, &x);
        if (clerror != CL_SUCCESS)
            goto e_failure;

        if (x > max_devices_count)
            max_devices_count = x;
    }

    devices = realloc(NULL, sizeof(*devices) * max_devices_count, realloc_arg);
    if (devices == NULL)
        goto e_failure;

    /* After all preparation steps are done, enumerate all platforms and their
       devices, checking if they satisfy the predicate. */

    for (iplat = 0; iplat < platforms_count; ++iplat) {
        cl_uint devices_count = 0;

        clerror = cl->get_device_ids(
                platforms[iplat], CL_DEVICE_TYPE_ALL,
                max_devices_count, devices, &devices_count);
        if (clerror != CL_SUCCESS)
            goto e_failure;

        for (idev = 0; idev < devices_count; ++idev) {
            int match = 0;

            ok = fod_parser_init(&parser, realloc, realloc_arg);
            if (!ok)
                goto e_failure;

            parser.destruct_tokens = 1;
            for (ilex=0; ilex < lexemes_count; ++ilex) {
                ok = fod_evaluate_lexeme_to_token(
                         realloc, realloc_arg, cl,
                         platforms[iplat], devices[idev],
                         lexemes_maj[ilex], &lexemes_min[ilex],
                         &tokmaj, &tokmin);
                if (!ok)
                    goto e_failure;

                fod_parser_consume_token(&parser, tokmaj, &tokmin);
                if (parser.is_error)
                    goto e_failure;
            }

            fod_parser_consume_token(&parser, 0, &tokmin);
            if (parser.is_error)
                goto e_failure;

            match = parser.result;
            fod_parser_close(&parser);

            if (match) {
                if (target_buf_platforms && target_buf_devices) {
                    if (ifound < target_buf_count) {
                        target_buf_platforms[ifound] = platforms[iplat];
                        target_buf_devices[ifound]   = devices[idev];
                    }

                    if (ifound + 1 < target_buf_count)
                        goto e_target_filled;

                }

                ++ifound;
            }
        }
    }

    retval = 1;

e_target_filled:

    if (out_actual_count)
        *out_actual_count = ifound;

e_failure:

    fod_parser_close(&parser);

    if (lexemes_maj && lexemes_min) {
        for (ilex = 0; ilex < lexemes_count; ++ilex) {
            fod_free_lexeme(
                    lexemes_maj[ilex], &lexemes_min[ilex], realloc,
                    realloc_arg);
        }
    }

    realloc(lexemes_maj, 0, realloc_arg);
    realloc(lexemes_min, 0, realloc_arg);
    realloc(platforms,   0, realloc_arg);
    realloc(devices,     0, realloc_arg);

    return retval;
}

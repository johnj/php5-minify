/* jsmin.c
   2011-09-30

Copyright (c) 2002 Douglas Crockford  (www.crockford.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
+----------------------------------------------------------------------+
| See the LICENSE file for further copyright information               |
+----------------------------------------------------------------------+
| Authors: John Jawed <jawed@php.net>                                  |
| this extension is not thread safe, yet                               |
+----------------------------------------------------------------------+
*/

#include "jsmin.h"


/* jsmin_isAlphanum -- return true if the character is a letter, digit, underscore,
        dollar sign, or non-ASCII character.
*/

static int
jsmin_isAlphanum(int c)
{
    return ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
        (c >= 'A' && c <= 'Z') || c == '_' || c == '$' || c == '\\' ||
        c > 126);
}


/* jsmin_get -- return the jsmin_next character from stdin. Watch out for lookahead. If
        the character is a control character, translate it to a space or
        linefeed.
*/

static int
jsmin_get(php_jsmin_ctx_t *ctx)
{
    int c = ctx->theLookahead;
    ctx->theLookahead = 0;

    if (c == 0) {
        c = ctx->jsmin_in[ctx->jsmin_in_c++];
    }
    if (c >= ' ' || c == '\n' || c == '\0') {
        return c;
    }
    if (c == '\r') {
        return '\n';
    }
    return ' ';
}


/* jsmin_peek -- jsmin_get the jsmin_next character without jsmin_getting it.
*/

static int
jsmin_peek(php_jsmin_ctx_t *ctx)
{
    ctx->theLookahead = jsmin_get(ctx);
    return ctx->theLookahead;
}


/* jsmin_next -- jsmin_get the next character, excluding comments. jsmin_peek(ctx) is used to see
        if a '/' is followed by a '/' or '*'.
*/

static int
jsmin_next(php_jsmin_ctx_t *ctx)
{
    int c = jsmin_get(ctx);
    if  (c == '/') {
        switch (jsmin_peek(ctx)) {
        case '/':
            for (;;) {
                c = jsmin_get(ctx);
                if (c <= '\n') {
                    return c;
                }
            }
        case '*':
            jsmin_get(ctx);
            for (;;) {
                switch (jsmin_get(ctx)) {
                case '*':
                    if (jsmin_peek(ctx) == '/') {
                        jsmin_get(ctx);
                        return ' ';
                    }
                    break;
                case '\0':
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript comment.");
                    ctx->lastError = 2;
                    return 0;
                }
            }
        default:
            return c;
        }
    }
    return c;
}


/* jsmin_action -- do something! What you do is determined by the argument:
        1   Output A. Copy B to A. Get the jsmin_next B.
        2   Copy B to A. Get the jsmin_next B. (Delete A).
        3   Get the jsmin_next B. (Delete B).
   jsmin_action treats a string as a single character. Wow!
   jsmin_action recognizes a regular expression if it is preceded by ( or , or =.
*/

static void
jsmin_action(php_jsmin_ctx_t *ctx, int d, smart_str *jsm)
{
    switch (d) {
    case 1:
        smart_str_appendl(jsm, &ctx->theA, 1);
    case 2:
        ctx->theA = ctx->theB;
        if (ctx->theA == '\'' || ctx->theA == '"' || ctx->theA == '`') {
            for (;;) {
                smart_str_appendl(jsm, &ctx->theA, 1);
                ctx->theA = jsmin_get(ctx);
                if (ctx->theA == ctx->theB) {
                    break;
                }
                if (ctx->theA == '\\') {
                    smart_str_appendl(jsm, &ctx->theA, 1);
                    ctx->theA = jsmin_get(ctx);
                }
                if (ctx->theA == '\0') {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript string literal.");
                    ctx->lastError = 3;
                    return;
                }
            }
        }
    case 3:
        ctx->theB = jsmin_next(ctx);
        if (ctx->theB == '/' && (ctx->theA == '(' || ctx->theA == ',' || ctx->theA == '=' ||
                            ctx->theA == ':' || ctx->theA == '[' || ctx->theA == '!' ||
                            ctx->theA == '&' || ctx->theA == '|' || ctx->theA == '?' ||
                            ctx->theA == '{' || ctx->theA == '}' || ctx->theA == ';' ||
                            ctx->theA == '\n')) {
            smart_str_appendl(jsm, &ctx->theA, 1);
            smart_str_appendl(jsm, &ctx->theB, 1);
            for (;;) {
                ctx->theA = jsmin_get(ctx);
                if (ctx->theA == '[') {
                    for (;;) {
                        smart_str_appendl(jsm, &ctx->theA, 1);
                        ctx->theA = jsmin_get(ctx);
                        if (ctx->theA == ']') {
                            break;
                        }
                        if (ctx->theA == '\\') {
                            smart_str_appendl(jsm, &ctx->theA, 1);
                            ctx->theA = jsmin_get(ctx);
                        }
                        if (ctx->theA == '\0') {
                            php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript regular expression literal.");
                            ctx->lastError = 1;
                            return;
                        }
                    }
                } else if (ctx->theA == '/') {
                    break;
                } else if (ctx->theA =='\\') {
                    smart_str_appendl(jsm, &ctx->theA, 1);
                    ctx->theA = jsmin_get(ctx);
                }
                if (ctx->theA == '\0') {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript regular expression literal.");
                    ctx->lastError = 2;
                    return;
                }
                smart_str_appendl(jsm, &ctx->theA, 1);
            }
            ctx->theB = jsmin_next(ctx);
        }
    }
}

int jsmin_jsmin(php_jsmin_ctx_t *ctx, smart_str *jsm)
{
    ctx->theA = '\n';

    jsmin_action(ctx, 3, jsm);
    while (ctx->theA != '\0') {
        switch (ctx->theA) {
        case ' ':
            if (jsmin_isAlphanum(ctx->theB)) {
                jsmin_action(ctx, 1, jsm);
            } else {
                jsmin_action(ctx, 2, jsm);
            }
            break;
        case '\n':
            switch (ctx->theB) {
            case '{':
            case '[':
            case '(':
            case '+':
            case '-':
                jsmin_action(ctx, 1, jsm);
                break;
            case ' ':
                jsmin_action(ctx, 3, jsm);
                break;
            default:
                if (jsmin_isAlphanum(ctx->theB)) {
                    jsmin_action(ctx, 1, jsm);
                } else {
                    jsmin_action(ctx, 2, jsm);
                }
            }
            break;
        default:
            switch (ctx->theB) {
            case ' ':
                if (jsmin_isAlphanum(ctx->theA)) {
                    jsmin_action(ctx, 1, jsm);
                    break;
                }
                jsmin_action(ctx, 3, jsm);
                break;
            case '\n':
                switch (ctx->theA) {
                case '}':
                case ']':
                case ')':
                case '+':
                case '-':
                case '"':
                case '\'':
                case '`':
                    jsmin_action(ctx, 1, jsm);
                    break;
                default:
                    if (jsmin_isAlphanum(ctx->theA)) {
                        jsmin_action(ctx, 1, jsm);
                    } else {
                        jsmin_action(ctx, 3, jsm);
                    }
                }
                break;
            default:
                jsmin_action(ctx, 1, jsm);
                break;
            }
        }
    }

    if(ctx->lastError) {
        return ctx->lastError;
    }

    return SUCCESS;
}

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

static int   theA;
static int   theB;
static int   theLookahead = EOF;
static char *jsmin_in = NULL;
static int   jsmin_in_c = 0;
static int   lastError = 0;

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
jsmin_get()
{
    int c = theLookahead;
    theLookahead = 0;

    if (c == 0) {
        c = jsmin_in[jsmin_in_c++];
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
jsmin_peek()
{
    theLookahead = jsmin_get();
    return theLookahead;
}


/* jsmin_next -- jsmin_get the next character, excluding comments. jsmin_peek() is used to see
        if a '/' is followed by a '/' or '*'.
*/

static int
jsmin_next()
{
    int c = jsmin_get();
    if  (c == '/') {
        switch (jsmin_peek()) {
        case '/':
            for (;;) {
                c = jsmin_get();
                if (c <= '\n') {
                    return c;
                }
            }
        case '*':
            jsmin_get();
            for (;;) {
                switch (jsmin_get()) {
                case '*':
                    if (jsmin_peek() == '/') {
                        jsmin_get();
                        return ' ';
                    }
                    break;
                case '\0':
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript comment.");
                    lastError = 2;
                    
                    return;
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
jsmin_action(int d, smart_str *jsm)
{
    switch (d) {
    case 1:
        smart_str_appendl(jsm, &theA, 1);
    case 2:
        theA = theB;
        if (theA == '\'' || theA == '"' || theA == '`') {
            for (;;) {
                smart_str_appendl(jsm, &theA, 1);
                theA = jsmin_get();
                if (theA == theB) {
                    break;
                }
                if (theA == '\\') {
                    smart_str_appendl(jsm, &theA, 1);
                    theA = jsmin_get();
                }
                if (theA == '\0') {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript string literal.");
                    lastError = 3;
                    return;
                }
            }
        }
    case 3:
        theB = jsmin_next();
        if (theB == '/' && (theA == '(' || theA == ',' || theA == '=' ||
                            theA == ':' || theA == '[' || theA == '!' ||
                            theA == '&' || theA == '|' || theA == '?' ||
                            theA == '{' || theA == '}' || theA == ';' ||
                            theA == '\n')) {
            smart_str_appendl(jsm, &theA, 1);
            smart_str_appendl(jsm, &theB, 1);
            for (;;) {
                theA = jsmin_get();
                if (theA == '[') {
                    for (;;) {
                        smart_str_appendl(jsm, &theA, 1);
                        theA = jsmin_get();
                        if (theA == ']') {
                            break;
                        }
                        if (theA == '\\') {
                            smart_str_appendl(jsm, &theA, 1);
                            theA = jsmin_get();
                        }
                        if (theA == '\0') {
                            php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript regular expression literal.");
                            lastError = 1;
                            return;
                        }
                    }
                } else if (theA == '/') {
                    break;
                } else if (theA =='\\') {
                    smart_str_appendl(jsm, &theA, 1);
                    theA = jsmin_get();
                }
                if (theA == '\0') {
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "php-minify: unterminated javascript regular expression literal.");
                    lastError = 2;
                    return;
                }
                smart_str_appendl(jsm, &theA, 1);
            }
            theB = jsmin_next();
        }
    }
}

int jsmin_jsmin(char *in, smart_str *jsm)
{
    theB = jsmin_in_c = lastError = theLookahead = 0;
    jsmin_in = in;
    theA = '\n';

    jsmin_action(3, jsm);
    while (theA != '\0') {
        switch (theA) {
        case ' ':
            if (jsmin_isAlphanum(theB)) {
                jsmin_action(1, jsm);
            } else {
                jsmin_action(2, jsm);
            }
            break;
        case '\n':
            switch (theB) {
            case '{':
            case '[':
            case '(':
            case '+':
            case '-':
                jsmin_action(1, jsm);
                break;
            case ' ':
                jsmin_action(3, jsm);
                break;
            default:
                if (jsmin_isAlphanum(theB)) {
                    jsmin_action(1, jsm);
                } else {
                    jsmin_action(2, jsm);
                }
            }
            break;
        default:
            switch (theB) {
            case ' ':
                if (jsmin_isAlphanum(theA)) {
                    jsmin_action(1, jsm);
                    break;
                }
                jsmin_action(3, jsm);
                break;
            case '\n':
                switch (theA) {
                case '}':
                case ']':
                case ')':
                case '+':
                case '-':
                case '"':
                case '\'':
                case '`':
                    jsmin_action(1, jsm);
                    break;
                default:
                    if (jsmin_isAlphanum(theA)) {
                        jsmin_action(1, jsm);
                    } else {
                        jsmin_action(3, jsm);
                    }
                }
                break;
            default:
                jsmin_action(1, jsm);
                break;
            }
        }
    }

    if(lastError) {
        return lastError;
    }

    return SUCCESS;
}

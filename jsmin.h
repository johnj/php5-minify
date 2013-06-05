/*
+----------------------------------------------------------------------+
| See the LICENSE file for further copyright information               |
+----------------------------------------------------------------------+
| Authors: John Jawed <jawed@php.net>                                  |
+----------------------------------------------------------------------+
*/

#ifndef _PHP_JSMIN_H_
#  define _PHP_JSMIN_H_

#include <stdlib.h>
#include "php.h"
#include "ext/standard/php_smart_str.h"

typedef struct _php_jsmin_ctx_t {
	int   theA;
	int   theB;
	int   theLookahead;
	char *jsmin_in;
	int   jsmin_in_c;
	int   lastError;
} php_jsmin_ctx_t;

int jsmin_jsmin(php_jsmin_ctx_t *ctx, smart_str *jsm);

#endif
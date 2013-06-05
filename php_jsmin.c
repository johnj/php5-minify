/*
+----------------------------------------------------------------------+
| See the LICENSE file for further copyright information               |
+----------------------------------------------------------------------+
| Authors: John Jawed <jawed@php.net>                                  |
+----------------------------------------------------------------------+
*/

#include "php.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/info.h"
#include "php_ini.h"
#include "jsmin.h"
#include "php_jsmin.h"
#include "minify.h"

static zend_class_entry *jsmin_class_entry;

JSMINIFY_METHOD(__construct) {
    return;
}

JSMINIFY_METHOD(minify) {

	char *js = NULL;
    int js_len = 0;
    smart_str out = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &js, &js_len) == FAILURE) {
        return;
    }

	if (js_len) {
		php_jsmin_ctx_t ctx = { 0, 0, EOF, js, 0, 0 };
	    INIT_SMART_STR(out);

	    if(jsmin_jsmin(&ctx, &out)==SUCCESS) {
	        ZVAL_STRINGL(return_value, out.c, out.len, 1);
	        smart_str_free(&out);
	        return;
	    }
	}
    ZVAL_BOOL(return_value, 0);
}

MINIFY_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_minify_jsmin__construct, 0, 0, 0)
ZEND_END_ARG_INFO()

MINIFY_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_minify_jsmin_minify, 0, 0, 0)
ZEND_ARG_INFO(0, js)
ZEND_END_ARG_INFO()

static zend_function_entry minify_jsmin_methods[] = {
    JSMINIFY_ME(__construct,         arginfo_minify_jsmin__construct,      ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    JSMINIFY_ME(minify,         arginfo_minify_jsmin_minify,      ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    {NULL, NULL, NULL}
};

extern int php_minify_jsmin_register() {
    zend_class_entry minjsce;

    INIT_CLASS_ENTRY(minjsce, "JSMin", minify_jsmin_methods);

    jsmin_class_entry = zend_register_internal_class(&minjsce TSRMLS_CC);

    return SUCCESS;
}


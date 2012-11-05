/*
+----------------------------------------------------------------------+
| See LICENSE file for further copyright information                   |
+----------------------------------------------------------------------+
| Authors: John Jawed <jawed@php.net>                                  |
+----------------------------------------------------------------------+
*/

#ifndef PHP_MINIFY_H
#define PHP_MINIFY_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "zend_API.h"
#include "zend_variables.h"
#include "ext/standard/head.h"
#include "php_globals.h"
#include "php_main.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_string.h"
#include "php_globals.h"

#define PHP_MINIFY_VERSION 1.0.4

#define JSMINIFY_METHOD(func) PHP_METHOD(jsmin, func)
#define JSMINIFY_ME(func, arg_info, flags) PHP_ME(jsmin, func, arg_info, flags)

#define __stringify_1(x)    #x
#define __stringify(x)      __stringify_1(x)
#define __PHP_MINIFY_VERSION PHP_MINIFY_VERSION

#define MINIFY_EXT_VER __stringify(__PHP_MINIFY_VERSION)

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2) || PHP_MAJOR_VERSION > 5
# define MINIFY_ARGINFO
#else
# define MINIFY_ARGINFO static
#endif

extern zend_module_entry minify_module_entry;

#define phpext_minify_ptr &minify_module_entry

#ifdef ZTS
#error "php5-minify is not thread safe yet"
#endif

PHP_MINIT_FUNCTION(minify);
PHP_MSHUTDOWN_FUNCTION(minify);
PHP_MINFO_FUNCTION(minify);

#ifdef ZTS_0
#define MINIFY(v) TSRMG(minify_globals_id, zend_minify_globals *, v)
#else
#define MINIFY(v) (minify_globals.v)
#endif

extern int php_minify_jsmin_register();

#endif
/**
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4 expandtab
 */

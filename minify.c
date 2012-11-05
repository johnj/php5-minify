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

PHP_MINIT_FUNCTION(minify) 
{
    php_minify_jsmin_register();
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(minify) 
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(minify)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "minify support", "enabled");
    php_info_print_table_row(2, "JSMin support", "enabled");
    php_info_print_table_end();
}

/* {{{ xcom_module_entry */
zend_module_entry minify_module_entry = {
STANDARD_MODULE_HEADER_EX, NULL,
NULL,
"minify",
NULL,
PHP_MINIT(minify),
PHP_MSHUTDOWN(minify),
NULL,
NULL,
PHP_MINFO(minify),
MINIFY_EXT_VER,
STANDARD_MODULE_PROPERTIES
};
/* }}} */

#if COMPILE_DL_MINIFY
ZEND_GET_MODULE(minify)
#endif


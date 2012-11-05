dnl $Id$

PHP_ARG_ENABLE(minify, whether to enable minify support,
        [  --disable-minify           Enable minify support], yes)

if test "$PHP_MINIFY" != "no"; then
    PHP_NEW_EXTENSION(minify, minify.c php_jsmin.c jsmin.c, $ext_shared)
    PHP_SUBST(MINIFY_SHARED_LIBADD)
fi


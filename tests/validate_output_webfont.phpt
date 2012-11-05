--TEST--
JSMin::minify test
--CREDITS--
John Jawed
--SKIPIF--
<?php if (!extension_loaded("minify")) print "skip"; ?>
--FILE--
<?php
echo "*** validate minify's JSMin output vs JSMin userspace for webfont.js\n";
include('Minify/jsmin.php');
$their_js = JSMin_B::minify("webfont.js");
$our_js = JSMin::minify("webfont.js");

var_dump(strcmp($their_js, $our_js));

?>
--EXPECTF--
*** validate minify's JSMin output vs JSMin userspace for webfont.js
int(0)

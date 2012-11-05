--TEST--
JSMin::minify test
--CREDITS--
John Jawed
--SKIPIF--
<?php if (!extension_loaded("minify")) print "skip"; ?>
--FILE--
<?php
echo "*** validate minify's JSMin output vs JSMin userspace for prototype.js\n";
include('Minify/jsmin.php');
$their_js = JSMin_B::minify("prototype.js");
$our_js = JSMin::minify("prototype.js");

var_dump(strcmp($their_js, $our_js));

?>
--EXPECTF--
*** validate minify's JSMin output vs JSMin userspace for prototype.js
int(0)

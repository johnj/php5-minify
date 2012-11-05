--TEST--
JSMin::minify test
--CREDITS--
John Jawed
--SKIPIF--
<?php if (!extension_loaded("minify")) print "skip"; ?>
--FILE--
<?php
echo "*** validate minify's JSMin output vs JSMin userspace for scriptaculous.js\n";
include('Minify/jsmin.php');
$their_js = JSMin_B::minify("scriptaculous.js");
$our_js = JSMin::minify("scriptaculous.js");

var_dump(strcmp($their_js, $our_js));

?>
--EXPECTF--
*** validate minify's JSMin output vs JSMin userspace for scriptaculous.js
int(0)

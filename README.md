php5-minify
===========

minify provides JavaScript and CSS minimizers for PHP5 which are substantially more efficient than PHP based ones.

Examples
========
Minifying JS:
```php
<?php

$minified_js = JSMin::minify(file_get_contents("some.js"));

?>
```

Notes
=====
- ~~This extension isn't thread safe - yet~~. This extension is thread safe now thanks to mkoppanen/#1.
- CSS minification support is coming *real soon*.
- Requires >=PHP5

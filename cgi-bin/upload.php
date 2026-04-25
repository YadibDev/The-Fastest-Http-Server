#!/usr/bin/php-cgi
<?php
    // Must send a Content-type header before any other output
    header("Content-type: text/html");

    echo "<h1>CGI PHP Test</h1>";
    echo "Server Time: " . date('Y-m-d H:i:s');
?>

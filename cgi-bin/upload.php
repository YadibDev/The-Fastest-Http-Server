#!/usr/bin/php-cgi
<?php
// Signal to the browser what kind of content is coming
header("Content-Type: text/html; charset=UTF-8");

// Standard CGI variables are available in the $_SERVER superglobal

echo "<html>";
echo "<head><title>PHP CGI Response</title></head>";
echo "<body>";
echo "<h1>Hello from PHP CGI!</h1>";
echo "<p><strong>Server Software:</strong> $server_software</p>";
echo "<p><strong>Request Method:</strong> $request_method</p>";
echo "<p>The current server time is: " . date('Y-m-d H:i:s') . "</p>";
echo "</body>";
echo "</html>";
?>
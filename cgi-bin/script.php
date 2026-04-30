#!/usr/bin/php-cgi
<?php
// 1. Manually output the HTTP Content-Type header
// This is required for CGI; otherwise, the server returns a 500 error.
echo "Content-type: text/html\r\n\r\n";

// 2. Standard PHP logic
echo "<html><body>";
echo "<h1>Hello from PHP CGI</h1>";
echo "<h1>Hello WORLD FROM CGI</h1>";
echo "<p>The current server time is: " . date('Y-m-d H:i:s') . "</p>";

// 3. Accessing CGI environment variables
// Standard variables like $_GET or $_POST may not be auto-populated in some CGI modes.
$queryString = getenv('QUERY_STRING');
echo "<p>Raw Query String: " . htmlspecialchars($queryString) . "</p>";
echo "</body></html>";
?>

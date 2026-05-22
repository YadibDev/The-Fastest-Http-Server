<?php
// Always output headers first in CGI

echo "status: 200 OK\r\n"

// Read GET parameters
$name = isset($_GET['name']) ? $_GET['name'] : "Guest";

echo "<html>";
echo "<head><title>PHP CGI</title></head>";
echo "<body>";
echo "<h1>Hello, " . htmlspecialchars($name) . "</h1>";
echo "<p>This is PHP running as CGI.</p>";
echo "</body>";
echo "</html>";
?>
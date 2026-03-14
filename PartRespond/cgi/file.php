<?php
// إرسال الـ Status Header
echo "HTTP/1.1 200 OK\r\n";

// إرسال الـ Content-Type Header
echo "Content-Type: text/html\r\n\r\n";

// الـ Body (المحتوى)
?>
<!DOCTYPE html>
<html>
<head>
    <title>CGI Response</title>
</head>
<body>
    <h1>Hello from CGI!</h1>
    <p>This is a simple HTML body.</p>
</body>
</html>
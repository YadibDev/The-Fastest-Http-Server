#!/usr/bin/php
<?php
// 1. إرسال الترويسة ليقوم المتصفح بتفسير المخرجات كـ HTML
echo "Content-Type: text/html\r\n\r\n";

// 2. كود الـ HTML
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>PHP Test for Webserv</title>
</head>
<body style="font-family: sans-serif; text-align: center; margin-top: 50px;">
    <h1 style="color: #4F5B93;">Hello Mosab! 🚀</h1>
    <p>This page was generated using <b>PHP</b>.</p>
    
    <div style="background: #eee; padding: 20px; display: inline-block; border-radius: 8px;">
        <?php
            echo "Current Server Time: " . date('Y-m-d H:i:s') . "<br>";
            echo "PHP Version: " . phpversion();
        ?>
    </div>
</body>
</html>
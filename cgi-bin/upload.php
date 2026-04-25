<?php
$targetDir = "/workspaces/The-Fastest-Http-Server/cgi-bin/uploads";

echo "<pre>";
    print_r($_FILES);
echo "</pre>";

$targetFile = $targetDir . "/" . basename($_FILES["file"]["name"]);

echo basename($targetFile);
echo "\n<br>";

if (move_uploaded_file($_FILES["file"]["tmp_name"], $targetFile)) {
    echo "File uploaded successfully.";
} else {
    echo "Upload failed. Error code: " . $_FILES["file"]["error"];
}
?>

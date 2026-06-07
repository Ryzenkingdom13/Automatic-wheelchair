<?php
$conn = new mysqli("localhost","esskay012024_prasath","Solutions@12345.","esskay012024_data");

$cmd = $_GET['cmd'];

$conn->query("UPDATE control SET command='$cmd' WHERE id=1");

$conn->close();
?>
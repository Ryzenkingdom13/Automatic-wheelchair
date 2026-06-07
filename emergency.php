<?php
$conn = new mysqli("localhost","esskay012024_prasath","Solutions@12345.","esskay012024_data");

$conn->query("UPDATE control SET emergency=1 WHERE id=1");

$conn->close();
?>
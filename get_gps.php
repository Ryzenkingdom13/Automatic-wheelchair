<?php
$conn = new mysqli("localhost","esskay012024_prasath","Solutions@12345.","esskay012024_data");

$result = $conn->query("SELECT * FROM gps_data ORDER BY id DESC LIMIT 1");

$row = $result->fetch_assoc();

echo $row['latitude'] . "," . $row['longitude'];

$conn->close();
?>
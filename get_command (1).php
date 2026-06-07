<?php
$conn = new mysqli("localhost","esskay012024_prasath","Solutions@12345.","esskay012024_data");

$result = $conn->query("SELECT * FROM control WHERE id=1");

$row = $result->fetch_assoc();

echo $row['command'] . "," . $row['emergency'];

$conn->close();
?>
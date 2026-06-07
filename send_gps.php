<?php
$conn = new mysqli("localhost","esskay012024_prasath","Solutions@12345.","esskay012024_data");
$lat = $_GET['lat'];
$lon = $_GET['lon'];

$sql = "INSERT INTO gps_data (latitude, longitude)
        VALUES ('$lat', '$lon')";

$conn->query($sql);
$conn->close();
?>
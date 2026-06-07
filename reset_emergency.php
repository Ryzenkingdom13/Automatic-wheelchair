<?php
$conn =new mysqli("localhost","esskay012024_prasath","Solutions@12345.","esskay012024_data");
$conn->query("UPDATE control SET emergency=0 WHERE id=1");
$conn->close();
?>
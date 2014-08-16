#!/usr/bin/php
<?php
$con = mysql_connect("localhost","test","1234");
if (!$con)
{
	die('Could not connect: ' . mysql_error());
}

mysql_select_db("test", $con);

$sql = "SELECT * from student";
$result = mysql_query($sql,$con);
print_r(mysql_fetch_array($result,MYSQL_ASSOC));


mysql_close($con);
?>

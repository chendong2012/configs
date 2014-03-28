#!/usr/bin/php
<?php
error_reporting(E_ALL);
set_time_limit(10);

$port = 5555;
$ip = "18.8.9.59";

/*
 +-------------------------------
 *    @socket连接整个过程
 +-------------------------------
 *    @socket_create
 *    @socket_connect
 *    @socket_write
 *    @socket_read
 *    @socket_close
 +--------------------------------
 */

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket < 0) {
	    echo "errcode:" . socket_strerror($socket);
}
socket_set_option($socket,SOL_SOCKET, SO_RCVTIMEO, array("sec"=>3, "usec"=>0));
$result = socket_connect($socket, $ip, $port);
if ($result < 0) {
	    echo "errcode:" . socket_strerror($result);
}
$out = '';
$in = 'hello server!';
if(!socket_write($socket, $in, strlen($in))) {
	echo "errcode:" . socket_strerror($socket);
}

while($out = socket_read($socket, 8192)) {
	        echo "from server:",$out;
}
socket_close($socket);

<?php
	$db_host = 'localhost:/var/run/mysql/mysql.sock';
	$db_user = 'xambro15';
	$db_password = '9hajeven';
	$db_name = 'xambro15';

	$db = mysql_connect($db_host, $db_user, $db_password);
	if (!mysql_select_db($db_name, $db))
		die('Connection failed: ' . mysql_error());

	mysql_set_charset("utf8", $db);
?>

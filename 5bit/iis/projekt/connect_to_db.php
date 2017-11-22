<?php
	$db_host = 'localhost:/var/run/mysql/mysql.sock';
	$db_user = 'xaubre02';
	$db_password = 'ajban3sa';
	$db_name ='xaubre02';
	
	$db = mysql_connect($db_host, $db_user, $db_password);
	
	if (!$db)
		die('Nelze se pripojit: '.mysql_error());
	
	if (!mysql_select_db($db_name, $db))
		die('Databaze neni dostupna: '.mysql_error());
	
	mysql_set_charset("utf8", $db);
?>
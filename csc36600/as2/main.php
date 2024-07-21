<?php

	session_start();
	$db = new mysqli('134.74.126.107', 'S1533621', '0189', 'S1533621');

	if($db->connect_errno > 0){
	    die('Unable to connect to database [' . $db->connect_error . ']');
	}

	//Goals
	$sql = <<<SQL
	    SELECT *
	    FROM `as2_goals`
SQL;

	if(!$result = $db->query($sql)){
	    die('There was an error running the query [' . $db->error . ']');
	}

	echo '<b><i><font size="15"/>'.'List of Goals'.'</b></i>'.'<table border="1" style="width:100%" align="center">';
	echo '<tr>'.'<th>'.'<b><u>GID</b></u>'.'</th>'.'<th>'.'<b>Goal Statement</b>'.'</th>'.'</tr>';

	while($row = $result->fetch_assoc()){
		echo '<tr>';
    	echo '<td>'.'g'. $row['id'] . '</td>';
    	echo '<td>'. $row['Goal'] . '</td>';
    	echo '</tr>';
	}
	echo '</table>';

	//Services
	$sql = <<<SQL
	    SELECT *
	    FROM `as2_services`
SQL;

	if(!$result = $db->query($sql)){
	    die('There was an error running the query [' . $db->error . ']');
	}

	echo '<b><i><font size="10"/>'.'List of Services'.'</b></i>'.'<table border="1" style="width:100%" align="center">';	
	echo '<tr>'.'<th>'.'<b><u>SID</b></u>'.'</th>'.'<th>'.'<b>Service Name</b>'.'</th>'.'</tr>';

	while($row = $result->fetch_assoc()){
		$currentName = $row['Service'];
		$currentId = $row['id'];
		echo '<tr>';
    	echo '<td>'.'<a href ="list.php?serviceId='.urlencode($currentId).'">'.'s'. $currentId . '</a></td>';
    	echo '<td>'. $currentName . '</td>';
    	echo '</tr>';
	}
	echo '</table>';
	$db->close();
?>
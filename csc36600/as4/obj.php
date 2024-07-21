<!DOCTYPE html>
<html>
<head>
	<title>As4</title>
	<link href="as4.css" rel="stylesheet" type="text/css"></link>
</head>
<body>
<?php
	error_reporting(E_ALL);
	ini_set('display_errors', '1');
	
	//$db = new mysqli('134.74.126.107', 'S1533621', '0189', 'S1533621');
	$db = new mysqli('localhost', 'root', 'sesame', 'S1533621');

	if($db->connect_errno > 0){
		die('Unable to connect to database ['.$db->connect_errno.']');
	}

	$verifyStatment = $db->prepare('SELECT a.Service,c.Objective FROM as4_services a, as4_s2o b, as4_objectives c where a.id=? and c.id=? and b.sid=? and b.oid=?');
	$passedSid = $_GET["sid"];
	$passedOid = $_GET["oid"];
	$passedRank = $_GET["rank"];
	$verifyStatment->bind_param('iiii', $passedSid, $passedOid, $passedSid, $passedOid);
	$verifyStatment->execute();
	$verifyStatment->bind_result($service, $objective);
	$secure = true;
	if($verifyStatment->fetch()){
		echo '<b><font size="15">'.'Objective '.$passedRank.' of [s'.$passedSid.':'.$service.']'.'</b></font><br><br>';	
		echo $objective.'<br><br>';
	}
	else{
		$secure = false;
		echo '<b><i>Specified url does not exist. Use the back button on your browser to select one of the provided links.</i></b>';
	}
	$verifyStatment->close();
	if($secure==true){
		echo '<form method="post" action="edit.php">';
		echo '<table>';
		echo '<tr><th>'.'<b>Measuring Instrument</b>'.'</th>';
		echo '<th>'.'<b>PDF Document</b>'.'</th>';
		echo '<th>'.'<b>Evaluate Condition</b>'.'</th>';
		$filesStatement = $db->prepare('SELECT b.id,b.filename,b.name,b.type,b.belowe,b.abovee FROM as4_o2f a, as4_file b where a.oid=? and a.fid=b.id');
		$filesStatement->bind_param('i', $passedOid);
		$filesStatement->execute();
		$filesStatement->bind_result($fileId, $fileName, $name, $fileType, $belowE, $aboveE);
		echo '<th rowspan="0">'.'<button name="editObj" type="submit" value="files_'.$passedOid.'">edit</button>'.'</th></tr>';
		echo '<input type="hidden" value="'.$passedOid.'" name="oid">';
		echo '<input type="hidden" value="'.$passedSid.'" name="sid">';
		echo '<input type="hidden" value="'.$passedRank.'" name="rank">';

		while($filesStatement->fetch()){
			echo '<tr>';
			echo '<td>name: '.$name;
			$string;
			if($fileType == 0){
				$string = "exam";
			}
			else{
				$string = "assignment";
			}
			echo ' type: '.$string.'</td>';
			echo '<td>'.'<a href="download.php?id='.$fileId.'">'.$fileName.'</a></td>';
			echo '<td>'.'Below Expectations: '.$belowE.'<br>Above Expectations: '.$aboveE.'</td>';
			echo '</tr>';
		}
		$filesStatement->close();
		echo '</table>';
		echo '</form>';
		echo '<form method="post" action="main.php"><br><br>';
		echo '<button type="submit">Back</button>';
		echo '</form>';
	}
	$db->close();
?>
</body>
</html>
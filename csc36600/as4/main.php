<!DOCTYPE html>
<html>
<head>
	<title>As4</title>
	<link href="as4.css" rel="stylesheet" type="text/css"></link>
</head>
<body>
<?php
	//error_reporting(E_ALL);
	//ini_set('display_errors', '1');
	
	//$db = new mysqli('134.74.126.107', 'S1533621', '0189', 'S1533621');
	$db = new mysqli('localhost', 'root', 'sesame', 'S1533621');

	if($db->connect_errno > 0){
		die('Unable to connect to database ['.$db->connect_errno.']');
	}

	//Goal statement
	if(!$result = $db->query("SELECT * FROM as4_goals")){
	    die('There was an error running the query ['.$db->error.']');
	}

	$ng = $result->num_rows;
	echo '<form method="post" action="edit.php">';
	//Goals
	echo '<b><i><font size="15">'.'List of Goals'.'</b></i></font>';
	echo '<table>';
	echo '<tr>'.'<th>'.'<b><u>GID</b></u>'.'</th>'.'<th>'.'<b>Goal Statement</b>'.'</th>';
	echo '<th rowspan="'.($ng+1).'">'.'<button name="edit" value="edit_g" type="submit">edit</button>'.'</th>'.'</tr>';

	$gids = [];
	while($row = $result->fetch_assoc()){
		echo '<tr>';
		echo '<td>'.'g'.$row['id'].'</td>';
		$gids[] = $row['id'];
		echo '<td>'.$row['Goal'].'</td>';
		echo '</tr>';
	}
	$result->close();
	echo '</table>';

	//Service statement
	if(!$result = $db->query("SELECT * FROM as4_services")){
		die('There was an error running the query ['.$db->error.']');
	}
	//Service array
	$serviceA = [];
	while($row = $result->fetch_assoc()){
		$serviceA[$row['id']] = $row['Service'];
	}
	$result->close();
	$serviceO = new ArrayObject($serviceA);
	$iterator = $serviceO->getIterator();
	//Services
	echo '<b><i><font size="10">'.'List of Services'.'</b></i></font>';
	echo '<table>';	
	echo '<tr>'.'<th>'.'<b><u>SID</b></u>'.'</th>'.'<th>'.'<b>Service Name</b>'.'</th>'.'<th>'.'<b>Objectives</b>'.'</th>'.'</tr>';
	while($iterator->valid()){
		echo '<tr>';
		echo '<td>'.'s'.$iterator->key().'</td>';
		echo '<td>'.$iterator->current().'</td>';
		$key = $iterator->key();
		//Objectives Statement
		$objectiveIdStatement = $db->prepare('SELECT o.id, o.Objective FROM as4_objectives o, as4_s2o WHERE as4_s2o.SID = ? and o.id = as4_s2o.oid');
		$objectiveIdStatement->bind_param('i', $key);
		$objectiveIdStatement->execute();
		$objectiveIdStatement->bind_result($oid, $objective);

		//Objectives array
		$objectiveIdA = [];
		$objectiveA = [];
		while($objectiveIdStatement->fetch()){
			$objectiveIdA[] = $oid;
			$objectiveA[] = $objective;
		}
		$objectiveIdO = new ArrayObject($objectiveIdA);
		$objectiveIdIterator = $objectiveIdO->getIterator();
		$objectiveO = new ArrayObject($objectiveA);
		$objectiveIterator = $objectiveO->getIterator();

		echo '<td>';
		echo '<table>';
		echo '<th>&nbsp</th>'.'<th>Objective Statement</th>';
		$gCount = 0;
		while($gCount < $ng){
			echo '<th>g'.$gids[$gCount].'</th>';
			$gCount++;
		}
		echo '<th rowspan="'.($objectiveIdO->count()+1).'">'.'<button name="edit" value="edit_s'.$iterator->key().'" type="submit">edit</button>'.'</th>';
		$objectiveIdStatement->close();
		$count = 1;
		while($objectiveIterator->valid()){
			echo '<tr>';
			echo '<td>'.'<a href="obj.php?rank='.$count.'&oid='.$objectiveIdIterator->current().'&sid='.$iterator->key().'">'.$count.'</a>'.'</td>';
			echo '<td>'.$objectiveIterator->current().'</td>';
			//Related Goals Statement
			$g2oo = $objectiveIdIterator->current();
			$g2o = $db->prepare('SELECT gid FROM as4_g2o WHERE oid = ?');
			$g2o->bind_param('i', $g2oo);
			$g2o->execute();
			$g2o->bind_result($g2og);
			$g2o->fetch();
			$columnCount = 0;
			while($columnCount < ($ng)){
				if($g2og == $gids[$columnCount]){
					echo '<td>'.'&#9770'.'</td>';
					$g2o->fetch();
					$columnCount++;
				}
				else{
					echo '<td>&nbsp</td>';
					$columnCount++;
				}
			}
			$g2o->close();
			$objectiveIterator->next();
			$objectiveIdIterator->next();
			$count++;
		}

		echo '</table>';
		echo '</td>';
		echo '</tr>';
		$iterator->next();
	}
	echo '</table>';
	echo '</form>';

	$db->close();
?>
</body>
</html>
<!DOCTYPE html>
<html>
<head>
	<title>Edit</title>
	<link href="as4.css" rel="stylesheet" type="text/css"></link>
</head>
<body>
<?php
	error_reporting(E_ALL);
	ini_set('display_errors', '1');

	$type = 0;
	$edit;
	$change;
	if(isset($_POST['edit'])){
		$edit = $_POST['edit'];
		if($edit != 'edit_g'){
			$edit = str_ireplace('edit_s', '', $edit);
			$type = 1;
		}
	}
	else if(isset($_POST['editObj'])){
		$change = $_POST['editObj'];
		$change = str_ireplace('files_', '', $change);
	}


	//$db = new mysqli('134.74.126.107', 'S1533621', '0189', 'S1533621');
	$db = new mysqli('localhost', 'root', 'sesame', 'S1533621');
	if($db->connect_errno > 0){
		die('Unable to connect to database ['.$db->connect_errno.']');
	}
	if(isset($_POST['edit'])){
		if(!$result = $db->query("SELECT * FROM as3_goals")){
			die('There was an error running the query ['.$db->error.']');
		}

		$gids = [];
		while($temp = $result->fetch_assoc()){
			$gids[] = $temp['id'];
		}
		$result->close();

		echo '<form method="post" action="change.php">';

		if($type == 0){
			//Goal edit
			if(!$result = $db->query("SELECT * FROM as3_goals")){
				die('There was an error running the query ['.$db->error.']');
			}
			echo '<b><i><font size="15">'.'List of Goals'.'</b></i></font>';
			echo '<table>';
			echo '<tr>'.'<th>'.'<b><u>GID</b></u>'.'</th>'.'<th>'.'<b>Goal Statement</b>'.'</th>';
			echo '<th>'.'<button name="submit" value="g_submit" type="submit">submit</button>'.'</th>';

			while($row = $result->fetch_assoc()){
				echo '<tr>';
				echo '<td>'.'g'.$row['id'].'</td>';
				echo '<td>'.'<input type="text" name="g['.$row['id'].'][goal]" value="'.$row['Goal'].'"></input></td>';
				echo '<td>'.'<input type="checkbox" name="g['.$row['id'].'][delete]" value="'.$row['id'].'">Delete</input></td>';
				echo '</tr>';
			}
			echo '<tr>';
			echo '<td>&nbsp</td>';
			echo '<td>'.'<input type="text" name="g_extra" value="Insert New Goal"></input></td>';
			echo '<td>&nbsp</td>';
			$result->close();
		}
		else{
			//Objective edit
			//Goal statement
			if(!$result = $db->query("SELECT * FROM as3_goals")){
		    	die('There was an error running the query ['.$db->error.']');
			}

			$ng = $result->num_rows;
			$result->close();
			//Service name,etc...
			$serviceStatement = $db->prepare('SELECT * FROM as3_services WHERE id = ?');
			$serviceStatement->bind_param('i', $edit);
			$serviceStatement->execute();
			$serviceStatement->bind_result($sid, $service);
			$serviceStatement->fetch();
			$serviceStatement->close();

			echo '<b><i><font size="15">'.'Objectives of [';
			echo 's'.$sid.':'.$service.']'.'</b></i></font>';
			echo '<table>';
			echo '<tr>'.'<th>'.'<b><u>OID</b></u>'.'</th><th>'.'<b>Objective Statement</b>'.'</th>';
			echo '<th>'.'<b>Associated Goals</b>'.'</th>';
			echo '<th>'.'<button name="submit" value="o_submit'.$sid.'" type="submit">submit</button>'.'</th>';
			echo '</tr>';

			//Objective statement
			$objectiveIdStatement = $db->prepare('SELECT o.id, o.Objective FROM as3_objectives o, as3_s2o WHERE as3_s2o.SID = ? and o.id = as3_s2o.oid');
			$objectiveIdStatement->bind_param('i', $sid);
			$objectiveIdStatement->execute();
			$objectiveIdStatement->bind_result($oid,$objective);

			//Objective array
			$objectiveIdA = [];
			$objectiveA = [];
			while($objectiveIdStatement->fetch()){
				$objectiveIdA[] = $oid;
				$objectiveA[] = $objective;
			}
			$objectiveIdO = new ArrayObject($objectiveIdA);
			$objectiveIdI = $objectiveIdO->getIterator();
			$objectiveO = new ArrayObject($objectiveA);
			$objectiveI = $objectiveO->getIterator();
			$objectiveIdStatement->close();

			while($objectiveI->valid() && $objectiveIdI->valid()){
				echo '<tr>';
				echo '<td>'.'o'.$objectiveIdI->current().'</td>';
				echo '<td>'.'<input type="text" name="o['.$objectiveIdI->current().'][objective]" value="'.$objectiveI->current().'"></input></td>';
				//Associated Goals
				$g2oo = $objectiveIdI->current();
				$g2o = $db->prepare('SELECT gid FROM as3_g2o WHERE oid = ?');
				$g2o->bind_param('i', $g2oo);
				$g2o->execute();
				$g2o->bind_result($g2og);
				$g2o->fetch();
				$columnCount = 0;
				echo '<td>';
				while($columnCount < ($ng)){
					if($g2og == $gids[$columnCount]){
						echo '<input type="checkbox" name="o['.$objectiveIdI->current().']['.$columnCount.'][goals]" value="'.$gids[$columnCount].'" checked></input>';
						$g2o->fetch();
						$columnCount++;
					}
					else{
						echo '<input type="checkbox" name="o['.$objectiveIdI->current().']['.$columnCount.'][goals]" value="'.$gids[$columnCount].'"></input>';
						$columnCount++;
					}
				}
				$g2o->close();
				echo '</td>';
				echo '<td>';
				echo '<input type="checkbox" name="o['.$objectiveIdI->current().'][delete]" value="delete" >Delete</input>';
				echo '</td>';
				echo '</tr>';

				$objectiveI->next();
				$objectiveIdI->next();
			}
			echo '<tr>';
			echo '<td>&nbsp</td>';
			echo '<td>'.'<input type="text" name="o_extra[objective]" value="Insert new Objective"></input></td>';
			$columnCount = 0;
			echo '<td>';
			while($columnCount < ($ng)){
				echo '<input type="checkbox" name="o_extra['.$columnCount.'][goals]" value="'.$gids[$columnCount].'""></input>';
				$columnCount++;
			}
			echo '</td>';
			echo '<td>&nbsp</td>';
			echo '</tr>';

			echo '</table>';
		}
		echo '</form>';
	}
	else if(isset($_POST['editObj'])){
		$passedSid = $_POST["sid"];
		$passedOid = $_POST["oid"];
		$passedRank = $_POST["rank"];
		$verifyStatment = $db->prepare('SELECT a.Service,c.Objective FROM as4_services a, as4_s2o b, as4_objectives c where a.id=? and c.id=? and b.sid=? and b.oid=?');
		$verifyStatment->bind_param('iiii', $passedSid, $passedOid, $passedSid, $passedOid);
		$verifyStatment->execute();
		$verifyStatment->bind_result($service, $objective);
		$verifyStatment->fetch();
		echo '<b><font size="15">'.'Objective '.$passedRank.' of [s'.$passedSid.':'.$service.']'.'</b></font><br><br>';	
		echo $objective.'<br><br>';

		$verifyStatment->close();
		echo '<form method="post" action="change.php" enctype="multipart/form-data">';
		echo '<table>';

		echo '<input type="hidden" name="MAX_FILE_SIZE" value="2000000">';
		echo '<input type="hidden" name="files" value="'.$passedOid.'">';
		echo '<tr><th>'.'<b>Measuring Instrument</b>'.'</th>';
		echo '<th>'.'<b>PDF Document</b>'.'</th>';
		echo '<th>'.'<b>Evaluate Condition</b>'.'</th>';
		echo '<th>'.'<button name="submit" type="submit" value="f_submit">Submit</button>'.'</th></tr>';
		$filesStatement = $db->prepare('SELECT b.id,b.name,b.filename,b.type,b.belowe,b.abovee FROM as4_o2f a, as4_file b where a.oid=? and a.fid=b.id');
		$passedOID = intval($passedOid);
		$filesStatement->bind_param('i', $passedOID);
		$filesStatement->execute();
		$filesStatement->bind_result($fileId, $name, $fileName, $fileType, $belowE, $aboveE);
		while($filesStatement->fetch()){
			echo '<tr>';
			echo '<td>name: '.'<input type="text" value="'.$name.'" name="fileName_'.$fileId.'"><br>';
			$string;
			if($fileType == 0){
				$string = "exam";
				echo '<input type="radio" value="0" name="fileType'.$fileId.'" checked>Exam</input>';
				echo '<input type="radio" value="1" name="fileType'.$fileId.'">Assignment</input>';
			}
			else{
				$string = "assignment";
				echo '<input type="radio" value="0" name="fileType'.$fileId.'">Exam</input>';
				echo '<input type="radio" value="1" name="fileType'.$fileId.'" checked>Assignment</input>';
			}
			echo '</td>';
			echo '<td>'.'<input type="file" id="'.$fileId.'" name="upload'.$fileId.' accept=".pdf"/>'.'<font size=3px>'.$fileName.'</font></td>';
			echo '<td>Below Expectations: <input type="number" name="be_'.$fileId.'" min="0" max="100" step="5" value="'.$belowE.'"><br>';
			echo 'Above Expectations: <input type="number" name="ae_'.$fileId.'" min="0" max="100" step="5" value="'.$aboveE.'"></td>';
			echo '<td><input type="checkbox" name="delete.'.$fileId.'">Delete</input></td>';
			echo '</tr>';
		}
		//new insert
		echo '<tr>';
		echo '<td>name: '.'<input type="text" value="ENTER NAME" name="insert_name"><br>';
		echo '<input type="radio" value="0" name="insert_type" checked>Exam</input>';
		echo '<input type="radio" value="1" name="insert_type">Assignment</input></td>';
		echo '<td> <input type="file" name="insert_upload" accept=".pdf"></td>';
		echo '<td>Below Expectations: <input type="number" name="insert_be" min="0" max="100" step="5" value="45"><br>';
		echo 'Above Expectations: <input type="number" name="insert_ae" min="0" max="100" step="5" value="55"></td>';
		echo '</tr>';


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
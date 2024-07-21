<?php
	error_reporting(E_ALL);
	ini_set('display_errors', '1');

	//$db = new mysqli('134.74.126.107', 'S1533621', '0189', 'S1533621');
	$db = new mysqli('localhost', 'root', 'sesame', 'S1533621');

	if($db->connect_errno > 0){
		die('Unable to connect to database ['.$db->connect_errno.']');
	}

	$type = 0;
	$edit = $_POST['submit'];
	if($edit != 'g_submit' && $edit != 'f_submit'){
		$edit = str_ireplace('o_submit', '', $edit);
		$type = 1;
	} 
	else if($edit == 'f_submit'){
		$type = 2;
	}

	if(!$result = $db->query("SELECT * FROM as4_goals")){
	    die('There was an error running the query ['.$db->error.']');
	}

	$ng = $result->num_rows;
	$result->close();

	if($type == 0){
		//Goal edit
		if(!$result = $db->query("SELECT * FROM as4_goals")){
			die('There was an error running the query ['.$db->error.']');
		}
		$gid = [];
		$OGoal = [];
		$NGoal = [];
		$Ydelete = false;
		$Yupdate = false;
		while($row = $result->fetch_assoc()){
			if(isset($_POST['g'][$row['id']]['delete'])){
				$gid[] = $row['id'];
				$Ydelete = true;
			}
			else if(trim($_POST['g'][$row['id']]['goal']) != trim($row['Goal'])){
				$OGoal[] = $row['Goal'];
				$NGoal[] = $_POST['g'][$row['id']]['goal'];
				$Yupdate = true;
			}
		}
		$result->close();
		//Delete Statements
		if($Ydelete == true){
			$gidO = new ArrayObject($gid);
			$gidI = $gidO->getIterator();
			while($gidI->valid()){
				$current = $gidI->current();
				$delete = $db->prepare('DELETE FROM as4_goals WHERE id = ?');
				$delete->bind_param('i', $current);
				$delete->execute();
				$delete->close();
				$delete = $db->prepare('DELETE FROM as4_g2o WHERE gid = ?');
				$delete->bind_param('i', $current);
				$delete->execute();
				$delete->close();
				$gidI->next();
			}
		}
		//Update Statements
		if($Yupdate == true){
			$OGoalO = new ArrayObject($OGoal);
			$OGoalI = $OGoalO->getIterator();
			$NGoalO = new ArrayObject($NGoal);
			$NGoalI = $NGoalO->getIterator();
			while($OGoalI->valid() && $NGoalI->valid()){
				$old = $OGoalI->current();
				$new = $NGoalI->current();
				$update = $db->prepare('UPDATE as4_goals SET Goal = ? WHERE Goal = ?');
				$update->bind_param('ss', $new, $old);
				$update->execute();
				$update->close();
				$OGoalI->next();
				$NGoalI->next();
			}
		}
		//Insert Statement
		if(trim($_POST['g_extra']) != 'Insert New Goal'){
			$new = $_POST['g_extra'];
			$add = $db->prepare('INSERT INTO as4_goals (Goal) VALUES (?)');
			$add->bind_param('s', $new);
			$add->execute();
			$add->close();
		}
		$db->close();
		echo ('<meta http-equiv="refresh" content="0;url=main.php">');
	}
	else if($type == 1){
		//Objective edit
		$objectiveIdStatement = $db->prepare('SELECT o.id, o.Objective FROM as4_objectives o, as4_s2o WHERE as4_s2o.SID = ? and o.id = as4_s2o.oid');
		$objectiveIdStatement->bind_param('i', $edit);
		$objectiveIdStatement->execute();
		$objectiveIdStatement->bind_result($oid,$objective);

		//Objective array
		$objectiveIdA = [];
		$objectiveA = [];
		while($objectiveIdStatement->fetch()){
			$objectiveIdA[] = $oid;
			$objectiveA[] = $objective;
		}
		$objectiveIdStatement->close();
		$objectiveIdO = new ArrayObject($objectiveIdA);
		$objectiveIdI = $objectiveIdO->getIterator();
		$objectiveO = new ArrayObject($objectiveA);
		$objectiveI = $objectiveO->getIterator();

		//Scanner
		$oid = [];
		$Nobjective = [];
		$Oobjective = [];
		$OldAssociatedGoals = array(array());
		$CurrentAssociatedGoals = array(array());
		while($objectiveIdI->valid()){
			$delete = false;
			if(isset($_POST['o'][$objectiveIdI->current()]['delete'])){
				$oid[] = $objectiveIdI->current();
				$delete = true;
			}
			else if(trim($objectiveI->current()) != trim($_POST['o'][$objectiveIdI->current()]['objective'])){
				$Oobjective[] = trim($objectiveI->current());
				$Nobjective[] = trim($_POST['o'][$objectiveIdI->current()]['objective']);
			}
			if($delete == false){
				//Goals
				$currentOid = $objectiveIdI->current();
				$g2o = $db->prepare('SELECT gid FROM as4_g2o WHERE oid = ?');
				$g2o->bind_param('i', $currentOid);
				$g2o->execute();
				$g2o->bind_result($currentGid);
				while($g2o->fetch()){
					$OldAssociatedGoals[$currentOid][] = $currentGid;
				}
				$g2o->close();
				$columnCount = 0;
				while($columnCount < $ng){
					if(isset($_POST['o'][$currentOid][$columnCount]['goals'])){
						$temp = $_POST['o'][$currentOid][$columnCount]['goals'];
						$CurrentAssociatedGoals[$currentOid][] = intval($temp);
					}
					$columnCount++;
				}
			}
			$objectiveIdI->next();
			$objectiveI->next();
		}
		//Delete Statements
		$oidO = new ArrayObject($oid);
		$oidI = $oidO->getIterator();
		while($oidI->valid()){
			$objid = $oidI->current();
			$deleteS = $db->prepare('DELETE FROM as4_objectives WHERE id = ?');
			$deleteS->bind_param('i', $objid);
			$deleteS->execute();
			$deleteS->close();
			$deleteS = $db->prepare('DELETE FROM as4_g2o WHERE oid = ?');
			$deleteS->bind_param('i', $objid);
			$deleteS->execute();
			$deleteS->close();
			$deleteS = $db->prepare('DELETE FROM as4_s2o WHERE oid = ?');
			$deleteS->bind_param('i', $objid);
			$deleteS->execute();
			$deleteS->close();
			$oidI->next();
		}
		//Update statements
		//Update text
		$NobjectiveO = new ArrayObject($Nobjective);
		$NobjectiveI = $NobjectiveO->getIterator();
		$OobjectiveO = new ArrayObject($Oobjective);
		$OobjectiveI = $OobjectiveO->getIterator();
		While($NobjectiveI->valid()){
			$old = $OobjectiveI->current();
			$new = $NobjectiveI->current();
			$update = $db->prepare('UPDATE as4_objectives SET Objective = ? WHERE Objective = ?');
			$update->bind_param('ss', $new, $old);
			$update->execute();
			$update->close();
			$NobjectiveI->next();
			$OobjectiveI->next();
		}
		//Update goals
		$objectiveIdI->rewind();		
		$OldAssociatedGoalsO = new ArrayObject($OldAssociatedGoals);
		$OldAssociatedGoalsI = $OldAssociatedGoalsO->getIterator();
		$CurrentAssociatedGoalsO = new ArrayObject($CurrentAssociatedGoals);
		$CurrentAssociatedGoalsI = $CurrentAssociatedGoalsO->getIterator();
		while($objectiveIdI->valid()){
			$currentOid = $objectiveIdI->current();
			if(!in_array($currentOid, $oid)){
					if(($CurrentAssociatedGoalsI->offsetExists($currentOid)) && ($OldAssociatedGoalsI->offsetExists($currentOid))){
						$insert = array_diff($CurrentAssociatedGoals[$currentOid], $OldAssociatedGoals[$currentOid]);
						$delete = array_diff($OldAssociatedGoals[$currentOid], $CurrentAssociatedGoals[$currentOid]);
					}
					else{
						$delete = array_diff($OldAssociatedGoals[$currentOid],array());
					}
					if(!empty($insert)){
						$insertO = new ArrayObject($insert);
						$insertI = $insertO->getIterator();
						while($insertI->valid()){
							$temp = $insertI->current();
							$insertS = $db->prepare('INSERT INTO as4_g2o (oid,gid) VALUES (?,?)');
							$insertS->bind_param('ii', $currentOid, $temp);
							$insertS->execute();
							$insertS->close();
							$insertI->next();
						}
					}
					if(!empty($delete)){
						$deleteO = new ArrayObject($delete);
						$deleteI = $deleteO->getIterator();
						while($deleteI->valid()){
							$temp = $deleteI->current();
							$deleteS = $db->prepare('DELETE FROM as4_g2o WHERE oid = ? and gid = ?');
							$deleteS->bind_param('ii', $currentOid, $temp);
							$deleteS->execute();
							$deleteS->close();
							$deleteI->next();
						}
					}
				}
			$objectiveIdI->next();
		}
		//Insert Statement
		if(trim($_POST['o_extra']['objective']) != 'Insert new Objective'){
			$new = trim($_POST['o_extra']['objective']);
			$add = $db->prepare('INSERT INTO as4_objectives (Objective) VALUES (?)');
			$add->bind_param('s', $new);
			$add->execute();
			$newId = $add->insert_id;
			$add->close();
			$add = $db->prepare('INSERT INTO as4_s2o (sid,oid) VALUES (?,?)');
			$add->bind_param('ii',$edit, $newId);
			$add->execute();
			$add->close();
			$goals2add = [];
			$columnCount = 0;
			while($columnCount < $ng){
				if(isset($_POST['o_extra'][$columnCount]['goals'])){
					$value = intval($_POST['o_extra'][$columnCount]['goals']);
					$goals2add[] = $value;
				}
				$columnCount++;
			}
			$goals2addO = new ArrayObject($goals2add);
			$goals2addI = $goals2addO->getIterator();
			while($goals2addI->valid()){
				$currentGid = $goals2addI->current();
				$add = $db->prepare('INSERT INTO as4_g2o (oid, gid) VALUES (?,?)');
				$add->bind_param('ii', $newId, $currentGid);
				$add->execute();
				$add->close();
				$goals2addI->next();
			}
		}
		$db->close();
		echo ('<meta http-equiv="refresh" content="0;url=main.php">');
	}
	else if($type == 2){

		// var_dump($_FILES['insert_upload']['size'] > 0);
		// var_dump($_FILES['insert_upload']['type']=="application/pdf");
		// var_dump($_POST['insert_name']=="ENTER NAME");

		//Scanner
		

		//delete


		//insert
		if($_FILES['insert_upload']['size'] > 0 && $_FILES['insert_upload']['type'] == "application/pdf" && $_POST['insert_name']!='ENTER NAME')
		{
			$passedOid = $_POST['files'];
			$fileName = $_FILES['insert_upload']['name'];
			$tmpName = $_FILES['insert_upload']['tmp_name'];
			$fileSize = $_FILES['insert_upload']['size'];
			$fileType = $_FILES['insert_upload']['type'];

			if(!get_magic_quotes_gpc()){
				$fileName = addslashes($fileName);
			}

			$name = $_POST['insert_name'];
			$type = $_POST['insert_type'];
			$belowe = $_POST['insert_be'];
			$abovee = $_POST['insert_ae'];
			$insertFile = $db->prepare('INSERT INTO as4_file (name, fileName, type, size, file, belowe, abovee) VALUES (?,?,?,?,?,?,?)');
			$holder = NULL;
			$insertFile->bind_param('ssiibii', $name, $fileName, $type, $fileSize, $holder, $belowe, $abovee);	
			$fp = fopen($tmpName, 'r');
			$insertFile->send_long_data(4,fread($fp, $fileSize));
			fclose($fp);
			$insertFile->execute();
			$newId = $insertFile->insert_id;
			$insertFile->close();
			$file2obj = $db->prepare('INSERT INTO as4_o2f (oid,fid) VALUES (?,?)');
			$file2obj->bind_param('ii', $passedOid, $newId);
			$file2obj->execute();
			$file2obj->close();
		}

		$db->close();
		echo ('<meta http-equiv="refresh" content="0;url=main.php">');
	}
?>
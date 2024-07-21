<?php

	$db = new mysqli('134.74.126.107', 'S1533621', '0189', 'S1533621');
	mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

	if($db->connect_errno > 0){
	    die('Unable to connect to database [' . $db->connect_error . ']');
	}

    //Get Service Name
    $serviceNameStatement = $db->prepare('SELECT Service FROM as2_services WHERE id  = ?');
    $idEq = $_GET["serviceId"];
    $serviceNameStatement->bind_param('i', $idEq);
    $serviceNameStatement->execute();
    $serviceNameStatement->bind_result($serviceName);
    $serviceNameStatement->fetch();
    $serviceId = $_GET["serviceId"];
    
    echo '<b><i><font size="15"/>'.'Objectives of [s'.$serviceId.':'.$serviceName.']'.'</b></i>'.'<table border="1" style="width:100%" align="center">';
    echo '<tr>'.'<th>'.'<b><u>OID</b></u>'.'</th>'.'<th>'.'<b>Objective Statement</b>'.'</th>'.'<th>'.'<b>Associated Goals</b>'.'</th>'.'</tr>';
    $serviceNameStatement->close();

    $objectiveIdsStatement = $db->prepare('SELECT * FROM as2_s2o WHERE SID = ?');
    $objectiveIdsStatement->bind_param('i', $serviceId);
    $objectiveIdsStatement->execute();
    $objectiveIdsStatement->bind_result($sid, $oid);
    $objectiveIdArray = [];
    while($objectiveIdsStatement->fetch()){
        $objectiveIdArray[] = $oid;
    }
    $objectiveIdsStatement->close();

    $objectiveObject = new ArrayObject($objectiveIdArray);
    $iterator = $objectiveObject->getIterator();
    $iterator->asort();
    while($iterator->valid()){
        echo '<tr>';
        echo '<td>'.'o'.$iterator->current().'</td>';
        echo '<td>';
        $objectiveStatement = $db->prepare('SELECT OBJECTIVE, NG FROM as2_objectives WHERE id = ?');
        $objectiveStatement->bind_param('i', $iterator->current());
        $objectiveStatement->execute();
        $objectiveStatement->bind_result($objectiveS, $NG);
        $objectiveStatement->fetch();
        echo $objectiveS.'</td>';
        $objectiveStatement->close();
        echo '<td>';
        $goalIdsStatement = $db->prepare('SELECT gid FROM as2_g2o WHERE oid = ?');
        $goalIdsStatement->bind_param('i', $iterator->current());
        $goalIdsStatement->execute();
        $goalIdsStatement->bind_result($gids);
        $i = 0;
        while($goalIdsStatement->fetch()){
            if($i+1 < $NG)
                echo 'g'.$gids.', ';
            else
                echo 'g'.$gids;
            $i = $i + 1;
        }
        $goalIdsStatement->close();
        echo '</td>';
        echo '</tr>';
        $iterator->next();
    }
    echo '</table>';
?>
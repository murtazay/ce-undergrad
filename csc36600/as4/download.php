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

	if(isset($_GET["id"])){
		$id = intval($_GET["id"]);
		$idStatement = $db->prepare('SELECT a.filename, a.size, a.file FROM as4_file a WHERE a.id = ?');
		$idStatement->bind_param('i',$id);
		$idStatement->execute();
		$idStatement->bind_result($fileName, $fileSize, $file);
		$idStatement->fetch();

		header("Content-length: $fileSize");
		header("Content-type: application/pdf");
		header("Content-Disposition: attachment; filename=$fileName");
		echo $file;
	}
	$db->close();
	exit;
?>
</body>
</html>
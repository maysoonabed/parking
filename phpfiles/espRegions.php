<?php

$conn = mysqli_connect("localhost","root","","parking");
$Uid =  isset($_POST['uid']) ? $_POST['uid'] : "";


if($_SERVER["REQUEST_METHOD"] == "POST"){	
 $sql = "SELECT `engname` FROM `regions` WHERE 1";
 $result = mysqli_query($conn, $sql);
 $x=0;
 if($result->num_rows>0){
    while($row = $result->fetch_assoc()){
     $json[$x] = $row;
	 $x++;
}
 $json["num"] = $x;

echo json_encode($json);
		
 mysqli_close($conn);
}
}
else {
    echo "fill all details! Thanks";
}
 

?>
 
<?php

$conn = mysqli_connect("localhost","root","","parking");
$Uid =  isset($_POST['uid']) ? $_POST['uid'] : "";


if($_SERVER["REQUEST_METHOD"] == "POST"){	
 $sql = "SELECT `region` FROM `drivers` WHERE `uid`='$Uid'";
 $result = mysqli_query($conn, $sql);

 if($result->num_rows>0){
    $row = mysqli_fetch_assoc($result);
	$json['hay']=1;
    $json['region']=$row['region'];
 }else        
	 $json['hay']=0;


 echo json_encode($json);
 mysqli_close($conn);
}

else {
    echo "fill all details! Thanks";
}
 

?>
 
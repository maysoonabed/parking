<?php
 $connect = mysqli_connect("localhost","root","","parking");

 $Uid = $_POST['uid'];

 $query = "SELECT * FROM `drivers` WHERE `uid`='$Uid'";
 $result = $connect->query($query);

 if($result->num_rows>0){
    $row = mysqli_fetch_assoc($result);
    $json['name']=$row['name'];
    $json['busType']=$row['busType'];
    $json['passengers']=$row['passengers'];
    $json['region']=$row['region'];
    $json['hay']=1;
 }else        $json['hay']=0;



 echo json_encode($json);
 mysqli_close($connect);

?>
<?php
 $connect = mysqli_connect("localhost","root","","parking");


    //$row = mysqli_fetch_assoc($result);
    //$json['arname']=$row['arname'];
    //$json['engname']=$row['engname'];

 $query = "SELECT * FROM `regions`";
 $result = $connect->query($query);
 if($result->num_rows>0)
    {   
        $spacecrafts=array();
        while($row=$result->fetch_array())
         {
            array_push($spacecrafts, array("arname"=>$row['arname'],"engname"=>$row['engname']));
            echo $spacecrafts;
         }
        //print(json_encode(array_reverse($spacecrafts)));
 }else
    {
        print(json_encode(array("PHP EXCEPTION : CAN'T RETRIEVE FROM MYSQL. ")));
     }
        
 echo json_encode($json);
 mysqli_close($connect);

?>
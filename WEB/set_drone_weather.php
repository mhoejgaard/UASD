<?php
    // Require DB config
    require('config.php');
    // Connect to database
    $con = mysqli_connect($mysql_host, $mysql_user, $mysql_pw, $mysql_db);
    if (!$con) {
        die("Connection failed: " . mysqli_connect_error());
    }

    $drone_id = $_POST['drone_id'];
    $weather = $_POST['weather'];

    $sql = "UPDATE `AED_drone_list` SET `weather` = '$weather' WHERE  `id` = '$drone_id'";
    $result = mysqli_query($con, $sql);          //query

    if(mysqli_affected_rows($con) == 1) {
        //die('Could not update data: ' . mysql_error());
        $array_out = array(
            "status" => 1,
        );
    } else {
        $array_out = array(
            "status" => 0,
        );
    }
    echo json_encode($array_out);

   mysqli_close($con);
?>

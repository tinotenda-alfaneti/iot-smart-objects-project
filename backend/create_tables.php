<?php


include "config.php";

class DataBaseClass
{
    private $con;

    // Constructor to initialize the $con variable
    function __construct($con)
    {
        $this->con = $con;
    }

	//--CREATE TABLE--//

    function create_tables() {
        $sql0 = "CREATE TABLE IF NOT EXISTS SmartObjects (
            smartObjectID INT AUTO_INCREMENT PRIMARY KEY,
            smartObjectName VARCHAR(255) NOT NULL,
            smartObjectLocation VARCHAR(255) NOT NULL
        )";

        $stmt0 = mysqli_prepare($this->con, $sql0);
        $res0 = mysqli_stmt_execute($stmt0);

        if (!$res0) {
            echo "Error in SmartObject table creation: " . mysqli_error($this->con);
            return false;
        }

        $sql1 = "CREATE TABLE IF NOT EXISTS Sensors (
                    sensorID INT AUTO_INCREMENT PRIMARY KEY,
                    sensorName VARCHAR(255) NOT NULL,
                    sensorType VARCHAR(255)
                )";
    
        $stmt1 = mysqli_prepare($this->con, $sql1);
        $res1 = mysqli_stmt_execute($stmt1);
    
        if (!$res1) {
            echo "Error in Sensor table creation: " . mysqli_error($this->con);
            return false;
        }
    
        $sql2 = "CREATE TABLE IF NOT EXISTS ReadingsData (
                    sensorDataID INT AUTO_INCREMENT PRIMARY KEY,
                    sensorID INT NOT NULL,
                    smartObjectID INT NOT NULL,
                    currentReading VARCHAR(20) NOT NULL,
                    recordedTime TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
                    FOREIGN KEY (sensorID) REFERENCES Sensors(sensorID),
                    FOREIGN KEY (smartObjectID) REFERENCES SmartObjects(smartObjectID)
                )";
    
        $stmt2 = mysqli_prepare($this->con, $sql2);
        $res2 = mysqli_stmt_execute($stmt2);
    
        if (!$res2) {
            echo "Error in SensorData table creation: " . mysqli_error($this->con);
            return false;
        }
    
        return true;
    }
	

}
?>
<?php


class DataBaseFunctions
{
    private $con;

    // Constructor to initialize the $con variable
    function __construct($con)
    {
        $this->con = $con;
    }

    function insertData($smartObjectName, $sensorName, $sensorType, $currentReading) {

          // Escape and quote the string values
        $sensorName = mysqli_real_escape_string($this->con, $sensorName);
        $sensorType = mysqli_real_escape_string($this->con, $sensorType);
        $smartObjectName = mysqli_real_escape_string($this->con, $smartObjectName);

        // Construct the SQL query with proper quoting
        $query = mysqli_query($this->con, "SELECT sensorID FROM Sensors WHERE sensorName = '$sensorName' AND sensorType = '$sensorType'");

        $data = mysqli_fetch_object($query);

        if ($data) {
            $fkID = $data->sensorID;
        } else {
            //simple use of prepared statements
            $sql='INSERT INTO Sensors (SensorName, SensorType) VALUES (?, ?)';
            $stmt = mysqli_prepare($this->con, $sql);
            mysqli_stmt_bind_param($stmt, "ss", $sensorName, $sensorType);

            $res=mysqli_stmt_execute($stmt);
            $fkID = $this->con->insert_id;

        }

        // Construct the SQL query with proper quoting
        $query1 = mysqli_query($this->con, "SELECT smartObjectID FROM SmartObjects WHERE smartObjectName = '$smartObjectName'");

        $data1 = mysqli_fetch_object($query1);
 
        $smartObjfkID = $data1->smartObjectID;


        // simple use of prepared statements
        $sql='INSERT INTO ReadingsData (smartObjectID, sensorID, currentReading) VALUES (?, ?, ?)';
        $stmt = mysqli_prepare($this->con, $sql);
        mysqli_stmt_bind_param($stmt, "iis", $smartObjfkID, $fkID, $currentReading);

        $res=mysqli_stmt_execute($stmt);
        return $res;
    }

    function addSmartObj($smartObjectName, $smartObjectLocation) {

        // Escape and quote the string values
        $smartObjectName = mysqli_real_escape_string($this->con, $smartObjectName);
        $smartObjectLocation = mysqli_real_escape_string($this->con, $smartObjectLocation);

        $sql='INSERT INTO SmartObjects (smartObjectName, smartObjectLocation) VALUES (?, ?)';
        $stmt = mysqli_prepare($this->con, $sql);
        mysqli_stmt_bind_param($stmt, "ss", $smartObjectName, $smartObjectLocation);

        $res=mysqli_stmt_execute($stmt);
        return $res;
    }

    function updateSmartObject($smartObjectName) {

        // Escape and quote the string values
        $smartObjectName = mysqli_real_escape_string($this->con, $smartObjectName);


        $sql='UPDATE SmartObjects SET smartObjectName=?';
        $stmt = mysqli_prepare($this->con, $sql);
        mysqli_stmt_bind_param($stmt, "s", $smartObjectName);

        $res=mysqli_stmt_execute($stmt);

        if ($res)
            echo json_encode(['message' => 'SensorData updated successfully...']);
        else 
            echo json_encode(['message' => 'Failure to update']);
        return $res;

    }

    function retrieve_all() {
        $query = mysqli_query($this->con, 'SELECT ReadingsData.*, SmartObjects.smartObjectName, SmartObjects.smartObjectLocation, Sensors.sensorName, Sensors.sensorType
                                           FROM ReadingsData
                                           JOIN SmartObjects ON ReadingsData.smartObjectID = SmartObjects.smartObjectID 
                                           JOIN Sensors ON Sensors.sensorID = ReadingsData.sensorID');
    
        $data = [];
    
        while ($row = mysqli_fetch_object($query)) {
            $data[] = $row;
        }
    
        return $data;
    }


    function retrieve_smo() {
        $query = mysqli_query($this->con, 'SELECT * FROM smartObjects');
    
        $data = [];
    
        while ($row = mysqli_fetch_object($query)) {
            $data[] = $row;
        }
    
        return $data;
    }

    function save_to_csv() {
        // Define the base directory where your project is located
        $baseDir = $_SERVER['DOCUMENT_ROOT'] . "/iot/smartObjectsWeb/";
    
        // Define the filename for temperature and light intensity data
        $tempFilePath = $baseDir . "sensor_data.csv";
        
        // Define the headings for temperature and light intensity data
        $tempHeadings = "Hour,Minute,SensorID,SmartObjectID,CurrentReading,RecordedTime,SensorType,SmartObjectLocation,SmartObjectName\n";
    
        // Check if the files exist, if not, write the headings along with the data
        if (!file_exists($tempFilePath)) {
            file_put_contents($tempFilePath, $tempHeadings);
        }
    
        // Fetch temperature and light intensity data
        $tempQuery = "SELECT 
            DATE_FORMAT(recordedTime, '%H') AS hour,
            DATE_FORMAT(recordedTime, '%i') AS minute,
            ReadingsData.sensorID,
            ReadingsData.smartObjectID,
            currentReading,
            recordedTime,
            Sensors.sensorType,
            smartObjectLocation,
            smartObjectName
            FROM ReadingsData
            JOIN SmartObjects ON ReadingsData.smartObjectID = SmartObjects.smartObjectID 
            JOIN Sensors ON Sensors.sensorID = ReadingsData.sensorID 
            WHERE sensorType IN ('Temperature', 'Light Intensity', 'Humidity')";
    
        $tempResult = $this->con->query($tempQuery);
    
        // Write temperature and light intensity data to file
        if ($tempResult->num_rows > 0) {
            $tempFile = fopen($tempFilePath, "a");
            while ($row = $tempResult->fetch_assoc()) {
                fputcsv($tempFile, $row);
            }
            fclose($tempFile);
            echo "Data exported successfully.\n";
        } else {
            echo "No data found.\n";
        }
    }

}

?>
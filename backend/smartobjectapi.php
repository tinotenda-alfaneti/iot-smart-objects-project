<?php

// Allow requests from any origin during development
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: GET, PUT, POST, OPTIONS");
header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");


include "config.php";
include "create_tables.php";
include "db_functions.php";

$tableName = "sensordata";
$db_class = new DataBaseClass($con);
$db_functions = new DataBaseFunctions($con);
$db_class->create_tables();

// Handle HTTP methods
$method = $_SERVER['REQUEST_METHOD'];
$requestUri = $_SERVER['REQUEST_URI'];
switch ($method) {

  case 'GET':

    if (strpos($requestUri, '/smo') !== false) {
      // Request for all items
      $data = $db_functions->retrieve_smo();

      http_response_code(200);
      echo json_encode($data);

    } else if (strpos($requestUri, '/save') !== false) {
      $db_functions->save_to_csv();
    } else if (strpos($requestUri, '/retrive25') !== false) {
        $data = [];
          // Retrieve all records without LIMIT
          $result = $con->query('SELECT ReadingsData.*, SmartObjects.smartObjectName, SmartObjects.smartObjectLocation, Sensors.sensorName, Sensors.sensorType
          FROM ReadingsData JOIN SmartObjects ON ReadingsData.smartObjectID = SmartObjects.smartObjectID 
          JOIN Sensors ON Sensors.sensorID = ReadingsData.sensorID  WHERE Sensors.sensorType in ("Light Intensity")  ORDER BY recordedTime DESC LIMIT 25');
          while ($row = $result->fetch_assoc()) {
              $data[] = $row;
          }
          http_response_code(200);
          echo json_encode($data);
    } else {
        // Request for all items
        $data = $db_functions->retrieve_all();

        http_response_code(200);
        echo json_encode($data);
    }
    break;

  case 'POST':
    /**
     * TEST JSON
     * {"smartObjectName":"smartieOut", "smartObjectLocation":"OutsideRB100"}
     * {"smartObjectName":"smartieOut", "sensorName":"Temp101", "sensorType":"Temperature", "currentReading":"30.12}
     */
    
    if (strpos($requestUri, '/add') !== false) {

      $data = json_decode(file_get_contents('php://input'), true);
      $smartObjectName = $_POST['smartObjectName'];
      $smartObjectLocation = $_POST['smartObjectLocation'];
      

      $res = $db_functions->addSmartObj($smartObjectName, $smartObjectLocation);
      if ($res)
        echo json_encode(['message' => 'SmartObject added successfully...']);
      else 
        echo json_encode(['message' => 'Failure to add data']);


    } else {


      $data = json_decode(file_get_contents('php://input'), true);
      $smartObjectName = $data['smartObjectName'];
      $sensorName = $data['sensorName'];
      $sensorType = $data['sensorType'];
      $currentReading = $data['currentReading'];
      
      $res = $db_functions->insertData($smartObjectName, $sensorName, $sensorType, $currentReading);
      if ($res)
        echo json_encode(['message' => 'ReadingsData added successfully...']);
      else 
        echo json_encode(['message' => 'Failure to add data']);

    }
    break;

  case 'PUT':
      
      if (strpos($requestUri, '/updateName') !== false) {
  
        $data = json_decode(file_get_contents('php://input'), true);
        $smartObjectName = $data['smartObjectName'];

          
        $res = $db_functions->updateSmartObject($smartObjectName);
        if ($res)
          echo json_encode(['message' => 'SmartObject added successfully...']);
        else 
          echo json_encode(['message' => 'Failure to add data']);
  
  
      } else {
  
  
        $data = json_decode(file_get_contents('php://input'), true);
        $smartObjectName = $data['smartObjectName'];
        $sensorName = $data['sensorName'];
        $sensorType = $data['sensorType'];
        $currentReading = $data['currentReading'];
        
        $res = $db_functions->insertData($smartObjectName, $sensorName, $sensorType, $currentReading);
        if ($res)
          echo json_encode(['message' => 'ReadingsData added successfully...']);
        else 
          echo json_encode(['message' => 'Failure to add data']);
  
      }

      break;
  
}

?>
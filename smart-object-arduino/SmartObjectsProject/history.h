char historyPage[] PROGMEM = R"(
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Sensor Data</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #1F1C2C;
            color: #fff;
            margin: 0;
            padding: 0;
        }

        .navbar {
            background-color: #333;
            overflow: hidden;
        }

        .navbar a {
            float: left;
            display: block;
            color: #fff;
            text-align: center;
            padding: 14px 20px;
            text-decoration: none;
        }

        .navbar a:hover {
            background-color: #ddd;
            color: #333;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }

        th, td {
            padding: 8px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }

        th {
            background-color: #333;
            color: #fff;
        }

        tr:nth-child(even) {
            background-color: #292734;
        }
    </style>
</head>
<body>

    <div class='navbar'>
        <a href='/'>Home</a>
        <a href='/history'>View History</a>
        <a href='/config'>Config</a>
    </div>

    <h2>Sensor Data</h2>

    <table id="sensorDataTable">
        <tr>
            <th>SensorName</th>
            <th>SensorType</th>
            <th>SensorReading</th>
            <th>SmartObjectName</th>
            <th>TimeRead</th>
        </tr>
    </table>

    <script>
        function loadSensorData() {
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/getSensorData', true);
            xhr.onload = function() {
                if (xhr.status === 200) {
                    var sensorData = JSON.parse(xhr.responseText);
                    displaySensorData(sensorData);
                }
            };
            xhr.send();
        }

        function displaySensorData(sensorData) {
            var tableBody = document.getElementById("sensorDataTable");
            tableBody.innerHTML = ""; // Clear existing data

            sensorData.forEach(function(data) {
                var row = tableBody.insertRow();
                var cell1 = row.insertCell(0);
                var cell2 = row.insertCell(1);
                var cell3 = row.insertCell(2);
                var cell4 = row.insertCell(3);
                var cell5 = row.insertCell(4);
                cell1.innerHTML = data.sensorName;
                cell2.innerHTML = data.sensorType;
                cell3.innerHTML = data.currentReading;
                cell4.innerHTML = data.smartObjectName;
                cell5.innerHTML = new Date().toISOString(); // Current timestamp
            });
        }

        // Load sensor data on page load
        loadSensorData();

        // Refresh sensor data every 3 seconds
        setInterval(function() {
            loadSensorData();
        }, 3000);
    </script>

</body>
</html>
)";

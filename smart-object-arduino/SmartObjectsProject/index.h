char indexPage[] PROGMEM = R"(
<!DOCTYPE html>
<html lang='en'>

<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Smart Object Dashboard</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #1F1C2C;
            color: #fff;
            margin: 0;
            padding: 0;
        }

        .container {
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background-color: #292734;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        h2 {
            text-align: center;
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

        .sensor-readings {
            text-align: center;
            margin-bottom: 20px;
        }

        .sensor-readings div {
            display: inline-block;
            margin: 0 20px;
        }

        .button-row {
            text-align: center;
            margin-bottom: 20px;
        }

        .button-row button {
            padding: 10px 20px;
            background-color: #cc3000;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin: 0 10px;
        }

        .button-row button:hover {
            background-color: #fff;
            color: #cc3000;
        }

        #ldr-data {
            text-align: center;
        }

        #ldr-data table {
            width: 100%;
            border-collapse: collapse;
        }

        #ldr-data table,
        #ldr-data th,
        #ldr-data td {
            border: 1px solid #ddd;
            padding: 8px;
        }

        #ldr-data th {
            background-color: #333;
            color: #fff;
        }

        #ldr-data tr:nth-child(even) {
            background-color: #f2f2f2;
        }
    </style>
</head>

<body>

    <div class='navbar'>
        <a href='/'>Home</a>
        <a href='/history'>View History</a>
        <a href='/config'>Config</a>
    </div>

    <div class='container'>
        <h2>Smart Object Dashboard</h2>

        <div class='sensor-readings'>
            <div id='temperature'>Temperature: --</div>
            <div id='humidity'>Humidity: --</div>
            <div id='light-intensity'>Light Intensity: --</div>
        </div>

        <div class='button-row'>
            <button id='start-fan'>Start Fan</button>
            <button id='stop-fan'>Stop Fan</button>
            <button id='show-ldr-data' onclick='fetchData()'>Show Last 25 LDR Data</button>
        </div>

        <div id='ldr-data' style='display: none;'>
            <h3>Last 25 LDR Data</h3>
            <table>
                <thead>
                    <tr>
                        <th>SensorName</th>
                        <th>SensorType</th>
                        <th>CurrentReading</th>
                        <th>SmartObjectName</th>
                        <th>Location</th>
                        <th>Timestamp</th>
                    </tr>
                </thead>
                <tbody id='ldr-data-body'>
                    <!-- LDR data will be inserted here -->
                </tbody>
            </table>
        </div>
    </div>

    <script>
        // Function to update sensor readings every 5 seconds
        function updateSensorReadings() {
            // Simulated sensor readings

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function()
            {
              if(this.readyState == 4 && this.status == 200)
                var values = this.responseText.split(",");
                console.log(values);
                
                // Extract temperature and humidity values
                var temperature = values[0];
                var humidity = values[1];
                var lightIntensity = values[2];
                // Update sensor readings on the page
                document.getElementById('temperature').innerText = 'Temperature: ' + temperature;
                document.getElementById('humidity').innerText = 'Humidity: ' + humidity;
                document.getElementById('light-intensity').innerText = 'Light Intensity: ' + lightIntensity;
            };
            xhttp.open('GET', '/currentReading', true);
            xhttp.send();

        }

        setInterval(fetchData, 5000);
        function fetchData() {
            fetch("http://192.168.195.170/iot/smartObjectsWeb/smartobjectapi.php/retrive25")
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.json();
            })
            .then(data => {
                if (data.length === 0) {
                    console.log('No data received');
                    return;
                }
                // Show the table
                document.getElementById('ldr-data').style.display = 'table';
        
                // Clear previous data
                const dataBody = document.getElementById('ldr-data-body');
                dataBody.innerHTML = '';
        
                // Populate table with new data
                data.forEach(row => {
                    const newRow = document.createElement('tr');
                    newRow.innerHTML = `
                        <td>${row.sensorName}</td>
                        <td>${row.sensorType}</td>
                        <td>${row.currentReading}</td>
                        <td>${row.smartObjectName}</td>
                        <td>${row.smartObjectLocation}</td>
                        <td>${row.recordedTime}</td>
                    `;
                    dataBody.appendChild(newRow);
                });
            })
            .catch(error => console.error('Error fetching data:', error));
        }

        // Update sensor readings every 5 seconds
        setInterval(updateSensorReadings, 500);

        // Event listeners for button clicks
        document.getElementById('start-fan').addEventListener('click', startFan);
        document.getElementById('stop-fan').addEventListener('click', stopFan);

    // Function to start the fan
    function startFan() {
        fetch('/startFan')
            .then(response => {
                if (response.ok) {
                    alert('Fan started successfully');
                } else {
                    alert('Error starting fan - check mode');
                }
            })
            .catch(error => {
                console.error('Request failed:', error);
            });
    }
    
    // Function to stop the fan
    function stopFan() {
        fetch('/stopFan')
            .then(response => {
                if (response.ok) {
                    alert('Fan stopped successfully');
                } else {
                    alert('Error stopping fan - check mode');
                }
            })
            .catch(error => {
                console.error('Request failed:', error);
            });
    }
    </script>
</body>

</html>
)";

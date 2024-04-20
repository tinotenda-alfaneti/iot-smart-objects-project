char configPage[] PROGMEM = R"(

<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Smart Object Configuration</title>
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

        .container {
            max-width: 600px;
            margin: 50px auto;
            padding: 20px;
            background-color: #292734;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        h2 {
            text-align: center;
        }

        label {
            display: block;
            margin-bottom: 10px;
            font-weight: bold;
        }

        input[type='text'],
        input[type='number'] {
            width: 100%;
            padding: 10px;
            margin-bottom: 20px;
            border: 1px solid #ccc;
            border-radius: 5px;
            box-sizing: border-box;
        }

        input[type='checkbox'] {
            display: none;
        }

        .switch-container {
            display: flex;
            align-items: center;
            justify-content: space-between;
            margin-bottom: 20px;
        }

        .switch-container label {
            width: 100px;
            text-align: center;
        }

        .switch {
            position: relative;
            width: 60px;
            height: 34px;
        }

        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }

        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            -webkit-transition: .4s;
            transition: .4s;
            border-radius: 34px;
        }

        .slider:before {
            position: absolute;
            content: '';
            height: 26px;
            width: 26px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            -webkit-transition: .4s;
            transition: .4s;
            border-radius: 100%;
        }

        .slider:before {
            position: absolute;
            content: '';
            height: 26px;
            width: 50px;
            left: 12px; /* Adjust this value as needed */
            bottom: 4px;
            background-color: white;
            -webkit-transition: .4s;
            transition: .4s;
            border-radius: 100%;
        }

        input:checked + .slider:before {
            -webkit-transform: translateX(30px); /* Adjust this value to move the slider to the end */
            -ms-transform: translateX(30px); /* Adjust this value to move the slider to the end */
            transform: translateX(30px); /* Adjust this value to move the slider to the end */
            -webkit-transform: translateY(30px); /* Adjust this value to move the slider to the end */
            -ms-transform: translateY(30px); /* Adjust this value to move the slider to the end */
            transform: translateY(30px); /* Adjust this value to move the slider to the end */
        }

        input:checked + .slider {
            background-color: #cc3000;
        }

        input:focus + .slider {
            box-shadow: 0 0 1px #cc3000;
        }

        input:checked + .slider:before {
            -webkit-transform: translateX(26px);
            -ms-transform: translateX(26px);
            transform: translateX(26px);
        }

        /* Rounded sliders */
        .slider.round {
            border-radius: 34px;
        }

        .slider.round:before {
            border-radius: 100%;
        }

        button {
            width: 100%;
            padding: 10px 20px;
            background-color: #cc3000;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }

        button:hover {
            background-color: #fff;
            color: #cc3000;
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
        <h2>Smart Object Configuration</h2>
        <form id='configForm'>
            <label for='smartObjectName'>Smart Object Name:</label>
            <input type='text' id='smartObjectName' name='smartObjectName'>
        
            <label for='ssid'>SSID:</label>
            <input type='text' id='ssid' name='ssid'>
        
            <label for='triggerTemperature'>Trigger Temperature:</label>
            <input type='number' id='triggerTemperature' name='triggerTemperature' min='0' step='1'>
        
            <div class='switch-container'>
                <label>MQTT</label>
                <label class='switch'>
                    <input type='checkbox' id='communicationMode' name='communicationMode'>
                    <span class='slider round'></span>
                </label>
                <label>HTTP</label>
            </div>
        
            <div class='switch-container'>
                <label>AUTO</label>
                <label class='switch'>
                    <input type='checkbox' id='fanControl' name='fanControl'>
                    <span class='slider round'></span>
                </label>
                <label>MANUAL</label>
            </div>
        
            <button type='button' onclick='saveConfig()'>Save Configuration</button>
        </form>
    </div>
     <script>
        function saveConfig() {
            // Get form data
            var formData = new FormData(document.getElementById('configForm'));
    
            // Send AJAX request to server
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/updateConfig');
            xhr.onload = function() {
                if (xhr.status === 200) {
                    console.log('Configuration updated successfully');
                    location.reload();
                } else {
                    console.error('Error updating configuration:', xhr.statusText);
                }
            };
            xhr.onerror = function() {
                console.error('Request failed');
            };
            xhr.send(formData);
        }
    
            function fetchConfig() {
            var xhr = new XMLHttpRequest();
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var config = JSON.parse(xhr.responseText);
                        document.getElementById('smartObjectName').value = config.smartObjectName;
                        document.getElementById('ssid').value = config.ssid;
                        document.getElementById('triggerTemperature').value = config.triggerTemperature;
                        document.getElementById('communicationMode').checked = config.communicationMode;
                        document.getElementById('fanControl').checked = config.fanControl;
                    } else {
                        console.error('Error fetching configuration:', xhr.statusText);
                    }
                }
            };
            xhr.open('GET', '/fetchConfig', true);
            xhr.send();
        }
    
    
          // Fetch the latest configuration when the page is loaded
        window.onload = function() {
            fetchConfig();
        };
     </script>
    </body>
    </html>
    )";

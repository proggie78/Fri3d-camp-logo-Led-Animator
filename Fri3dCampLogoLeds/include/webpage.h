#ifndef WEBPAGE_H
#define WEBPAGE_H

// HTML for the control page
const char* HTML_CODE = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 LED Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: #f4f6f8;
      margin: 0;
      padding: 0;
      display: flex;
      justify-content: center;
      align-items: flex-start;
      min-height: 100vh;
    }

    .container {
      background: #fff;
      padding: 30px 40px;
      margin-top: 50px;
      border-radius: 15px;
      box-shadow: 0 6px 15px rgba(0,0,0,0.1);
      text-align: center;
      max-width: 500px;
      width: 100%;
    }

    h1 {
      margin-bottom: 20px;
      color: #333;
      font-size: 1.8em;
    }

    .effect-group {
      margin-bottom: 25px;
      text-align: left;
    }

    .effect-group label {
      margin-left: 8px;
      font-size: 1.1em;
      color: #444;
    }

    .effect-option {
      margin: 10px 0;
      display: flex;
      align-items: center;
    }
    
    .color-picker {
      margin-left: 30px;
      margin-top: 8px;
    }

    .slider-container {
      display: flex;
      justify-content: center;
      align-items: center;
      margin: 20px 0 10px 0;
    }

    .slider-label {
      margin: 0 15px;
      font-size: 1em;
      color: #555;
    }

    .slider {
      width: 250px;
      height: 8px;
      border-radius: 5px;
      background: #ddd;
      outline: none;
      -webkit-appearance: none;
    }

    .slider:hover {
      background: #ccc;
    }

    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 20px;
      height: 20px;
      border-radius: 50%;
      background: #4CAF50;
      cursor: pointer;
      border: none;
      box-shadow: 0 2px 5px rgba(0,0,0,0.2);
    }

    #speedValue {
      margin-bottom: 25px;
      font-size: 1.1em;
      color: #333;
    }

    .button {
      background: #4CAF50;
      border: none;
      color: white;
      padding: 14px 28px;
      border-radius: 8px;
      font-size: 16px;
      cursor: pointer;
      transition: background 0.3s ease;
    }

    .button:hover {
      background: #45a049;
    }

    #animations-list {
      list-style-type: none;
      padding: 0;
      text-align: left;
    }
    
    #animations-list li {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin: 5px 0;
    }

    #animations-list .animation-name-button {
      background: none;
      border: none;
      color: #4CAF50;
      cursor: pointer;
      font-size: 1em;
      text-decoration: underline;
    }

    #animations-list .animation-name-button:hover {
      color: #45a049;
    }
    
    #animations-list .delete-button {
      background: #f44336;
      border: none;
      color: white;
      padding: 5px 10px;
      border-radius: 5px;
      cursor: pointer;
    }
    
    #animations-list .delete-button:hover {
      background: #d32f2f;
    }

    .upload-section {
      margin-top: 25px;
      padding-top: 25px;
      border-top: 1px solid #ddd;
      text-align: left;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Fri3dCamp LED Control</h1>

    <div class="effect-group">
      Color: <input type="color" id="colorPicker" class="color-picker" value="#ff0000" onchange="setColor(this.value)">
      <div class="effect-option">
        <input type="radio" id="solid" name="effect" value="SOLID" onchange="selectEffect(this.value)">
        <label for="solid">Solid</label>
      </div>

      <div class="effect-option">
        <input type="radio" id="chase" name="effect" value="CHASE" onchange="selectEffect(this.value)">
        <label for="chase">Chase</label>
      </div>
      <p>Set the speed of the lighting effect (delay in ms)</p>
      <div class="slider-container">
        <span class="slider-label">Slow</span>
        <input type="range" min="1" max="200" value="40" class="slider" id="speedRange">
        <span class="slider-label">Fast</span>
      </div>
      <p id="speedValue" style="text-align: center">Delay: 40ms</p>
    
      <div class="effect-option">
        <input type="radio" id="litpart" name="effect" value="LITPART" onchange="selectEffect(this.value)">
        <label for="litpart">lit part</label>
      </div>
      From <input type="number" id="litfrom" value="LITFROM">
      To <input type="number" id="litto" value="LITTO">
      <button class="button" onclick="litPart()">Update</button>
    </div>

    <div class="effect-option">
      <input type="radio" id="fire" name="effect" value="ANIMATION_FIRE" onchange="selectEffect(this.value)">
      <label for="fire">Fire</label>
    </div>
    <div class="effect-option">
      <input type="radio" id="leftright" name="effect" value="ANIMATION_LEFT_RIGHT" onchange="selectEffect(this.value)">
      <label for="leftright">Left to right</label>
    </div>
    
    <div class="effect-option">
      <input type="radio" id="uploadedAnimationRadio" name="effect" value="UPLOADED_ANIMATION" onchange="selectEffect(this.value)">
      <label for="uploadedAnimationRadio">Uploaded Animation</label>
    </div>

    <div class="upload-section">
      <h2>Upload New Animation</h2>
      <input type="file" id="animationFile" accept=".bin">
      <button class="button" onclick="uploadAnimation()">Upload & Save</button>
    </div>

    <div class="upload-section">
      <h2>Select Animation</h2>
      <button class="button" onclick="listAnimations()">Refresh Animations</button>
      <ul id="animations-list"></ul>
    </div>

    <div class="upload-section">
      <h2>Console Output</h2>
      <pre id="consoleOutput" style="background:#222;color:#eee;padding:10px;border-radius:8px;max-height:200px;overflow:auto;text-align:left;"></pre>
    </div>
  </div>

  <script>
    var slider = document.getElementById("speedRange");
    var output = document.getElementById("speedValue");
    var colorPicker = document.getElementById("colorPicker");

    // Slider update
    slider.oninput = function() {
      var value = this.value;
      output.innerHTML = "Delay: " + value + "ms";
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_speed?value=" + value, true);
      xhttp.send();
    }

    // Send effect selection
    function selectEffect(effect) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_effect?value=" + effect, true);
      xhttp.send();
    }

    // Send chosen color
    function setColor(color) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_color?value=" + color.substring(1), true); // send hex without #
      xhttp.send();
    }

    function litPart() {
      var valueFrom = document.getElementById("litfrom").value;
      var valueTo = document.getElementById("litto").value;
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_litpart?valueFrom=" + valueFrom + "&valueTo=" + valueTo, true);
      xhttp.send();
    }
    
    // New functions for animation management

    function uploadAnimation() {
      const fileInput = document.getElementById('animationFile');
      const file = fileInput.files[0];
      if (!file) {
        alert("Please select a file to upload.");
        return;
      }
      
      const formData = new FormData();
      formData.append('animationFile', file, file.name);

      fetch('/upload_animation', {
        method: 'POST',
        body: formData
      })
      .then(response => response.text())
      .then(result => {
        alert(result);
        listAnimations();
      })
      .catch(error => {
        console.error('Error:', error);
        alert('Upload failed!');
      });
    }

    function listAnimations() {
      fetch('/list_animations')
        .then(response => response.json())
        .then(animations => {
          const list = document.getElementById('animations-list');
          list.innerHTML = '';
          animations.forEach(fileName => {
            const li = document.createElement('li');
            const playButton = document.createElement('button');
            playButton.textContent = fileName;
            playButton.className = 'animation-name-button';
            playButton.onclick = () => selectAnimation(fileName);
            li.appendChild(playButton);

            const deleteButton = document.createElement('button');
            deleteButton.textContent = 'Delete';
            deleteButton.className = 'delete-button';
            deleteButton.onclick = () => deleteAnimation(fileName);
            li.appendChild(deleteButton);

            list.appendChild(li);
          });
        })
        .catch(error => console.error('Error listing animations:', error));
    }

    function selectAnimation(fileName) {
      fetch('/select_animation?filename=' + fileName)
        .then(response => response.text())
        .then(result => {
          alert('Playing ' + fileName + '!');
          // Set the radio button for the uploaded animation effect
          document.getElementById('uploadedAnimationRadio').checked = true;
          selectEffect('UPLOADED_ANIMATION');
        })
        .catch(error => console.error('Error selecting animation:', error));
    }
    
    function deleteAnimation(fileName) {
      if (confirm(`Are you sure you want to delete ${fileName}?`)) {
        fetch('/delete_animation?filename=' + fileName)
          .then(response => response.text())
          .then(result => {
            alert(result);
            listAnimations();
          })
          .catch(error => console.error('Error deleting animation:', error));
      }
    }
    
    // Call listAnimations on page load
    window.onload = function() {
      listAnimations();
      fetchLogs();
    };

    // Fetch logs from /logs endpoint and update consoleOutput
    function fetchLogs() {
      fetch('/logs')
        .then(response => response.json())
        .then(logs => {
          document.getElementById('consoleOutput').textContent = logs.join('\n');
        })
        .catch(error => {
          document.getElementById('consoleOutput').textContent = 'Error fetching logs.';
        });
      setTimeout(fetchLogs, 5000); // Refresh every 5 seconds
    }
  </script>
</body>
</html>
)rawliteral";

#endif

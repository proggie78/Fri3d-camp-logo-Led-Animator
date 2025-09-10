#ifndef WEBPAGE_H
#define WEBPAGE_H

// HTML for the control page
const char* HTML_CODE = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Fri3dCamp LED Control</title>
  <style>
    :root {
      --primary-color: #4CAF50;
      --secondary-color: #f44336;
      --background-color: #f0f2f5;
      --card-background: #ffffff;
      --text-color: #333;
      --border-color: #e0e0e0;
    }
    body {
      font-family: 'Arial', sans-serif;
      background-color: var(--background-color);
      color: var(--text-color);
      display: flex;
      justify-content: center;
      padding: 20px;
    }
    .container {
      width: 100%;
      max-width: 700px;
      display: flex;
      flex-direction: column;
      gap: 20px;
    }
    .header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding-bottom: 20px;
      border-bottom: 2px solid var(--border-color);
    }
    .header h1 {
      margin: 0;
      font-size: 2em;
    }
    .power-button {
      background-color: var(--secondary-color);
      color: white;
      border: none;
      padding: 10px 20px;
      font-size: 1em;
      border-radius: 8px;
      cursor: pointer;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      transition: background-color 0.3s ease;
    }
    .power-button.on {
      background-color: var(--primary-color);
    }
    .power-button:hover {
      filter: brightness(1.1);
    }
    .card {
      background-color: var(--card-background);
      border-radius: 10px;
      padding: 20px;
      box-shadow: 0 4px 8px rgba(0,0,0,0.1);
    }
    .card-title {
      font-size: 1.5em;
      margin-bottom: 15px;
      border-bottom: 1px solid var(--border-color);
      padding-bottom: 10px;
    }
    .control-group {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
      gap: 15px;
    }
    .control-item {
      display: flex;
      flex-direction: column;
      align-items: flex-start;
    }
    .button-group {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
    }
    .effect-button {
      background-color: #eee;
      border: 2px solid #ddd;
      color: var(--text-color);
      padding: 10px 15px;
      border-radius: 8px;
      cursor: pointer;
      transition: all 0.3s ease;
      flex-grow: 1;
    }
    .effect-button.active {
      background-color: var(--primary-color);
      color: white;
      border-color: var(--primary-color);
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    .effect-button:hover:not(.active) {
      background-color: #e0e0e0;
    }
    .input-group {
      display: flex;
      align-items: center;
      gap: 10px;
      margin-top: 10px;
    }
    .input-group input[type="number"] {
      width: 60px;
      padding: 8px;
      border-radius: 5px;
      border: 1px solid var(--border-color);
    }
    .slider-container {
      width: 100%;
      margin-top: 15px;
    }
    .slider {
      width: 100%;
    }
    .slider-label {
      font-size: 0.9em;
      color: #666;
      text-align: center;
      margin-top: 5px;
    }
    .animation-list {
      list-style-type: none;
      padding: 0;
      max-height: 200px;
      overflow-y: auto;
      border: 1px solid var(--border-color);
      border-radius: 5px;
      margin-top: 10px;
    }
    .animation-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 8px 15px;
      border-bottom: 1px solid #f0f0f0;
    }
    .animation-item:last-child {
      border-bottom: none;
    }
    .animation-name {
      font-weight: bold;
      flex-grow: 1;
    }
    .animation-item button {
      background-color: transparent;
      border: none;
      color: var(--primary-color);
      cursor: pointer;
    }
    .delete-button {
      color: var(--secondary-color) !important;
      margin-left: 10px;
    }
    .file-upload-section {
      margin-top: 15px;
    }
    .file-upload-section input[type="file"] {
      border: 1px solid var(--border-color);
      border-radius: 5px;
      padding: 8px;
      width: calc(100% - 18px);
    }
    .main-button {
      background-color: var(--primary-color);
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 8px;
      cursor: pointer;
      transition: background-color 0.3s ease;
      margin-top: 10px;
    }
    .main-button:hover {
      background-color: #45a049;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>Fri3dCamp LED Control</h1>
      <button id="power-button" class="power-button">Power</button>
    </div>

    <!-- Main Controls -->
    <div class="card">
      <div class="card-title">Main Controls</div>
      <div class="control-group">
        <div class="control-item">
          <label for="colorPicker">Solid Color</label>
          <input type="color" id="colorPicker" onchange="setColor(this.value)" value="#ff0000">
        </div>
        <div class="control-item">
          <label>Effects</label>
          <div class="button-group">
            <button class="effect-button" data-effect="SOLID" onclick="selectEffect('SOLID')">Solid</button>
            <button class="effect-button" data-effect="CHASE" onclick="selectEffect('CHASE')">Chase</button>
            <button class="effect-button" data-effect="ANIMATION_FIRE" onclick="selectEffect('ANIMATION_FIRE')">Fire</button>
            <button class="effect-button" data-effect="ANIMATION_LEFT_RIGHT" onclick="selectEffect('ANIMATION_LEFT_RIGHT')">Left/Right</button>
          </div>
        </div>
      </div>
      <div class="slider-container">
        <label>Speed</label>
        <input type="range" min="1" max="200" value="40" class="slider" id="speedRange">
        <div class="slider-label" id="speedValue">Delay: 40ms</div>
      </div>
    </div>
    
    <!-- Lit Part Controls -->
    <div class="card">
      <div class="card-title">Lit Part</div>
      <div class="input-group">
        <label>From</label>
        <input type="number" id="litfrom" value="0">
        <label>To</label>
        <input type="number" id="litto" value="10">
        <div>
        <button class="main-button" onclick="litPart()">Update</button>
        </div>
      </div>
    </div>

    <!-- Animations -->
    <div class="card">
      <div class="card-title">Animations</div>
      <button class="main-button" onclick="listAnimations()">Refresh Animations</button>
      <ul class="animation-list" id="animations-list"></ul>
      <div class="file-upload-section">
        <label>Upload New Animation (.bin)</label>
        <input type="file" id="animationFile" accept=".bin">
        <button class="main-button" onclick="uploadAnimation()">Upload & Save</button>
      </div>
    </div>
    <div class="upload-section">
      <h2>Console Output</h2>
      <pre id="consoleOutput" style="background:#222;color:#eee;padding:10px;border-radius:8px;max-height:200px;overflow:auto;text-align:left;"></pre>
    </div>
  </div>
  </div>

  <script>
    var slider = document.getElementById("speedRange");
    var output = document.getElementById("speedValue");
    var colorPicker = document.getElementById("colorPicker");
    var litColorPicker = document.getElementById("litColorPicker");
    var powerButton = document.getElementById("power-button");
    
    // Initial active state for a button
    document.querySelector('[data-effect="SOLID"]').classList.add('active');

    slider.oninput = function() {
      var value = this.value;
      output.innerHTML = "Delay: " + value + "ms";
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_speed?value=" + value, true);
      xhttp.send();
    }

    function selectEffect(effect) {
      document.querySelectorAll('.effect-button').forEach(btn => btn.classList.remove('active'));
      document.querySelector(`[data-effect="${effect}"]`).classList.add('active');

      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_effect?value=" + effect, true);
      xhttp.send();
    }

    function setColor(color) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_color?value=" + color.substring(1), true);
      xhttp.send();
    }

    function litPart() {
      var valueFrom = document.getElementById("litfrom").value;
      var valueTo = document.getElementById("litto").value;
      var litColor = litColorPicker.value.substring(1);
      
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/set_litpart?valueFrom=" + valueFrom + "&valueTo=" + valueTo + "&litColor=" + litColor, true);
      xhttp.send();
    }
    
    powerButton.onclick = function() {
      fetch('/toggle_power')
        .then(response => response.text())
        .then(result => {
          if (result === "ON") {
            powerButton.classList.add('on');
            // Re-select the last active effect or the default one
            let lastActiveBtn = document.querySelector('.effect-button.active');
            if (lastActiveBtn) {
              selectEffect(lastActiveBtn.dataset.effect);
            } else {
              selectEffect('SOLID');
            }
          } else {
            powerButton.classList.remove('on');
            selectEffect('OFF');
          }
        })
        .catch(error => console.error('Error toggling power:', error));
    };

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
            li.className = 'animation-item';
            
            const nameSpan = document.createElement('span');
            nameSpan.className = 'animation-name';
            nameSpan.textContent = fileName;
            li.appendChild(nameSpan);

            const playButton = document.createElement('button');
            playButton.textContent = 'Play';
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
          // Set the radio button for the uploaded animation effect
          document.querySelectorAll('.effect-button').forEach(btn => btn.classList.remove('active'));
          selectEffect('UPLOADED_ANIMATION');
        })
        .catch(error => console.error('Error selecting animation:', error));
    }
    
    function deleteAnimation(fileName) {
      if (confirm(`Are you sure you want to delete ${fileName}?`)) {
        fetch('/delete_animation?filename=' + fileName)
          .then(response => response.text())
          .then(result => {
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

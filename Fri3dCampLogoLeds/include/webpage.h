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
      <!-- Color picker always visible -->
      

      <div class="effect-option">
        <input type="radio" id="chase" name="effect" value="CHASE" onchange="selectEffect(this.value)">
        <label for="chase">Chase</label>
		<p>Set the speed of the lighting effect (delay in ms)</p>
      </div>
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
  </script>
</body>
</html>


)rawliteral";

#endif
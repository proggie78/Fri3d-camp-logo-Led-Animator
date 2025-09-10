#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <vector>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "webpage.h"
#include "led_animation_fire.h" // Include the generated animation data header
#include "led_animation_left_right.h" // Include the generated animation data header

// Your existing defines
#define LED_PIN     2
#define NUM_LEDS    379
#define LED_TYPE    WS2815
#define COLOR_ORDER RGB

CRGB leds[NUM_LEDS];
WebServer server(80);
File uploadedFile;

// Log buffer
std::vector<String> logBuffer;
const size_t LOG_BUFFER_MAX = 50;
void logMessage(const String& msg) {
    Serial.println(msg);
    logBuffer.push_back(msg);
    if (logBuffer.size() > LOG_BUFFER_MAX) logBuffer.erase(logBuffer.begin());
}

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

enum Effect {
    SOLID,
    CHASE,
    LITPART,
    ANIMATION_FIRE, // New ANIMATION_FIRE effect
    ANIMATION_LEFT_RIGHT,
    UPLOADED_ANIMATION // New effect for user-uploaded animations
};

// Global variables for control
volatile unsigned long led_interval = 20;
volatile bool lit_all = false;
volatile Effect led_effect = SOLID; // Default effect
CRGB currentColor = CRGB::White;

// Global variables for animation
int currentFrame = 0;
int numFrames = 60; // You will need to change this to the number of frames you generate
int uploadedNumFrames = 0;
uint8_t* uploadedAnimationData = nullptr;
String currentAnimationFile = "";

// Function prototypes
void loadAnimation(const String& fileName);

void handleRoot() {
    server.send(200, "text/html", HTML_CODE);
}

void handleSetSpeed() {
    if (server.hasArg("value")) {
        led_interval = 200 - server.arg("value").toInt();
        logMessage("New speed set: " + String(led_interval));
    }
    server.send(200, "text/plain", "OK");
}

void handleLitAll() {
    lit_all = true;
    logMessage("Lit all command received.");
    server.send(200, "text/plain", "OK");
}

void handleSetEffect() {
    if (server.hasArg("value")) {
        String effectStr = server.arg("value");
        if (effectStr == "SOLID") {
            led_effect = SOLID;
            FastLED.showColor(currentColor);
        } else if (effectStr == "CHASE") {
            led_effect = CHASE;
        } else if (effectStr == "LITPART") {
            led_effect = LITPART;
        } else if (effectStr == "ANIMATION_FIRE") {
            led_effect = ANIMATION_FIRE;
            currentFrame = 0;
        } else if (effectStr == "ANIMATION_LEFT_RIGHT") {
            led_effect = ANIMATION_LEFT_RIGHT;
            currentFrame = 0;
        } else if (effectStr == "UPLOADED_ANIMATION") {
            // This is handled by a separate endpoint that also passes the filename
            //logMessage("Please use the /select_animation endpoint to choose a file.");
            server.send(200, "text/plain", "OK");
            return;
        }
        logMessage("New effect set: " + effectStr);
    }
    server.send(200, "text/plain", "OK");
} 

void handleSetColor() {
    if (server.hasArg("value")) {
        String hexColor = server.arg("value");
        long number = strtol(hexColor.c_str(), NULL, 16);
        byte r = (number >> 16) & 0xFF;
        byte g = (number >> 8) & 0xFF;
        byte b = number & 0xFF;
        currentColor = CRGB(r, g, b); // Set global color
        for(int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(r, g, b);
        }
        FastLED.show();
        logMessage("New color set: #" + hexColor);
    }
    server.send(200, "text/plain", "OK");
}

void handleSetLitPart() {
    if (server.hasArg("valueFrom") && server.hasArg("valueTo")) {
        int valueFrom = server.arg("valueFrom").toInt();
        int valueTo = server.arg("valueTo").toInt();
        FastLED.clear(); // Clear all leds efficiently
        // Set the lit part of the LEDs
        for (int i = valueFrom; i <= valueTo && i < NUM_LEDS; i++) {
            leds[i] = CRGB::White;
        }
        FastLED.show();
        logMessage("New lit part set: " + String(valueFrom) + " to " + String(valueTo));
        server.send(200, "text/plain", "OK");
    }
    else {
        logMessage("Error: Missing valueFrom or valueTo parameter.");
        server.send(400, "text/plain", "Error: Missing valueFrom or valueTo parameter.");
    }
}

void handleUploadAnimation() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) {
            filename = "/" + filename;
        }
        Serial.printf("Receiving uploaded file: %s\n", filename.c_str());
        uploadedFile = LittleFS.open(filename, "w");
        if (!uploadedFile) {
            Serial.println("Failed to open file for writing");
            return;
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (uploadedFile) {
            uploadedFile.write(upload.buf, upload.currentSize);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (uploadedFile) {
            uploadedFile.close();
            Serial.printf("File upload complete. File size: %d bytes\n", upload.totalSize);
            server.sendHeader("Connection", "close");
            server.send(200, "text/plain", "Animation saved successfully!");
        } else {
            Serial.println("File handle lost during upload.");
            server.send(500, "text/plain", "File upload failed!");
        }
    }
}

void loadAnimation(const String& fileName) {
Serial.printf("Loading animation from file: %s\n", fileName.c_str());

    File file = LittleFS.open(fileName, "r", false);
    if (!file) {
        logMessage("Failed to open uploaded animation file");
        Serial.printf("Failed to open uploaded animation file: %s\n", fileName.c_str());
        uploadedNumFrames = 0;
        return;
    }

    if (uploadedAnimationData != nullptr) {
        free(uploadedAnimationData);
        uploadedAnimationData = nullptr;
    }
    Serial.println("File size2: " + String(file.size()));
    // Allocate memory for the animation data
    uploadedAnimationData = (uint8_t*)malloc(file.size());
    Serial.println("File size3: " + String(file.size()));
    if (uploadedAnimationData == nullptr) {
        logMessage("Failed to allocate memory for uploaded animation!");
        uploadedNumFrames = 0;
        file.close();
        return;
    }
    
    file.read(uploadedAnimationData, file.size());
    uploadedNumFrames = file.size() / (NUM_LEDS * 3);
    logMessage("File Size: " + String(file.size()) + " bytes");
    logMessage("Loaded animation '" + fileName + "' with " + String(uploadedNumFrames) + " frames.");
    Serial.printf("Loaded animation '%s' with %d frames.\n", fileName.c_str(), uploadedNumFrames);
    file.close();
}

void handleSelectAnimation() {
    if (server.hasArg("filename")) {
        String filename = server.arg("filename");
        
        // Find the position of " - " to strip the file size info
        int pos = filename.indexOf(" - ");
        if (pos > 0) {
            filename = filename.substring(0, pos);
        }

        Serial.printf("Selecting animation: %s\n", filename.c_str());
        currentAnimationFile = filename;
        led_effect = UPLOADED_ANIMATION;
        currentFrame = 0;
        loadAnimation("/" + filename);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Error: Missing filename parameter.");
    }
}

void handleDeleteAnimation() {
    if (!server.hasArg("filename")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }
    
    String filename = server.arg("filename");
        // Find the position of " - " to strip the file size info
    int pos = filename.indexOf(" - ");
    if (pos > 0) {
        filename = filename.substring(0, pos);
    }
    String fullPath = "/" + filename;
    if (LittleFS.exists(fullPath)) {
        if (LittleFS.remove(fullPath)) {
            server.send(200, "text/plain", "File deleted successfully!");
        } else {
            server.send(500, "text/plain", "Failed to delete file!");
        }
    } else {
        server.send(404, "text/plain", "File not found!");
    }
}

void handleListAnimations() {
    DynamicJsonDocument doc(1024);
    JsonArray animations = doc.to<JsonArray>();

    File root = LittleFS.open("/");
    File file = root.openNextFile();

    while (file) {
        logMessage("Found animation file: " + String(file.path()) + String(file.name()) + " - " + String(file.size()) + " bytes");
        animations.add(String(file.name()) + " - " + String(file.size()) + " bytes");
        file = root.openNextFile();
    }
    logMessage("Total size:" + String(LittleFS.totalBytes()) + " used bytes: " + String(LittleFS.usedBytes()));
    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
}

void setup() {
    Serial.begin(115200);
    ArduinoOTA.setHostname("fri3dcamplogo");

    // WiFi setup
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());

    // OTA setup
    ArduinoOTA.begin();

    // LED setup
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();

        // Add /logs endpoint
        server.on("/logs", []() {
            String logsJson = "[";
            for (size_t i = 0; i < logBuffer.size(); ++i) {
                logsJson += '"' + logBuffer[i] + '"';
                if (i < logBuffer.size() - 1) logsJson += ",";
            }
            logsJson += "]";
            server.send(200, "application/json", logsJson);
        });

    // LittleFS setup
    if (!LittleFS.begin(true)) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    // Web server setup
    server.on("/", handleRoot);
    server.on("/set_speed", handleSetSpeed);
    server.on("/set_effect", handleSetEffect);
    server.on("/set_color", handleSetColor);
    server.on("/set_litpart", handleSetLitPart);
    server.on("/upload_animation", HTTP_POST, [](){ server.send(200); }, handleUploadAnimation);
    server.on("/list_animations", handleListAnimations);
    server.on("/select_animation", handleSelectAnimation);
    server.on("/delete_animation", handleDeleteAnimation);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    ArduinoOTA.handle();
    server.handleClient();

    static int current = 0;
    static bool turningOn = true;
    static unsigned long lastUpdate = 0;
    FastLED.setBrightness(20);


    if(led_effect == SOLID)
    {
      delay(100); 
    }
    else if(led_effect == LITPART)
    {
      delay(100); 
    }
    else if(led_effect == CHASE)
    {
      if (millis() - lastUpdate > led_interval) {
          lastUpdate = millis();

          if (turningOn) {
              if (current < NUM_LEDS) {
                  leds[current] = CRGB::White;
                  FastLED.show();
                  current++;
              } else {
                  turningOn = false;
                  current = 0;
              }
          } else {
              FastLED.clear(); // Clear all leds efficiently
              FastLED.show();
              turningOn = true;
              current = 0;
          }
        }
    }
    else if (led_effect == ANIMATION_FIRE)
    {
      if (millis() - lastUpdate > led_interval)
      {
          lastUpdate = millis();
          FastLED.clear();
          for (int i = 0; i < NUM_LEDS; i++)
          {
              int index = (currentFrame * NUM_LEDS * 3) + (i * 3);
              leds[i].r = ledAnimationFire[index + 0];
              leds[i].g = ledAnimationFire[index + 1];
              leds[i].b = ledAnimationFire[index + 2];
          }

          FastLED.show();
          currentFrame = (currentFrame + 1) % 61;
      }
    }
    else if (led_effect == ANIMATION_LEFT_RIGHT)
    {
      if (millis() - lastUpdate > led_interval)
      {
          lastUpdate = millis();
          FastLED.clear();
          for (int i = 0; i < NUM_LEDS; i++)
          {
              int index = (currentFrame * NUM_LEDS * 3) + (i * 3);
              leds[i].r = ledAnimationLeftRight[index + 0];
              leds[i].g = ledAnimationLeftRight[index + 1];
              leds[i].b = ledAnimationLeftRight[index + 2];
          }

          FastLED.show();
          currentFrame = (currentFrame + 1) % 37;
      }
    }
    else if (led_effect == UPLOADED_ANIMATION) // Display the uploaded animation
    {
      if (uploadedAnimationData != nullptr && uploadedNumFrames > 0)
      {
        if (millis() - lastUpdate > led_interval)
        {
            lastUpdate = millis();
            FastLED.clear();
            for (int i = 0; i < NUM_LEDS; i++)
            {
                int index = (currentFrame * NUM_LEDS * 3) + (i * 3);
                leds[i].r = uploadedAnimationData[index + 0];
                leds[i].g = uploadedAnimationData[index + 1];
                leds[i].b = uploadedAnimationData[index + 2];
            }
            FastLED.show();
            currentFrame = (currentFrame + 1) % uploadedNumFrames;
        }
      } else {
        if (uploadedNumFrames==0) {
          FastLED.showColor(CRGB::Blue);
        }
        // If no animation is loaded, show a solid color
        FastLED.showColor(CRGB::Red);
        delay(100);
      }
    }
}

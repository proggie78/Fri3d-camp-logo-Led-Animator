#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include "secrets.h"
#include "webpage.h"

// Your existing defines
#define LED_PIN     2
#define NUM_LEDS    379
#define LED_TYPE    WS2815
#define COLOR_ORDER RGB

CRGB leds[NUM_LEDS];
WebServer server(80);

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

enum Effect {
  SOLID,
  CHASE,
  LITPART
};

// Global variables for control
volatile unsigned long led_interval = 20;
volatile bool lit_all = false;
volatile Effect led_effect = SOLID; // Default effect



void handleRoot() {
    server.send(200, "text/html", HTML_CODE);
}

void handleSetSpeed() {
    if (server.hasArg("value")) {
        led_interval = 200 - server.arg("value").toInt();
        Serial.print("New speed set: ");
        Serial.println(led_interval);
    }
    server.send(200, "text/plain", "OK");
}

void handleLitAll() {
    lit_all = true;
    Serial.println("Lit all command received.");
    server.send(200, "text/plain", "OK");
}

void handleSetEffect() {
    if (server.hasArg("value")) {
        String effectStr = server.arg("value");
        if (effectStr == "SOLID") {
            led_effect = SOLID;
        } else if (effectStr == "CHASE") {
            led_effect = CHASE;
        } else if (effectStr == "LITPART") {
            led_effect = LITPART;
        }
        Serial.print("New effect set: ");
        Serial.println(effectStr);
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
        for(int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(r, g, b);
        }
        FastLED.show();
        Serial.print("New color set: #");
        Serial.println(hexColor);
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
        Serial.print("New lit part set: ");
        Serial.print(valueFrom);
        Serial.print(" to ");
        Serial.println(valueTo);
        server.send(200, "text/plain", "OK");
    }
    else {
        Serial.println("Error: Missing valueFrom or valueTo parameter.");
        server.send(400, "text/plain", "Error: Missing valueFrom or valueTo parameter.");
    }
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

    // Web server setup
    server.on("/", handleRoot);
    server.on("/set_speed", handleSetSpeed);
    server.on("/set_effect", handleSetEffect);
    server.on("/set_color", handleSetColor);
    server.on("/set_litpart", handleSetLitPart);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    ArduinoOTA.handle();
    server.handleClient();

    static int current = 0;
    static bool turningOn = true;
    static unsigned long lastUpdate = 0;
    //FastLED.setBrightness(led_brightness);
    FastLED.setBrightness(20);

    // Check for "lit all" command first
    //if (lit_all) {
    //    FastLED.showColor(CRGB::White);
    //    lit_all = false; // Reset the flag
    //    turningOn = true; // Reset the normal animation
    //    current = 0;
    //} 
    //else 
    if(led_effect == SOLID)
    {
        FastLED.showColor(CRGB::White);
    }
    else if(led_effect == LITPART)
    {
      //handeld in the update of the part
      // do nothing here, the part is set via the webserver
      delay(10); // Allow time for processor to handle new requests
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
    }
 
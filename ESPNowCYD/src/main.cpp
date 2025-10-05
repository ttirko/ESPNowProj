//************************************************************************************
//* ESP Now CYD Responder                                   
//* Author: T2                                                                       
//* Company: T2Elektroteknik                                                         
//* Date: 10/04/25                                                                  
//* Description: Baseline Responder using CYD to display info       
//* CPU: ESP32 CYD                                                                    
//************************************************************************************
#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <WiFi.h>
#include "esp_now.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// Define a data structure
typedef struct struct_message {
  char devAddr;
  char devType;
  float humidity;
  float temperature;
} struct_message;
 
// Create a structured object
struct_message myData;

// Function Prototypes
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

/////////////////////////////////////////////////////////////////////////////
// Setup Function
/////////////////////////////////////////////////////////////////////////////
void setup(void) {
  Serial.begin(9600);
  tft.init();
  tft.setRotation(1);
  // Put ESP32 into Station mode
  //WiFi.mode(WIFI_MODE_STA);
 
  // Print MAC Address to Serial monitor
  //Serial.print("MAC Address: ");
  //Serial.println(WiFi.macAddress());
  // Start ESP32 in Station mode
  WiFi.mode(WIFI_STA);
 
  // Initalize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
   
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

/////////////////////////////////////////////////////////////////////////////
// Loop Forever
/////////////////////////////////////////////////////////////////////////////
void loop() {

  tft.fillScreen(TFT_BLACK);

  // Draw some random ellipses
  for (int i = 0; i < 40; i++)
  {
    int rx = random(60);
    int ry = random(60);
    int x = rx + random(320 - rx - rx);
    int y = ry + random(240 - ry - ry);
    tft.fillEllipse(x, y, rx, ry, random(0xFFFF));
  }

  delay(2000);
  tft.fillScreen(TFT_BLACK);

  for (int i = 0; i < 40; i++)
  {
    int rx = random(60);
    int ry = random(60);
    int x = rx + random(320 - rx - rx);
    int y = ry + random(240 - ry - ry);
    tft.drawEllipse(x, y, rx, ry, random(0xFFFF));
  }

  delay(2000);

  tft.fillScreen(TFT_RED);
  delay(2000);
  tft.fillScreen(TFT_GREEN);
  delay(2000);
  tft.fillScreen(TFT_BLUE);
  delay(2000);
}

// Callback function
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // Get incoming data
  memcpy(&myData, incomingData, sizeof(myData));
  
  // Print to Serial Monitor
  Serial.print("Temp: ");
  Serial.println(myData.temperature);
  
  Serial.print("Humidity: ");
  Serial.println(myData.humidity); 
}
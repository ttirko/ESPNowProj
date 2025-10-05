//************************************************************************************
//* ESP Now Sensor XIAO ESP32-C3                                   
//* Author: T2                                                                       
//* Company: T2Elektroteknik                                                         
//* Date: 10/04/25                                                                  
//* Description: Baseline Sensor initiator/sender       
//* CPU: XIAO ESP32-C3                                                                     
//************************************************************************************
#include <Arduino.h>
#include "DHT.h"
#include <WiFi.h>
#include "esp_now.h"

#define DHTPIN D2       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

// ESP Now Settings
// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0x7A, 0xAE, 0x7C};

#define DEVICE_ADDR 1 //<< Address you are assigning to this device
#define DEVICE_TYPE 0x01 //<< Device ID Temp/Humidity
 
// Define a data structure
typedef struct struct_message {
  char devAddr;
  char devType;
  float humidity;
  float temperature;
} struct_message;
 
// Create a structured object
struct_message myData;
 
// Peer info
esp_now_peer_info_t peerInfo;

// Function Prototypes
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

 
/////////////////////////////////////////////////////////////////////////////
// Setup Function
/////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  
  dht.begin();

    // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Loop Forever
/////////////////////////////////////////////////////////////////////////////
void loop() {
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(f);
  Serial.println(F("°F "));

  myData.devAddr = DEVICE_ADDR;
  myData.devType = DEVICE_TYPE;
  myData.humidity = h;
  myData.temperature = f;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  
  // Wait a few seconds between measurements.
  delay(2000);

}


// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
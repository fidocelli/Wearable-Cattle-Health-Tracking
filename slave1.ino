#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_MLX90614.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define KG011_PIN 34  // Analog pin for KG011 pulse sensor

// Structure to hold sensor data
typedef struct struct_message {
  float accelX;
  float accelY;
  float accelZ;
  float ambientTemp;
  float objectTemp;
  int pulseValue;
  uint8_t senderID; // Unique ID for each sender
} struct_message;

struct_message sensorData;

// MAC Address of the Receiver ESP32 (replace with the actual MAC address)
uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0xDE, 0xAD, 0xB2};  // Replace with actual MAC of the receiver

void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Delivery status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  // Initialize sensors
  if (!accel.begin()) {
    Serial.println("ADXL345 not detected.");
    while (1);
  }
  if (!mlx.begin()) {
    Serial.println("MLX90614 not detected.");
    while (1);
  }
  pinMode(KG011_PIN, INPUT);

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(onSent);

  // Add receiver as a peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}
void loop() {
  // Read sensor data
  sensors_event_t event;
  accel.getEvent(&event);
  sensorData.accelX = event.acceleration.x;
  sensorData.accelY = event.acceleration.y;
  sensorData.accelZ = event.acceleration.z;
  sensorData.ambientTemp = mlx.readAmbientTempC();
  sensorData.objectTemp = mlx.readObjectTempC();
  sensorData.pulseValue = analogRead(KG011_PIN);

  // Assign a unique sender ID to each ESP32 (manually set for each sender)
  sensorData.senderID = 1;  // Change this for each sender
  // Send sensor data to the receiver ESP32
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&sensorData, sizeof(sensorData));

  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }

  delay(1000);  // Send data every second
}

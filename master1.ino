#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// TFT pins
#define TFT_CS    5
#define TFT_RST   26
#define TFT_DC    27

// Create TFT object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Structure to hold received sensor data
typedef struct struct_message {
  float accelX;
  float accelY;
  float accelZ;
  float ambientTemp;
  float objectTemp;
  int pulseValue;
  uint8_t senderID; // Unique ID to identify the sender
} struct_message;

struct_message sensorData;

// Callback function that runs when data is received
void onReceive(const uint8_t *macAddr, const uint8_t *incomingData, int len) {
  memcpy(&sensorData, incomingData, sizeof(sensorData));

  // Display data on TFT screen
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);

  tft.print("Sender ID: "); tft.println(sensorData.senderID);
  tft.print("Accel X: "); tft.println(sensorData.accelX);
  tft.print("Accel Y: "); tft.println(sensorData.accelY);
  tft.print("Accel Z: "); tft.println(sensorData.accelZ);
  tft.print("Ambient Temp: "); tft.println(sensorData.ambientTemp);
  tft.print("Object Temp: "); tft.println(sensorData.objectTemp);
  tft.print("Pulse: "); tft.println(sensorData.pulseValue);
}

void setup() {
  Serial.begin(115200);

  // Initialize TFT
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback
  esp_now_register_recv_cb(onReceive);
}

void loop() {
  // The loop is empty because the onReceive callback handles data display
}

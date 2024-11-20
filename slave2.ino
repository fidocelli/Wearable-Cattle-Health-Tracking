#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_MLX90614.h>

// Sensor objects
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Define analog pin for KG011 pulse sensor
#define KG011_PIN 34  // Change this to your connected GPIO pin for the pulse sensor

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C
  Wire.begin();

  // Initialize ADXL345
  if (!accel.begin()) {
    Serial.println("ADXL345 not detected.");
    while (1);
  }
  Serial.println("ADXL345 initialized!");

  // Initialize MLX90614
  if (!mlx.begin()) {
    Serial.println("MLX90614 not detected.");
    while (1);
  }
  Serial.println("MLX90614 initialized!");

  // Initialize analog pin for pulse monitor (KG011)
  pinMode(KG011_PIN, INPUT);
}

void loop() {
  // Read ADXL345 (accelerometer data)
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.print("ADXL345 - X: ");
  Serial.print(event.acceleration.x);
  Serial.print(" m/s^2, Y: ");
  Serial.print(event.acceleration.y);
  Serial.print(" m/s^2, Z: ");
  Serial.print(event.acceleration.z);
  Serial.println(" m/s^2");

  // Read MLX90614 (temperature data)
  double ambientTemp = mlx.readAmbientTempC();
  double objectTemp = mlx.readObjectTempC();
  Serial.print("MLX90614 - Ambient Temp: ");
  Serial.print(ambientTemp);
  Serial.print(" C, Object Temp: ");
  Serial.print(objectTemp);
  Serial.println(" C");

  // Read KG011 (pulse monitor data)
  int pulseValue = analogRead(KG011_PIN);
  Serial.print("KG011 Pulse Value: ");
  Serial.println(pulseValue);

  // Delay between readings
  delay(1000);
}

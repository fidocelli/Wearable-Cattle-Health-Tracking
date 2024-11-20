#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>  // Or use Adafruit_ILI9341 for ILI9341 display
#include <MFRC522.h>

// TFT Screen pins
#define TFT_CS    5   // Chip select for TFT
#define TFT_RST   26  // Reset pin for TFT
#define TFT_DC    27  // Data/Command pin for TFT

// RFID Reader pins
#define RFID_CS   2   // Chip select for RFID
#define RFID_RST  4   // Reset pin for RFID

// Create TFT object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Create RFID object
MFRC522 rfid(RFID_CS, RFID_RST);

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);

  // Initialize TFT
  tft.initR(INITR_BLACKTAB); // Initialization for ST7735
  tft.setRotation(1);        // Adjust for landscape mode
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("RFID Reader");
  tft.setCursor(10, 30);
  tft.println("Scan a card...");

  // Initialize RFID
  SPI.begin();       // Initialize SPI bus
  rfid.PCD_Init();   // Initialize RFID reader

  // Clear RFID reader buffer
  delay(1000);
}

void loop() {
  // Look for new RFID cards
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // If a card is found, print its UID to Serial Monitor
    Serial.print("Card UID: ");
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Display the card UID on the TFT screen
    tft.fillRect(10, 60, 140, 20, ST7735_BLACK);  // Clear previous UID
    tft.setCursor(10, 60);
    tft.setTextColor(ST7735_GREEN);
    tft.print("Card UID: ");
    tft.println(uid);

    // Halt the card
    rfid.PICC_HaltA();
  }

  // Add a small delay to avoid overloading the loop
  delay(500);
}

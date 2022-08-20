#include <SPI.h>
#include <MFRC522.h>

// RFID Reader Pins
#define SS_PIN  5  // ESP32 pin GIOP5 
#define RST_PIN 27 // ESP32 pin GIOP27 

// RFID Reader variables
MFRC522 rfidReader(SS_PIN, RST_PIN);
uint8_t control = 0x00;
uint8_t rfidReadInterval = 100;

void setup() {
  Serial.begin(115200);

  // RFID Reader setup
  SPI.begin(); // init SPI bus
  rfidReader.PCD_Init(); // init RFID reader
  Serial.println("RFID Reader is ready...");
}

void loop() {
  if (!rfidReader.PICC_IsNewCardPresent()) {
    return;
  }
  if (!rfidReader.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("NewCard ");

  while (true) {
    control = 0;
    for (int i = 0; i < 3; i++) {
      if (!rfidReader.PICC_IsNewCardPresent()) {
        if (rfidReader.PICC_ReadCardSerial()) {
          //Serial.print('a');
          control |= 0x16;
        }
        if (rfidReader.PICC_ReadCardSerial()) {
          //Serial.print('b');
          control |= 0x16;
        }
        //Serial.print('c');
        control += 0x1;
      }
      //Serial.print('d');
      control += 0x4;
    }

    //Serial.println(control);
    if (control == 13 || control == 14) {
      //card is still there
    } else {
      break;
    }
  }

  Serial.println("Card Removed");

  delay(rfidReadInterval); // Interval at which to read cards (maybe we should turn it into variable)

  rfidReader.PICC_HaltA();
  rfidReader.PCD_StopCrypto1();
}

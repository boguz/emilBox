#include <SPI.h>
#include <MFRC522.h>
#include "DFRobotDFPlayerMini.h"

// RFID Reader Pins
#define RFID_CARD_SS_PIN  5  // ESP32 pin GIOP5 
#define RFID_CARD_RST_PIN 27 // ESP32 pin GIOP27

// RFID Reader variables
MFRC522 rfidReader(RFID_CARD_SS_PIN, RFID_CARD_RST_PIN);
uint8_t control = 0x00;
uint8_t rfidReadInterval = 100;

// Volume variables
int VOLUME = 15;
int VOLUME_MAX = 30;
int VOLUME_MIN = 0;
int VOLUME_CHANGE_AMOUNT = 1;

// Tag ids
String TAG_TEST =           "93 44 5C 92";
String TAG_BACH =           "9C CD 69 0F";
String TAG_MOZART =         "***********";
String TAG_BEETHOVEN =      "*******";
String TAG_TCHAIKOVSKI =    "*******";
String TAG_DEBUSSY =        "*******";
String TAG_SAINT_SAENS =    "*******";
String TAG_PROKOFIEV =      "*******";
String TAG_VIVALDI =        "*******";
String TAG_CHILDREN =       "*******";

// Folder ids
int FOLDER_BACH =          1;
int FOLDER_MOZART =        2;
int FOLDER_BEETHOVEN =     3;
int FOLDER_TCHAIKOVSKI =   4;
int FOLDER_DEBUSSY =       5;
int FOLDER_SAINT_SAENS =   6;
int FOLDER_PROKOFIEV =     7;
int FOLDER_VIVALDI =       8;
int FOLDER_CHILDREN =      9;

/******************************************************************************************
***     SETUP                                                                           ***
******************************************************************************************/
void setup() {
  // Start serial communication
  Serial.begin(115200);

  // RFID Reader setup
  SPI.begin(); // init SPI bus
  rfidReader.PCD_Init(); // init RFID reader
  Serial.println("RFID Reader is ready...");
}


/******************************************************************************************
***     GET TAG UID                                                                     ***
******************************************************************************************/
String getTagUid() {
  String content = "";
  byte letter;
  for (byte i = 0; i < rfidReader.uid.size; i++) {
    content.concat(String(rfidReader.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfidReader.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  String tag_uid = content.substring(1);
  return content.substring(1);
}


/******************************************************************************************
***     CHECK TAG VALIDITY AND START PLAY IF RELEVANT                                   ***
******************************************************************************************/
void checkTagValidity(String tag_uid) {
  if (tag_uid == TAG_TEST) {
    Serial.println("BLUE TAG");
  } else if (tag_uid == TAG_BACH) {
    Serial.println("BACH");
  } else {
    Serial.println("UNKNOWN CARD: ");
    Serial.print(tag_uid);
  }
}


/******************************************************************************************
***     LOOP                                                                            ***
******************************************************************************************/
void loop() {
  if (!rfidReader.PICC_IsNewCardPresent()) {
    return;
  }
  if (!rfidReader.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("NewCard ");

  // Get tag uid
  String tag_uid = getTagUid();
  Serial.print("TAG UID"); Serial.println(tag_uid);

  // Check if valid tag
  checkTagValidity(tag_uid);

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

  delay(rfidReadInterval); // Interval at which to read cards

  rfidReader.PICC_HaltA();
  rfidReader.PCD_StopCrypto1();
}

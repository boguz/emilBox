#include <SPI.h>
#include <MFRC522.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <U8g2lib.h>

// Pins for the RFID Tag reader
#define RFID_RC522_RST_PIN  9
#define RFID_RC522_SDA_PIN  10
#define GREEN_LED_PIN       7
// Pins for the SD Card reader
#define DFPLAYER_MINI_RX_PIN  4
#define DFPLAYER_MINI_TX_PIN  3
// Pins for buttons
#define BUTTON_VOL_DOWN_PIN 7
#define BUTTON_VOL_UP_PIN   8
#define BUTTON_PREV_TRACK   5
#define BUTTON_NEXT_TRACK   6

bool BUTTON_VOL_DOWN_PREV_STATE = HIGH;
bool BUTTON_VOL_UP_PREV_STATE = HIGH;
bool BUTTON_PREV_TRACK_STATE = HIGH;
bool BUTTON_NEXT_TRACK_STATE = HIGH;

// Create MFRC522 instance
MFRC522 mfrc522(RFID_RC522_SDA_PIN, RFID_RC522_RST_PIN);

// Initialize the sd card reader
SoftwareSerial mySoftwareSerial(DFPLAYER_MINI_TX_PIN, DFPLAYER_MINI_RX_PIN); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Oled display
U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI oled1(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);


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

MFRC522::Uid id;
uint8_t control = 0x00;

int VOLUME = 15;
int VOLUME_MAX = 30;
int VOLUME_MIN = 0;
int VOLUME_CHANGE_AMOUNT = 1;

/******************************************************************************************
***     SETUP                                                                           ***
******************************************************************************************/
void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_VOL_UP_PIN, INPUT_PULLUP);

  // Init SPI bus for the RFIS carr reader
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  //Use softwareSerial to communicate with mp3.
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }

  Serial.println(F("DFPlayer Mini online."));

  // DFPLayer settings
  myDFPlayer.volume(VOLUME);  // Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//   myDFPlayer.playFolder(1, 1);

  u8g2.begin();
}

//*****************************************************************************************//

void cpid(MFRC522::Uid *id) {
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}


/******************************************************************************************
***     GET TAG UID                                                                     ***
******************************************************************************************/
String getTagUid() {
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
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
    myDFPlayer.playFolder(1, 1);
  } else if (tag_uid == TAG_BACH) {
    Serial.println("BACH");
  } else {
    Serial.println("UNKNOWN CARD: ");
    Serial.print(tag_uid);
  }
}

/******************************************************************************************
***     VOLUME                                                                          ***
******************************************************************************************/
void volumeIncrease() {
  if (VOLUME < VOLUME_MAX) VOLUME += VOLUME_CHANGE_AMOUNT;
  myDFPlayer.volume(VOLUME);
}

void volumeDecrease() {
  if (VOLUME > VOLUME_MIN) VOLUME -= VOLUME_CHANGE_AMOUNT;
  myDFPlayer.volume(VOLUME);
}

/******************************************************************************************
***     HANDLE BUTTONS                                                                  ***
******************************************************************************************/

void handleButtons() {
  // Volume buttons
  bool volumeButtonDownState = digitalRead(BUTTON_VOL_DOWN_PIN);
  bool volumeButtonUpState = digitalRead(BUTTON_VOL_UP_PIN);

  if (volumeButtonDownState == LOW && BUTTON_VOL_DOWN_PREV_STATE == HIGH) {
    BUTTON_VOL_DOWN_PREV_STATE = LOW;
    volumeDecrease();
  } else if (volumeButtonDownState == HIGH && BUTTON_VOL_DOWN_PREV_STATE == LOW) {
    BUTTON_VOL_DOWN_PREV_STATE = HIGH;
  }

  if (volumeButtonUpState == LOW && BUTTON_VOL_UP_PREV_STATE == HIGH) {
    BUTTON_VOL_UP_PREV_STATE = LOW;
    volumeIncrease();
  } else if (volumeButtonUpState == HIGH && BUTTON_VOL_UP_PREV_STATE == LOW) {
    BUTTON_VOL_UP_PREV_STATE = HIGH;
  }

  // Prev track button
  bool prevTrackState = digitalRead(BUTTON_PREV_TRACK);

  if (prevTrackState == LOW && BUTTON_PREV_TRACK_STATE == HIGH) {
    BUTTON_PREV_TRACK_STATE = LOW;
    Serial.println("PREV TRACK");
    myDFPlayer.previous();
  } else if (prevTrackState == HIGH && BUTTON_PREV_TRACK_STATE == LOW) {
    BUTTON_PREV_TRACK_STATE = HIGH;
  }

  // Next track button
  bool nextTrackState = digitalRead(BUTTON_NEXT_TRACK);

  if (nextTrackState == LOW && BUTTON_NEXT_TRACK_STATE == HIGH) {
    BUTTON_NEXT_TRACK_STATE = LOW;
    myDFPlayer.next();
    Serial.println("NEXT TRACK");
  } else if (nextTrackState == HIGH && BUTTON_NEXT_TRACK_STATE == LOW) {
    BUTTON_NEXT_TRACK_STATE = HIGH;
  }
}

/******************************************************************************************
***     LOOP                                                                            ***
******************************************************************************************/
void loop() {
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_fub30_tn); // choose a suitable fon
  u8g2.drawStr(0,10,"Hello Lena!");  // write something to the internal memory
  u8g2.drawStr(0,20,"You my puki!!!!");  // write something to the internal memory

  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);

  Serial.println("XXXX");

  handleButtons();

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("NewCard ");

  // Get tag uid
  cpid(&id);
  String tag_uid = getTagUid();

  // Check if valid tag
  checkTagValidity(tag_uid);

  while (true) {
    handleButtons();
    digitalWrite(GREEN_LED_PIN, HIGH);
    control = 0;
    for (int i = 0; i < 3; i++) {
      if (!mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
          //Serial.print('a');
          control |= 0x16;
        }
        if (mfrc522.PICC_ReadCardSerial()) {
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

  Serial.println("CardRemoved");
  myDFPlayer.stop(); // Stop playing
  Serial.println("Stopped song");

//   digitalWrite(GREEN_LED_PIN, LOW);
  delay(100); // Interval at which to read cards (maybe we should turn it into variable)

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>

#include <SPI.h>
#include <MFRC522.h>
#include "DFRobotDFPlayerMini.h"

// Network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create WebSocket object
AsyncWebSocket ws("/ws");

String message = "";

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

// State variables
boolean isPlaying = false;
boolean isValidTag = false;
String artistPlaying = "";
String songPlaying = "";


/******************************************************************************************
***     INIT SPIFFS                                                                     ***
******************************************************************************************/
void initFS() {
  if(!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS!");  
  } else {
    Serial.println("SPIFFS mounted successfully");
  }
}


/******************************************************************************************
***     INIT WIFI                                                                       ***
******************************************************************************************/
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);  
  }
  Serial.println(WiFi.localIP());
}

/******************************************************************************************
***     NOTIFY CLIENTS                                                                  ***
******************************************************************************************/
void notifyClients(String sliderValues) {
  ws.textAll(sliderValues);
}


/******************************************************************************************
***     HANDLE WS MESSAGE                                                               ***
******************************************************************************************/
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    Serial.println("Handling WS message");
    Serial.println("message");
  }
}


/******************************************************************************************
***     HANDLE WS EVENTS                                                                ***
******************************************************************************************/
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
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
***     INIT WEBSOCKET                                                                  ***
******************************************************************************************/
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


/******************************************************************************************
***     CHECK TAG VALIDITY AND START PLAY IF RELEVANT                                   ***
******************************************************************************************/
void checkTagValidity(String tag_uid) {
  if (tag_uid == TAG_TEST) {
    Serial.println("BLUE TAG");
    isValidTag = true;
    artistPlaying = "Blue tag";
    songPlaying = "blue song";
    startPlayingSong();
  } else if (tag_uid == TAG_BACH) {
    Serial.println("BACH");
    artistPlaying = "Bach tag";
    songPlaying = "Bach song";
    startPlayingSong();
  } else {
    Serial.println("UNKNOWN CARD: ");
    Serial.print(tag_uid);
    isValidTag = false;
  }
}

/******************************************************************************************
***     UPDATE WEB UI                                                                   ***
******************************************************************************************/
void updateWebUI() {
  Serial.println("UPDATING WEB UI");
  Serial.println("Is playingg =====> "); Serial.print(isPlaying);
}


/******************************************************************************************
***     START PLAYING SONG                                                              ***
******************************************************************************************/
void startPlayingSong() {
  Serial.println("START PLAYING");
  Serial.println(artistPlaying);
  Serial.println(songPlaying);
  isPlaying = true;
  updateWebUI();
}


/******************************************************************************************
***     STOP PLAYING SONG                                                               ***
******************************************************************************************/
void stopPlayingSong() {
  Serial.println("STOP PLAYING");
  isPlaying = false;
  updateWebUI();
}


/******************************************************************************************
***     SETUP                                                                           ***
******************************************************************************************/
void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Init SPIFFS
  initFS();

  // Init WiFi
  initWiFi();

  // Init WebSocket
  initWebSocket();

  // Add routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "index.html", "text/html");  
  });

  // Server config
  server.serveStatic("/", SPIFFS, "/");

  // Init server
  server.begin();

  // RFID Reader setup
  SPI.begin(); // init SPI bus
  rfidReader.PCD_Init(); // init RFID reader
  Serial.println("RFID Reader is ready...");
}


/******************************************************************************************
***     LOOP                                                                            ***
******************************************************************************************/
void loop() {
  // Cleanup WS clients
  ws.cleanupClients();
  
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
  stopPlayingSong();

  delay(rfidReadInterval); // Interval at which to read cards

  rfidReader.PICC_HaltA();
  rfidReader.PCD_StopCrypto1();
}

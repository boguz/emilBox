#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"
#include "Audio.h"

// Import code partials
#include "_globalsAndVariables.h"
#include "_sdCard.h"
#include "_broadcastController.h"
#include "_volumeControllers.h"
#include "_webIndex.h"
#include "_wsHandlers.h"
#include "_wsEvents.h"
#include "_wsRequests.h"
#include "_buttonHandlers.h"
#include "_rfidTagReader.h"
#include "_audioPlayer.h"

/******************************************************************************************
******      SETUP                                                                    ******
******************************************************************************************/
void setup() {
  // Init buttons
  pinMode(BUTTON_VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_VOL_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_STOP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP);

  // Start Serial port
  Serial.begin(115200);

  // Init SPI bus (for the tag reader)
  SPI.begin();

  // Init the tag reader
  mfrc522.PCD_Init();

  setupSDCardReader();

  // Start access point
  WiFi.softAP(ssid, password);

  // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // 404 page
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  setupAudioPlayer();

  Serial.println("AFTER SETUP ========================");
  Serial.println(audioPlayer.isRunning());
  Serial.println(audioPlayer.getVolume());
  Serial.println(audioPlayer.getBitsPerSample());
  Serial.println(audioPlayer.getFileSize());

  audioPlayer.connecttoSD("/amaze.mp3", 0);

  Serial.println("AFTER CONNECT ========================");
  Serial.println(audioPlayer.isRunning());
  Serial.println(audioPlayer.getVolume());
  Serial.println(audioPlayer.getBitsPerSample());
  Serial.println(audioPlayer.getFileSize());
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}

/******************************************************************************************
******      LOOP                                                                     ******
******************************************************************************************/
void loop() {
  // Check for button clicks
  handleButtons();

  // Look for and handle WebSocket data
  webSocket.loop();

  // Get correct state for the rfid reader, detect new tag and tag removal
  rfidTagReaderLoop();

  audioPlayer.loop();
}
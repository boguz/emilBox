#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"

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
}

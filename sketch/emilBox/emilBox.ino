/*
   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/
*/

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "web_index.h"

// Constants
const char *ssid = "EmilBox";
const char *password =  "e1000e1000";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int led_pin = 15;

// Globals
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
char msg_buf[10];

// Volume Variables
int VOLUME = 15;
int VOLUME_NORMAL_MAX = 30;
int VOLUME_LIMIT_MAX = 15;
int VOLUME_MAX = VOLUME_NORMAL_MAX;
int VOLUME_MIN = 0;
int VOLUME_CHANGE_AMOUNT = 1;
bool VOLUME_IS_LIMITED = false;

// Player variables
bool IS_PLAYING = false;
char TRACK_NAME[] = "Air on the G String";
char ARTIST_NAME[] = "Bach";

// Button variables
const int BUTTON_VOL_DOWN_PIN = 34;
bool BUTTON_VOL_DOWN_STATE = HIGH;
bool BUTTON_VOL_DOWN_PREV_STATE = HIGH;

const int BUTTON_VOL_UP_PIN = 35;
bool BUTTON_VOL_UP_STATE = HIGH;
bool BUTTON_VOL_UP_PREV_STATE = HIGH;

const int BUTTON_STOP_PIN = 32;
bool BUTTON_STOP_STATE = HIGH;
bool BUTTON_STOP_PREV_STATE = HIGH;

const int BUTTON_NEXT_PIN = 33;
bool BUTTON_NEXT_STATE = HIGH;
bool BUTTON_NEXT_PREV_STATE = HIGH;


/***********************************************************
   Functions
*/

void volumeDecrease() {
  if (VOLUME > VOLUME_MIN) {
    VOLUME = VOLUME - VOLUME_CHANGE_AMOUNT;
    broadcastUpdate();
  }
}

void volumeIncrease() {
  if (VOLUME < VOLUME_MAX) {
    VOLUME = VOLUME + VOLUME_CHANGE_AMOUNT;
    broadcastUpdate();
  } else {
    VOLUME = VOLUME_MAX;
    broadcastUpdate();
  }
}

void updateVolumeLimitState(bool state) {
  VOLUME_IS_LIMITED = state;
  broadcastUpdate();
}

void broadcastUpdate() {
  DynamicJsonDocument doc(1024);

  doc["volume"] = VOLUME;
  doc["volume_min"] = VOLUME_MIN;
  doc["volume_max"] = VOLUME_MAX;
  doc["volume_is_limited"] = VOLUME_IS_LIMITED;
  doc["is_playing"] = IS_PLAYING;
  doc["track_name"] = TRACK_NAME;
  doc["artist_name"] = ARTIST_NAME;

  char json_string[1024];
  serializeJson(doc, json_string);
  webSocket.broadcastTXT(json_string);
}

void handleWsTextMessage(uint8_t client_num, uint8_t * payload) {
  if ( strcmp((char *)payload, "getValues") == 0 ) {
    broadcastUpdate();
  } else if ( strcmp((char *)payload, "volume_down_button_click") == 0 ) {
    volumeDecrease();
  } else if ( strcmp((char *)payload, "volume_up_button_click") == 0 ) {
    volumeIncrease();
  } else if ( strcmp((char *)payload, "volume_limit_checkbox_on") == 0 ) {
    updateVolumeLimitState(true);
  } else if ( strcmp((char *)payload, "volume_limit_checkbox_off") == 0 ) {
    updateVolumeLimitState(false);
  } else { // Message not recognized
    Serial.println("[%u] Message not recognized");
  }
}



// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {

  // Figure out the type of WebSocket event
  switch (type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:

      // Print out raw message
      Serial.printf("[%u] Received text: %s\n", client_num, payload);

      handleWsTextMessage(client_num, payload);

      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request) {
  const char* dataType = "text/html";
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
//   request->send(SPIFFS, "/index.html", "text/html");
  AsyncWebServerResponse *response = request->beginResponse_P(200, dataType, index_html_gz, index_html_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

/***********************************************************
   Main
*/

void handleButtons() {
  // VOLUME DOWN BUTTON
  bool buttonVolDownState = digitalRead(BUTTON_VOL_DOWN_PIN);
  if (buttonVolDownState == LOW && BUTTON_VOL_DOWN_PREV_STATE == HIGH) {
    Serial.println("button down pressed");
    volumeDecrease();
    BUTTON_VOL_DOWN_PREV_STATE = LOW;
  } else if (buttonVolDownState == HIGH && BUTTON_VOL_DOWN_PREV_STATE == LOW) {
    BUTTON_VOL_DOWN_PREV_STATE = HIGH;
  }

  // VOLUME UP BUTTON
  bool buttonVolUpState = digitalRead(BUTTON_VOL_UP_PIN);
  if (buttonVolUpState == LOW && BUTTON_VOL_UP_PREV_STATE == HIGH) {
    Serial.println("button up pressed");
    volumeIncrease();
    BUTTON_VOL_UP_PREV_STATE = LOW;
  } else if (buttonVolUpState == HIGH && BUTTON_VOL_UP_PREV_STATE == LOW) {
    BUTTON_VOL_UP_PREV_STATE = HIGH;
  }

  // STOP BUTTON
  bool buttonStopState = digitalRead(BUTTON_STOP_PIN);
  if (buttonStopState == LOW && BUTTON_STOP_PREV_STATE == HIGH) {
    Serial.println("button stop pressed");
    volumeIncrease();
    BUTTON_STOP_PREV_STATE = LOW;
  } else if (buttonStopState == HIGH && BUTTON_STOP_PREV_STATE == LOW) {
    BUTTON_STOP_PREV_STATE = HIGH;
  }

  // NEXT BUTTON
  bool buttonNextState = digitalRead(BUTTON_NEXT_PIN);
  if (buttonNextState == LOW && BUTTON_NEXT_PREV_STATE == HIGH) {
    Serial.println("button next pressed");
    volumeIncrease();
    BUTTON_NEXT_PREV_STATE = LOW;
  } else if (buttonNextState == HIGH && BUTTON_NEXT_PREV_STATE == LOW) {
    BUTTON_NEXT_PREV_STATE = HIGH;
  }
}

void setup() {
  // Init buttons
  pinMode(BUTTON_VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_VOL_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_STOP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP);

  // Start Serial port
  Serial.begin(115200);

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

void loop() {
  // Check for button clicks
  handleButtons();

  // Look for and handle WebSocket data
  webSocket.loop();
}

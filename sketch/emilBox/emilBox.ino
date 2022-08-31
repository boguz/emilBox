/*
   https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/
*/

#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

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
int led_state = 0;

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
const int BUTTON_VOL_UP_PIN = 35;
bool BUTTON_VOL_DOWN_STATE = HIGH;
bool BUTTON_VOL_DOWN_PREV_STATE = HIGH;
bool BUTTON_VOL_UP_STATE = HIGH;
bool BUTTON_VOL_UP_PREV_STATE = HIGH;


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
    broadcastUpdate();
  } else if ( strcmp((char *)payload, "volume_limit_checkbox_off") == 0 ) {
    updateVolumeLimitState(false);
    broadcastUpdate();
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
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
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

void setup() {
  // Init buttons
  pinMode(BUTTON_VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_VOL_UP_PIN, INPUT_PULLUP);

  // Start Serial port
  Serial.begin(115200);

  // Make sure we can read the file system
  if ( !SPIFFS.begin()) {
    Serial.println("Error mounting SPIFFS");
    while (1);
  }

  // Start access point
  WiFi.softAP(ssid, password);

  // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // Serve static files
  server.serveStatic("/", SPIFFS, "/");

  // 404 page
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {
  // Check buttons
  bool buttonDownState = digitalRead(BUTTON_VOL_DOWN_PIN);
  if (buttonDownState == LOW && BUTTON_VOL_DOWN_PREV_STATE == HIGH) {
    Serial.println("button down pressed");
    volumeDecrease();
    BUTTON_VOL_DOWN_PREV_STATE = LOW;
  } else if (buttonDownState == HIGH && BUTTON_VOL_DOWN_PREV_STATE == LOW) {
    BUTTON_VOL_DOWN_PREV_STATE = HIGH;
  }

  bool buttonUpState = digitalRead(BUTTON_VOL_UP_PIN);
  if (buttonUpState == LOW && BUTTON_VOL_UP_PREV_STATE == HIGH) {
    Serial.println("button up pressed");
    volumeIncrease();
    BUTTON_VOL_UP_PREV_STATE = LOW;
  } else if (buttonUpState == HIGH && BUTTON_VOL_UP_PREV_STATE == LOW) {
    BUTTON_VOL_UP_PREV_STATE = HIGH;
  }


  // Look for and handle WebSocket data
  webSocket.loop();
}

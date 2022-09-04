/*
* Define and set all constants and variables
*/

// Constants
const char *ssid = "EmilBox";
const char *password =  "e1000e1000";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;

// Globals
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
char msg_buf[10];

// Tag reader variables
#define RFID_RC522_RST_PIN  27
#define RFID_RC522_SDA_PIN  2

MFRC522 mfrc522(RFID_RC522_SDA_PIN, RFID_RC522_RST_PIN);

bool rfid_tag_present_prev = false;
bool rfid_tag_present = false;
int _rfid_error_counter = 0;
bool _tag_found = false;

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
String TRACK_NAME = "-";
String ARTIST_NAME = "-";

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

// Tag IDs
String TAG_TEST = "93 44 5C 92";
String TAG_BACH = "9C CD 69 0F";

// SD Card variables
uint8_t cardType;
/*
* Handle the different webSockets text messages
*/
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
/*
* Broadcast the current data to all websocket clients
*/
void broadcastUpdate() {
  DynamicJsonDocument doc(1024);

  // Get current values
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
/*
* Get the Uid for the current tag
*/
String getTagUid() {
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  String tag_uid = content.substring(1);
  Serial.println("Getting tag uid");
  Serial.println(content.substring(1));
  return content.substring(1);
}

/*
* Check if a tag uid is valid and set corresponding variable values
*/
void checkTagValidity(String tag_uid) {
  if (tag_uid == TAG_TEST) {
    Serial.println("BLUE TAG");
    ARTIST_NAME = "Blue Tag";
    TRACK_NAME = "Super Track name";
    IS_PLAYING = true;
    readFile(SD, "/hello.txt");
    broadcastUpdate();
  } else if (tag_uid == TAG_BACH) {
    Serial.println("BACH");
  } else {
    Serial.println("UNKNOWN CARD: ");
    Serial.print(tag_uid);
  }
}

/*
* Test if there is a new tag or if a tag has been removed
*/
void rfidTagReaderLoop() {
  rfid_tag_present_prev = rfid_tag_present;

  _rfid_error_counter += 1;
  if (_rfid_error_counter > 2) {
    _tag_found = false;
  }

  // Detect Tag without looking for collisions
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  // Reset baud rates
  mfrc522.PCD_WriteRegister(mfrc522.TxModeReg, 0x00);
  mfrc522.PCD_WriteRegister(mfrc522.RxModeReg, 0x00);
  // Reset ModWidthReg
  mfrc522.PCD_WriteRegister(mfrc522.ModWidthReg, 0x26);

  MFRC522::StatusCode result = mfrc522.PICC_RequestA(bufferATQA, &bufferSize);

  if (result == mfrc522.STATUS_OK) {
    if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
      return;
    }
    _rfid_error_counter = 0;
    _tag_found = true;
  }

  rfid_tag_present = _tag_found;

  // rising edge
  if (rfid_tag_present && !rfid_tag_present_prev) {
    Serial.println("Tag found");
    // Get tag uid
    String tag_uid = getTagUid();

    // Check if valid tag
    checkTagValidity(tag_uid);
  }

  // falling edge
  if (!rfid_tag_present && rfid_tag_present_prev) {
    Serial.println("Tag gone");
    ARTIST_NAME = "-";
    TRACK_NAME = "-";
    IS_PLAYING = false;
    broadcastUpdate();
  }
}
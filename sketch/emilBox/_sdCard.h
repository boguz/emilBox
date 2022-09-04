/*
* Setup the SD card reader
*/
void setupSDCardReader() {
  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    return;
  }

  cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  } else {
    Serial.println("SD Card mounted successfully");
  }
}

/*
* Read a file from the sd card
*/
void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}
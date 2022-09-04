/*
* handle button clicks
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
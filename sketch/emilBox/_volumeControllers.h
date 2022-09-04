/*
* Decrease the volume
*/
void volumeDecrease() {
  if (VOLUME > VOLUME_MIN) {
    VOLUME = VOLUME - VOLUME_CHANGE_AMOUNT;
    broadcastUpdate();
  }
}

/*
* Increase the volume
*/
void volumeIncrease() {
  if (VOLUME < VOLUME_MAX) {
    VOLUME = VOLUME + VOLUME_CHANGE_AMOUNT;
    broadcastUpdate();
  } else {
    VOLUME = VOLUME_MAX;
    broadcastUpdate();
  }
}

/*
* Set the state on the volume_is_limited variable
*/
void updateVolumeLimitState(bool state) {
  VOLUME_IS_LIMITED = state;
  broadcastUpdate();
}
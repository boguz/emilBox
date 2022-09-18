void setupAudioPlayer() {
  Serial.println("Setting up audio player...");
  audioPlayer.setPinout(AUDIOPLAYER_BCLK, AUDIOPLAYER_LRC, AUDIOPLAYER_DOUT);
  audioPlayer.i2s_mclk_pin_select(AUDIOPLAYER_BCLK);
  audioPlayer.setVolume(100);
}

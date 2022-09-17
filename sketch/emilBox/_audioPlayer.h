void setupAudioPlayer() {
  Serial.println("Setting up audio player...");
  audioPlayer.setPinout(AUDIOPLAYER_BCLK, AUDIOPLAYER_LRC, AUDIOPLAYER_DOUT);
  audioPlayer.setVolume(15);
  audioPlayer.connecttoFS(SD, "/amaze.mp");
}

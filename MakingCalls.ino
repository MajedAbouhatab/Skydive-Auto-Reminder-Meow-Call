#include <SDHCI.h>
#include <Audio.h>
SDClass ThisSD;
AudioClass *ThisAudio;
File ThisFile;

void setup()
{
  // Using serial to receive data
  Serial.begin(9600);
  // Initializing audio
  ThisAudio = AudioClass::getInstance();
  ThisAudio->begin();
  ThisAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  ThisAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT);
  ThisAudio->setVolume(0);
  ThisAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3, "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);
  // Sound check
  for (int i = 1000; i <= 3000; i += 500)
  {
    ThisAudio->setBeep(1, -20, i);
    delay(50);
    ThisAudio->setBeep(0, 0, 0);
    delay(50);
  }
}

void loop()
{
  // Wait for text
  if (Serial.available())
  {
    // Store the whole string
    String temp = Serial.readString();
    while (temp.length() > 0)
    {
      // Open file from first piece of the string
      ThisFile = ThisSD.open(temp.substring(0, temp.indexOf("|")) + ".mp3");
      // If good file name
      if (ThisFile == 1)
      {
        PlaySound();
      }
      // Default file name for carrier type
      else if (isAlpha(temp.charAt(0)))
      {
        ThisFile = ThisSD.open("Load.mp3");
        PlaySound();
      }
      // Close file
      ThisFile.close();
      // Split the string and take the first piece out
      if (temp.indexOf("|") > 0) temp = temp.substring(temp.indexOf("|") + 1, temp.length());
      // No more pieces - do this to exit while loop
      else temp = "";
    }
  }
}

void PlaySound()
{
  // Prime sound player
  ThisAudio->writeFrames(AudioClass::Player0, ThisFile);
  // Start sound player
  ThisAudio->startPlayer(AudioClass::Player0);
  // Getting the next chunk(s) of sound until the end
  while (!ThisAudio->writeFrames(AudioClass::Player0, ThisFile));
  // Stop audio player
  ThisAudio->stopPlayer(AudioClass::Player0);
}

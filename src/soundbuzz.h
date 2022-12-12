#ifndef SoundBuzz_h
#define SoundBuzz_h
#include "pitches.h"

#define NOTE_LOW 440
#define NOTE_HIGH 880

class SoundBuzz {
public:
  SoundBuzz(byte buzzerPin);

  void PlayStart();
  void PlaySecond();
  void PlayFinish();
  void PlayInit();
  void PlayCancel();

  void PlayNote(int note);

  void Play(int *melody, byte *tempo, byte len);
private:
  byte buzzerPin;
};

#endif
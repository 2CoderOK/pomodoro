#include "soundbuzz.h"

SoundBuzz::SoundBuzz(byte buzzerPin) {
  this->buzzerPin = buzzerPin;
}

void SoundBuzz::PlayStart() {
  int melody[] = {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0, 0,
    NOTE_G6, 0, 0, 0,    
  };

  byte tempo[] = {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,   
  };

  Play(melody, tempo, (sizeof(melody) / sizeof(int)));
}

void SoundBuzz::PlaySecond() {  
  int melody_line_1[] = { NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_AS3, NOTE_AS4, 0, 0 };
  int melody_line_2[] = { NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4, NOTE_DS3, NOTE_DS4, 0, 0 };
  byte tempo[] = { 12, 12, 12, 12, 12, 12, 6, 3 };
  Play(melody_line_1, tempo, (sizeof(melody_line_1) / sizeof(int)));
  Play(melody_line_1, tempo, (sizeof(melody_line_1) / sizeof(int)));
  Play(melody_line_2, tempo, (sizeof(melody_line_1) / sizeof(int)));
  Play(melody_line_2, tempo, (sizeof(melody_line_1) / sizeof(int)));
}

void SoundBuzz::PlayFinish() {
  int melody[] = {
    0, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_E6,
    NOTE_G6, NOTE_E6, 0,
  };

  byte tempo[] = {
    12, 12, 12, 12, 12, 12,
    4, 6, 6,
  };

  Play(melody, tempo, (sizeof(melody) / sizeof(int)));
}

void SoundBuzz::PlayCancel() {
  for (byte i = 0; i < 3; i++) {
    Play(NOTE_C4, 12, 1);
    Play(NOTE_B3, 12, 1);
  }
  Play(0, 12, 1);
}

void SoundBuzz::PlayInit() {
  int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4, 0 };

  byte tempo[] = { 5, 5, 3, 4 };

  Play(melody, tempo, (sizeof(melody) / sizeof(int)));
}

void SoundBuzz::Play(int *melody, byte *tempo, byte len) {
  for (uint8_t thisNote = 0; thisNote < len; thisNote++) {
    int noteDuration = 1000 / uint8_t(tempo[thisNote]);  //convert duration to time delay
    tone(buzzerPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 2;  //Here 1.05 is tempo, increase to play it slower
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

void SoundBuzz::PlayNote(int note) {
  tone(buzzerPin, note, 100);
  delay(100);
  noTone(buzzerPin);
}
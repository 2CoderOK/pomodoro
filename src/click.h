#ifndef Click_h
#define Click_h
#include "pitches.h"

#define CLICK_SHORT 40000
#define CLICK_LONG CLICK_SHORT * 4

class Click {
public:
  Click(byte cPin);
  unsigned char GetClick();
  unsigned char GetDisplay();
  unsigned long GetDuration();

private:
  byte clickPin;
  unsigned long clickDuration;
  bool clickLastState;
};

#endif
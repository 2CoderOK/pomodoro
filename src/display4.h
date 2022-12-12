#ifndef Display4_h
#define Display4_h

#include "TM1637.h"


class Display4 {
public:
  Display4(int8_t clkPin, int8_t dioPin);

  void Init();
  void DisplayString(char str[]);
  void DisplayNum(int num);
  void DisplayClock(int8_t timeDisp[]);
  void ClearDisplay();
  void DisplayDot(bool show);
  void DotNext();

private:
  bool lastPointState = false;
  TM1637 tm1637;
};

#endif
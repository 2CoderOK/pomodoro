#include "display4.h"

Display4::Display4(int8_t clkPin, int8_t dioPin)
  : tm1637(clkPin, dioPin) {
}

void Display4::Init() {
  tm1637.set();
  tm1637.init();
}

void Display4::DisplayString(char str[]) {
  tm1637.point(POINT_OFF);
  tm1637.displayStr(str);
  tm1637.point(lastPointState);
}

void Display4::DisplayNum(int num) {
  tm1637.point(POINT_OFF);
  tm1637.displayNum(num);
  tm1637.point(lastPointState);
}

void Display4::DisplayClock(int8_t timeDisp[]) {
  tm1637.display(timeDisp);
}

void Display4::ClearDisplay() {
  tm1637.clearDisplay();
}

void Display4::DisplayDot(bool show) {
  if (show) {
    tm1637.point(POINT_ON);
    lastPointState = true;
  } else {
    tm1637.point(POINT_OFF);
    lastPointState = false;
  }
}

void Display4::DotNext() {
  DisplayDot(!lastPointState);
}
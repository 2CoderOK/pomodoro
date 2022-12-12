#include "click.h"

Click::Click(byte cPin) {
  clickPin = cPin;
  clickDuration = 0;
  clickLastState = false;
}

unsigned char Click::GetClick() {
  int8_t action = 0;

  if (digitalRead(this->clickPin) == HIGH) {
    clickLastState = true;
    clickDuration++;
  } else {
    if (clickLastState) {
      if (clickDuration > CLICK_LONG) {
        Serial.println("CLICK CLICK_LONG");
        action = 2;
      } else if (clickDuration > CLICK_SHORT) {
        Serial.println("CLICK CLICK_SHORT");
        action = 1;
      }
    } else
      action = 0;

    clickLastState = false;
    clickDuration = 0;
  }
  return action;
}

unsigned long Click::GetDuration() {
  return clickDuration;
}

unsigned char Click::GetDisplay() {
  unsigned char clickStatus = 0;
  if (clickDuration)
    if (clickDuration >= CLICK_SHORT) {
      clickStatus = 2;
      for (int8_t i = 2; i <= 4; i++)
        if (clickDuration >= CLICK_SHORT * i)
          clickStatus = i + 1;
        else
          break;
    } else
      clickStatus = 1;

  return clickStatus;
}
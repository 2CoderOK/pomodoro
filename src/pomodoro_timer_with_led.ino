#include <TimerOne.h>
#include <Wire.h>
#include <RtcDS1307.h>

#include "state_machine.h"
#include "rtsl.h"

RtcDS1307<TwoWire> Rtc(Wire);
StateMachine logic = StateMachine(&Rtc);

void setup() {
  Serial.begin(57600);
  Serial.println("Setup");

  pinMode(BTN_BUZZER, OUTPUT);
  pinMode(BTN_CLICK, INPUT);

  logic.Init();

  Timer1.initialize(500000);
  Timer1.attachInterrupt(TimingISR);
}

void loop() {
  logic.Update();
}

void TimingISR() {
  logic.Tick();
}
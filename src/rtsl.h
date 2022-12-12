#ifndef RTSL_h
#define RTSL_h

#define countof(a) (sizeof(a) / sizeof(a[0]))
#include <Wire.h>
#include <RtcDS1307.h>

#define MEMORY_ADDRESS 13

class RTSL {
public:
  RTSL(RtcDS1307<TwoWire>* rtc);
  void Init();
  void GetData(unsigned int* data);
  void SaveData(unsigned int* data);
  void PrintDateTime(const RtcDateTime& dt);
  void HardResetData();
  RtcDateTime GetTime();

private:
  RtcDS1307<TwoWire>* rtc;
};

#endif
#include "rtsl.h"

RTSL::RTSL(RtcDS1307<TwoWire>* rtsl) {
  rtc = rtsl;
}

void RTSL::HardResetData() {
  Serial.println("HardResetData");
  RtcDateTime now = RtcDateTime(__DATE__, __TIME__);
  unsigned int empty[] = { now.Day(), 0, 0, 0, 0 };
  SaveData(empty);
}

void RTSL::SaveData(unsigned int* num2) {
  unsigned int num[5] = { num2[0], num2[1], num2[2], num2[3], num2[4] };
  Serial.println("SaveData");
  for (int8_t i = 0; i < 5; i++)
    Serial.println(num[i]);

  rtc->SetMemory(0, MEMORY_ADDRESS);
  uint8_t written = rtc->SetMemory(MEMORY_ADDRESS, (const uint8_t*)num, 10);
  rtc->SetMemory(1, written);
  Serial.println("Written");
  Serial.println(written);
}

void RTSL::GetData(unsigned int* num) {
  uint8_t address = rtc->GetMemory(0);
  if (address != MEMORY_ADDRESS) {
    Serial.println("address didn't match");
    Serial.println(address);
    HardResetData();
  } else {
    Serial.println("address match");
    uint8_t count = rtc->GetMemory(1);
    Serial.println("data size");
    Serial.println(count);
    uint8_t buff[10];

    uint8_t gotten = rtc->GetMemory(address, buff, count);
    if (gotten != count) {
      Serial.println("data size read didn't match");
      Serial.println(gotten);
    } else {
      Serial.println("data size read match");
      byte y = 0;
      for (int8_t i = 0; i < 5; i++) {
        num[i] = buff[y + 1];
        num[i] = num[i] << 8;
        num[i] |= buff[y];
        y = y + 2;
      }
      for (int8_t i = 0; i < 5; i++)
        Serial.println(num[i]);
    }
  }
  return num;
}

void RTSL::Init() {
  rtc->Begin();
  Serial.print("rtc time: ");
  PrintDateTime(Rtc.GetDateTime());

  Serial.print("compiled: ");
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  PrintDateTime(compiled);
  Serial.println();

  if (!rtc->IsDateTimeValid()) {
    if (rtc->LastError() != 0) {
      Serial.print("RTC communications error = ");
      Serial.println(rtc->LastError());
    } else {
      Serial.println("RTC lost confidence in the DateTime!");
      rtc->SetDateTime(compiled);
    }
  }

  if (!rtc->GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    rtc->SetIsRunning(true);
  }

  if (rtc->GetDateTime() < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    rtc->SetDateTime(compiled);
  }

  rtc->SetSquareWavePin(DS1307SquareWaveOut_Low);
}

void RTSL::PrintDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}

RtcDateTime RTSL::GetTime() {
  return rtc->GetDateTime();
}
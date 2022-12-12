#ifndef StateMachine_h
#define StateMachine_h
#include "soundbuzz.h"
#include "click.h"
#include "display4.h"
#include "rtsl.h"

#define RTC_CLK 3  // pins definitions for TM1637 and can be changed to other ports
#define RTC_DIO 4

#define BTN_CLICK 6   // click button to arduino pin 6
#define BTN_BUZZER 9  // buzzer to arduino pin 9

#define STATUS_IDLE 0
#define STATUS_PREPARE 1
#define STATUS_RUN_WORK 2
#define STATUS_RUN_PAUSE 3
#define STATUS_RUN_REST 4

#define DEFAULT_TIME 25
#define INCREASE_TIME 5
#define MAX_TIME 60

class StateMachine {
public:
  StateMachine(RtcDS1307<TwoWire> *rtsl);
  void Init();
  void Update();

  void Tick();

private:
  void Idle();
  void IncreaseTime();
  void Prepare();
  void Start();
  void Rest();
  void Pause();
  void Cancel();

  void HandleClick();
  void MinusSecond();
  void UpdateDisplay();
  void UpdatePomodoro(bool success = true);
  void UpdateStatus(int8_t newStatus);
  byte GetStatus();

  bool DisplayClick();
  void ClearDisplay();

  void LoadUserData();
  void PrintUserData();
  void PrintStats();

  byte CalcWeekday(int, int, int);

  SoundBuzz sound;
  Click button;
  Display4 disp;
  RTSL rtc = NULL;

  int8_t timeDisp[4];
  unsigned int userData[5];  // 0 - current_day_number, stats_for_today, stats_this_week (of a given month), stats_this_month, stats_total

  byte cur_status = STATUS_IDLE;
  byte prev_status = STATUS_IDLE;

  bool redraw = false;
  bool seconds_event = false;

  int8_t second = 59;
  byte idle_time = 0;
  int8_t pause_second = 0;
  int8_t pause_minute = 0;
  byte halfsecond = 0;

  int8_t lastClickDisplay = 0;

  int8_t pomodoro_minute = DEFAULT_TIME;
  int8_t pomodoro_minute_last = DEFAULT_TIME;
  int8_t pomodoro_completed = 0;  // track number of completed pomodors, after 3 pomodoros give rest 25 min
};

#endif
#include "state_machine.h"

StateMachine::StateMachine(RtcDS1307<TwoWire> *rtsl)
  : sound(BTN_BUZZER), button(BTN_CLICK), disp(RTC_CLK, RTC_DIO) {
  rtc = RTSL(rtsl);
  for (uint8_t i = 0; i < 5; i++)
    userData[i] = 0;
}

void StateMachine::Init() {
  bool hardwareReset = digitalRead(BTN_CLICK) == HIGH ? true : false;
  rtc.Init();
  LoadUserData();
  disp.Init();
  sound.PlayInit();
  disp.DisplayString("HELL0");
  if (hardwareReset) {
    rtc.HardResetData();
  }
  Idle();
}

byte StateMachine::CalcWeekday(int year, int month, int day)
/* Calculate day of week in proleptic Gregorian calendar. Sunday == 0. */
{
  int adjustment, mm, yy;
  if (year < 2000) year += 2000;
  adjustment = (14 - month) / 12;
  mm = month + 12 * adjustment - 2;
  yy = year - adjustment;
  return (day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
}

void StateMachine::PrintStats() {
  Serial.println("Display stats");
  disp.DisplayString("today _");
  disp.DisplayNum(userData[1]);
  delay(1500);
  disp.DisplayString("week _");
  disp.DisplayNum(userData[2] + userData[1]);
  delay(1500);
  disp.DisplayString("month _");
  disp.DisplayNum(userData[3] + userData[2] + userData[1]);
  delay(1500);
  disp.DisplayString("all _");
  disp.DisplayNum(userData[4] + userData[3] + userData[2] + userData[1]);
  delay(1500);
}

void StateMachine::PrintUserData() {
  Serial.print("Pomodoro today: ");
  Serial.print(userData[1]);
  Serial.println();
  Serial.print("Pomodoro this week: ");
  Serial.print(userData[2]);
  Serial.println();
  Serial.print("Pomodoro this month: ");
  Serial.print(userData[3]);
  Serial.println();
  Serial.print("Pomodoro total: ");
  Serial.print(userData[4]);
  Serial.println();
}

void StateMachine::LoadUserData() {
  Serial.println("LoadUserData");
  rtc.GetData(userData);
  PrintUserData();
  RtcDateTime now = rtc.GetTime();
  if (userData[0] == now.Day()) {
    Serial.println("Same day, ok");
    pomodoro_completed = userData[1];  // load todays pomodoros
  } else {
    Serial.println("Different day, not ok");
    bool rollover_month = false;
    bool rollover_week = false;
    bool rollover_day = false;

    if (now.Day() > userData[0]) {
      Serial.println("Same month, next day");
      byte prev_day_of_week = CalcWeekday(now.Year(), now.Month(), userData[0]);
      if (now.DayOfWeek() <= prev_day_of_week || now.Day() - userData[0] > 7) {
        Serial.println("next week");
        rollover_week = true;
      } else {
        Serial.println("same week");
      }
      rollover_day = true;
    } else {
      Serial.println("Next month, rollover today, week, month");
      rollover_month = rollover_week = rollover_day = true;
    }
    userData[0] = now.Day();  // set today
    if (rollover_day) {
      Serial.println("rollover today");
      userData[2] = userData[2] + userData[1];  // copy today`s pomodoro (old) to this week
      userData[1] = 0;                          // reset today`s pomodoro
    }
    if (rollover_week) {
      Serial.println("rollover this week");
      userData[3] = userData[3] + userData[2];  // copy this week`s pomodoro to this month
      userData[2] = 0;                          // reset this week`s pomodoro
    }
    if (rollover_month) {
      Serial.println("rollover this month");
      userData[4] = userData[4] + userData[3];  // copy this month`s pomodoro to all time total
      userData[3] = 0;                          // reset this month`s pomodoro
    }
    rtc.SaveData(userData);
    PrintUserData();
  }
}

void StateMachine::UpdateStatus(int8_t newStatus) {
  Serial.println("UpdateStatus");
  Serial.println(newStatus);
  prev_status = cur_status;
  cur_status = newStatus;
}

byte StateMachine::GetStatus() {
  return cur_status;
}

void StateMachine::Tick() {
  if (!seconds_event) {
    halfsecond++;
    if (halfsecond == 2) {
      halfsecond = 0;
      if (lastClickDisplay == 0) {
        second--;
        if (second < 0) {
          seconds_event = true;
          second = 59;
        }
      }
    }
    redraw = true;
  }
}

void StateMachine::IncreaseTime() {
  Serial.println("IncreaseTime");

  pomodoro_minute = pomodoro_minute + INCREASE_TIME;

  if (pomodoro_minute > MAX_TIME)
    pomodoro_minute = DEFAULT_TIME;
  pomodoro_minute_last = pomodoro_minute;
}

void StateMachine::Prepare() {
  Serial.println("Prepare");
  second = 10;
  UpdateStatus(STATUS_PREPARE);
}

void StateMachine::Start() {
  Serial.println("Start");
  pomodoro_minute--;
  second = 59;
  pause_minute = 5;
  UpdateStatus(STATUS_RUN_WORK);
  disp.DisplayDot(false);
  disp.ClearDisplay();
  sound.PlayStart();
}

void StateMachine::Rest() {
  Serial.println("Rest");
  disp.DisplayDot(false);
  disp.ClearDisplay();
  UpdatePomodoro();
  if (pomodoro_completed % 3 == 0)
    pomodoro_minute = 30;
  else
    pomodoro_minute = 5;
  UpdateStatus(STATUS_RUN_REST);
  pomodoro_minute--;
  second = 59;
  sound.PlayFinish();
}

void StateMachine::Pause() {
  if (cur_status != STATUS_RUN_PAUSE) {
    Serial.println("Pause");
    pause_second = second;
    second = 59;
    UpdateStatus(STATUS_RUN_PAUSE);
    disp.DisplayString("PAUSE");
  } else {
    Serial.println("Unpause");
    second = pause_second;
    UpdateStatus(STATUS_RUN_WORK);
  }
  disp.DisplayDot(false);
  ClearDisplay();
}

void StateMachine::Cancel() {
  Serial.println("Cancel");
  if (cur_status == STATUS_RUN_WORK)
    UpdatePomodoro(false);

  pomodoro_minute = pomodoro_minute_last;
  disp.DisplayDot(false);
  ClearDisplay();
  sound.PlayCancel();
  Idle();
}

void StateMachine::Idle() {
  Serial.println("Idle");
  UpdateStatus(STATUS_IDLE);
  redraw = true;
  pomodoro_minute = pomodoro_minute_last;
  disp.DisplayDot(false);
  disp.ClearDisplay();
}

void StateMachine::UpdatePomodoro(bool success = true) {
  Serial.println("UpdatePomodoro");
  Serial.println(success);
  if (success) {
    pomodoro_completed++;
    userData[1]++;
    rtc.SaveData(userData);
    PrintUserData();
  } else
    pomodoro_completed = pomodoro_completed > 0 ? pomodoro_completed - 1 : 0;
}

void StateMachine::HandleClick() {
  int8_t action = button.GetClick();

  if (action)
    switch (cur_status) {
      case STATUS_IDLE:
        if (action == 2)
          Prepare();
        else
          IncreaseTime();
        break;
      case STATUS_RUN_WORK:
        if (action == 2)
          Cancel();
        else
          Pause();
        break;
      case STATUS_PREPARE:
      case STATUS_RUN_REST:
        Cancel();
        break;
      case STATUS_RUN_PAUSE:
        Pause();
        break;
      default:
        Serial.println("Click: Unknown status");
        Serial.println(cur_status);
    }
}

void StateMachine::ClearDisplay() {
  for (int8_t i = 0; i < 4; i++)
    timeDisp[i] = 0x40;
}

bool StateMachine::DisplayClick() {
  int8_t clickDisplay = button.GetDisplay();

  if (clickDisplay > 0) {
    if (clickDisplay != lastClickDisplay) {
      lastClickDisplay = clickDisplay;
      ClearDisplay();

      for (uint8_t i = 2; i <= clickDisplay; i++)
        timeDisp[i - 2] = '-';

      disp.DisplayDot(false);
      disp.DisplayClock(timeDisp);

      if (clickDisplay == 2)
        sound.PlayNote(NOTE_LOW);
      else if (clickDisplay == 5)
        sound.PlayNote(NOTE_HIGH);
    }
    return true;
  }
  lastClickDisplay = 0;
  return false;
}

void StateMachine::UpdateDisplay() {
  if (!DisplayClick())
    if (redraw) {
      switch (cur_status) {
        case STATUS_IDLE:
          disp.DisplayDot(false);
          timeDisp[0] = pomodoro_minute / 10;
          timeDisp[1] = pomodoro_minute % 10;
          timeDisp[2] = 0x40;
          timeDisp[3] = pomodoro_completed % 10;
          break;
        case STATUS_PREPARE:
          timeDisp[0] = 0x40;
          timeDisp[1] = 0x40;
          timeDisp[2] = second / 10;
          timeDisp[3] = second % 10;
          disp.DotNext();
          break;
        case STATUS_RUN_WORK:
        case STATUS_RUN_REST:
          timeDisp[0] = pomodoro_minute / 10;
          timeDisp[1] = pomodoro_minute % 10;
          timeDisp[2] = second / 10;
          timeDisp[3] = second % 10;
          disp.DotNext();
          break;
        case STATUS_RUN_PAUSE:
          ClearDisplay();
          disp.DotNext();
          break;
      }
      disp.DisplayClock(timeDisp);
      redraw = false;
    }
}

void StateMachine::MinusSecond() {
  Serial.println("MinusSecond");
  switch (cur_status) {
    case STATUS_IDLE:
      idle_time++;
      if (idle_time == 5) {
        idle_time = 0;
        PrintStats();
      }
      break;
    case STATUS_PREPARE:
      Start();
      break;
    case STATUS_RUN_WORK:
    case STATUS_RUN_REST:
      if (pomodoro_minute == 0) {
        if (cur_status == STATUS_RUN_WORK) {
          Rest();
        } else {
          Idle();
          sound.PlaySecond();
        }
      } else {
        pomodoro_minute--;
      }
      break;
    case STATUS_RUN_PAUSE:
      pause_minute--;
      if (pause_minute < 0)
        Cancel();
      break;
  }
  seconds_event = false;
}

void StateMachine::Update() {
  if (seconds_event)
    MinusSecond();
  else {
    HandleClick();
    UpdateDisplay();
    if (second == 30)
      if (cur_status == STATUS_RUN_WORK)
        disp.DisplayString("[11 11 11]");
      else if (cur_status == STATUS_RUN_REST)
        disp.DisplayString("[00 00 00]");
      else if (cur_status == STATUS_RUN_PAUSE)
        disp.DisplayString("PAUSE");
  }
}
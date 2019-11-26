#include "Button.h"
#include <ESP8266WiFi.h>
#include <time.h>
#include <sys/time.h>
#include <LedControl.h>

#define LIGHT_SENSOR  A0

#define DIN           D1
#define LOAD          D2
#define CLK           D0

#define BUTTON1       D4
#define BUTTON2       D8

#define MODE_HH_MM    0
#define MODE_MM_SS    1
#define MODE_SET_H    2
#define MODE_SET_M    3

#define INTENSITY_LOW   0
#define INTENSITY_MID   4
#define INTENSITY_HIGH  15

#define SSID "xxxxxx"
#define PASSWORD "xxxxxx"

// Ukraine/Kiev use /usr/share/zoneinfo
#define TZ_ENV "EET-2EEST,M3.5.0/3,M10.5.0/4"

Button *btn1;
Button *btn2;

byte h = 0;
byte m = 0;
byte s = 0;

LedControl *lc;

void setup() {
  pinMode(DIN, OUTPUT);
  pinMode(LOAD, OUTPUT);
  pinMode(CLK, OUTPUT);

  lc = new LedControl(DIN, CLK, LOAD, 1); // pins and 1 indicator
  lc->setScanLimit(0, 3); // 4 digits
  lc->shutdown(0, false); // turn it on
  lc->setIntensity(0, INTENSITY_HIGH); // max intensity
  lc->setDigit(0, 0, 8, false);
  lc->setDigit(0, 1, 8, false);
  lc->setDigit(0, 2, 8, false);
  lc->setDigit(0, 3, 8, false);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  configTime(0, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");
  setenv("TZ", TZ_ENV, 0);


  btn1 = new Button(BUTTON1, LOW);
  btn1->auto_repeat_speed = 500;
  btn1->onClick = &btn1Clicked;
  btn2 = new Button(BUTTON2, HIGH);
  btn2->auto_repeat_speed = 500;
  btn2->onClick = &btn2Clicked;
}


static int mode = MODE_HH_MM;

/**
   top button click handler
   switches between hh:mm and mm:ss mode
   when the SET_xx mode is on increments the currently altered value
*/
void btn1Clicked(bool auto_repeated) {
  timeval tv;
  switch (mode) {
    case MODE_HH_MM:
      if (auto_repeated) return;
      mode = MODE_MM_SS;
      break;

    case MODE_MM_SS:
      if (auto_repeated) return;
      mode = MODE_HH_MM;
      break;

    case MODE_SET_H:
      gettimeofday(&tv, nullptr);
      tv.tv_sec += 60 * 60;
      settimeofday(&tv, nullptr);
      break;

    case MODE_SET_M:
      gettimeofday(&tv, nullptr);
      tv.tv_sec += 60;
      settimeofday(&tv, nullptr);
      break;
  }
}


void btn2Clicked(bool auto_repeated) {
  if (auto_repeated) return;

  switch (mode) {
    case MODE_HH_MM:
      mode = MODE_SET_H;
      break;

    case MODE_MM_SS:
      mode = MODE_HH_MM;
      break;

    case MODE_SET_H:
      mode = MODE_SET_M;
      break;

    case MODE_SET_M:
      mode = MODE_HH_MM;
      break;
  }
}


void loop() {
  static unsigned long timer = millis();
  static unsigned long lightTimer = timer;

  static int d1 = 0;
  static int d2 = 1;
  static int d3 = 2;
  static int d4 = 3;

  btn1->update();
  btn2->update();

  long unsigned now = millis();
  static int dotIsOn = false;
  static int dotCyclesCountDown = 0;
  static int blinkCyclesCountDown = 0;

  // update the display 10 times per second
  if (now - timer >= 100) {
    time_t t = time(nullptr);
    tm * lt = localtime(&t);

    // if the internal timer is changed significally for some reason catch up with it
    if (now - timer >= 5000) {
      timer = now;
    } else {
      timer += 100;
    }

    if (!dotCyclesCountDown--) {
      dotIsOn = false;
    }
    if (s != lt->tm_sec) {
      dotIsOn = true;
      dotCyclesCountDown = 4;
    }

    s = lt->tm_sec;
    m = lt->tm_min;
    h = lt->tm_hour;

    switch (mode) {
      case MODE_HH_MM:
      case MODE_SET_H:
      case MODE_SET_M:
        d4 = m % 10;
        d3 = m / 10;
        d2 = h % 10;
        d1 = h / 10;
        break;
      case MODE_MM_SS:
        d4 = s % 10;
        d3 = s / 10;
        d2 = m % 10;
        d1 = m / 10;
        break;
    }


    static int isHMon = true;
    if (!blinkCyclesCountDown--) {
      blinkCyclesCountDown = 2;
      isHMon = !isHMon;
    }


    if (mode == MODE_SET_H && !isHMon) {
      lc->setChar(0, 0, ' ', false);
      lc->setChar(0, 1, ' ', false);
      lc->setDigit(0, 2, d3, dotIsOn);
      lc->setDigit(0, 3, d4, false);
    } else if (mode == MODE_SET_M && !isHMon) {
      lc->setDigit(0, 0, d1, false);
      lc->setDigit(0, 1, d2, dotIsOn);
      lc->setChar(0, 2, ' ', dotIsOn);
      lc->setChar(0, 3, ' ', false);
    } else {
      lc->setDigit(0, 0, d1, false);
      lc->setDigit(0, 1, d2, dotIsOn);
      lc->setDigit(0, 2, d3, dotIsOn);
      lc->setDigit(0, 3, d4, false);
    }
  }

  // update the level of light every 5 sec
  if (now - lightTimer >= 5000) {
    lightTimer = now;

    int sensor = analogRead(LIGHT_SENSOR);
    int lightLevel = 2 - map(constrain(sensor, 20, 900), 20, 900, 0, 2);
    int intesity;
    switch (lightLevel) {
      case 0:
        intesity = INTENSITY_LOW;
        break;
      case 1:
        intesity = INTENSITY_MID;
        break;
      case 2:
        intesity = INTENSITY_HIGH;
        break;
    }
    lc->setIntensity(0, intesity);
  }
}

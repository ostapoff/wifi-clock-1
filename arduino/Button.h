#ifndef Button_h
#define Button_h

#include "Arduino.h"

class Button {
  public:
    Button(int pin, bool pressed_state);
    void update();
    void (*onClick)(bool auto_repeated);
    bool auto_repeat = true;
    int auto_repeat_delay = 1000;
    int auto_repeat_speed = 100;
    unsigned long debounce_proof_interval_us = 100000;
    
  private:
    int pin;
    bool pressed_state;
    bool last_press_state;
    bool is_auto_repeating;
    bool debouncing_value;
    unsigned long debounce_proof_interval_start_us;
    unsigned long press_started_at_ms;
    unsigned long repeat_started_at_ms;
    bool is_btn_pressed();
    void update_debounce();
};

#endif

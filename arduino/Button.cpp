#include "Button.h"

Button::Button(int pin, bool pressed_state)
{
  this->pin = pin;
  this->pressed_state = pressed_state;
  pinMode(pin, INPUT);
  last_press_state = debouncing_value = is_btn_pressed();
  debounce_proof_interval_start_us = micros();
  onClick = 0;
}

void Button::update_debounce()
{
  int new_value = is_btn_pressed();
  if (new_value != debouncing_value) {
    unsigned long now_us = micros();
    if (now_us - debounce_proof_interval_start_us >= debounce_proof_interval_us) {
        debounce_proof_interval_start_us = now_us;
        debouncing_value = new_value;
    }
  }
}


bool Button::is_btn_pressed()
{
  return (digitalRead(pin) == pressed_state);
}


void Button::update()
{
  update_debounce();
  unsigned long now_ms = millis();
  bool press_state = debouncing_value;
  if (press_state) {
    if (press_state != last_press_state) { // starting the press
      press_started_at_ms = now_ms;
      is_auto_repeating = false;
      if (onClick) {
        onClick(false);
      }
    } else { // has been already pressed
      if (auto_repeat) {
        if (!is_auto_repeating) {
          if (now_ms - press_started_at_ms > auto_repeat_delay) { // 1 second before auto-repeat
            is_auto_repeating = true;
            repeat_started_at_ms = now_ms + auto_repeat_speed; // run autorepeat now
          }
        }

        if (is_auto_repeating) {
          if (now_ms - repeat_started_at_ms > auto_repeat_speed) { // .2s each auto repeat
            repeat_started_at_ms = now_ms;
            if (onClick) {
              onClick(true);
            }
          }
        }
      }
    }
  }
  last_press_state = press_state;
}

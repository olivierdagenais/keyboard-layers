// Arduino sketch to enhance an ergonomic keyboard.
// Copyright (C) 2021  Olivier Dagenais. All rights reserved.
// Licensed under the GNU General Public License. See LICENSE in the project
// root.

#include "Mouse.h"

typedef void (*callback_no_params)(void);

class Button {
  protected:
    const uint8_t _buttonPin;
    const callback_no_params _pressed;
    const callback_no_params _released;

    volatile bool _currentButtonState;

    virtual bool isPressed() {
        int v = digitalRead(_buttonPin);
        return (v == 0);
    }

  public:
    Button(uint8_t buttonPin,
           callback_no_params pressed,
           callback_no_params released)
        : _buttonPin(buttonPin), _pressed(pressed), _released(released) {
        _currentButtonState = false;
        pinMode(_buttonPin, INPUT_PULLUP);
    }

    void scan() {
        bool newState = isPressed();
        if (_currentButtonState) { // button was previously pressed
            if (!newState) {       // now it's released
                _currentButtonState = false;
                if (_pressed != NULL) {
                    _pressed();
                }
            }
        }
        else {              // button wasn't pressed
            if (newState) { // now it's pressed
                _currentButtonState = true;
                if (_released != NULL) {
                    _released();
                }
            }
        }
    }
};

const int NUM_BUTTONS = 5;
Button *_buttons[NUM_BUTTONS];

bool _mouseActive = false;
void toggleMouseActive() { _mouseActive = !_mouseActive; }

void setup() {
    _buttons[0] = new Button(2, &toggleMouseActive, NULL);
    _buttons[1] = new Button(3, &toggleMouseActive, NULL);
    _buttons[2] = new Button(4, &toggleMouseActive, NULL);
    _buttons[3] = new Button(5, &toggleMouseActive, NULL);
    _buttons[4] = new Button(6, &toggleMouseActive, NULL);
    Serial.begin(115200);
    Mouse.begin();
}

void loop() {
    for (uint8_t b = 0; b < NUM_BUTTONS; b++) {
        _buttons[b]->scan();
    }

    delay(5);
}

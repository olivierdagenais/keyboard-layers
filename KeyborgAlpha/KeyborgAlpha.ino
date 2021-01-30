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

class AnalogAxis {
  protected:
    const uint8_t _analogPin;
    const uint16_t _minAnalogValue;
    const uint16_t _maxAnalogValue;
    const uint16_t _range;
    const uint16_t _threshold;
    const uint16_t _center;
    const uint8_t _throttle;

  public:
    AnalogAxis(uint8_t analogPin,
               int16_t minAnalogValue,
               int16_t maxAnalogValue,
               uint16_t range,
               uint16_t threshold,
               uint8_t throttle)
        : _analogPin(analogPin),
          _minAnalogValue(minAnalogValue),
          _maxAnalogValue(maxAnalogValue),
          _range(range),
          _threshold(threshold),
          _center(range / 2),
          _throttle(throttle) {
        // Do I need to initialize the pin?
    }

    int16_t read(uint8_t percent) {
        if (0 != (percent % _throttle)) {
            return 0;
        }
        int16_t reading = analogRead(_analogPin);
        int16_t adjusted =
            map(reading, _minAnalogValue, _maxAnalogValue, 0, _range);
        int16_t distance = adjusted - _center;
        if (abs(distance) < _threshold) {
            distance = 0;
        }
        return distance;
    }
};

const int NUM_BUTTONS = 5;
Button *_buttons[NUM_BUTTONS];

const int NUM_ANALOG_AXIS = 4;
AnalogAxis *_analogAxis[NUM_ANALOG_AXIS];

bool _mouseActive = false;
void toggleMouseActive() { _mouseActive = !_mouseActive; }

void setup() {
    _buttons[0] = new Button(2, &toggleMouseActive, NULL);
    _buttons[1] = new Button(3, &toggleMouseActive, NULL);
    _buttons[2] = new Button(4, &toggleMouseActive, NULL);
    _buttons[3] = new Button(5, &toggleMouseActive, NULL);
    _buttons[4] = new Button(6, &toggleMouseActive, NULL);

    _analogAxis[0] = new AnalogAxis(0, 1023, 0, 10, 2, 50);
    _analogAxis[1] = new AnalogAxis(1, 1023, 0, 10, 2, 50);
    // https://www.sparkfun.com/products/9426 (Thumb Slide Joystick) says:
    // "(...)you can expect a range of about 128 to 775 on each axis."
    _analogAxis[2] = new AnalogAxis(2, 128, 775, 12, 3, 16);
    _analogAxis[3] = new AnalogAxis(3, 128, 775, 12, 3, 16);
    Serial.begin(115200);
    Mouse.begin();
}

uint8_t _percent = 0;
void loop() {
    for (uint8_t b = 0; b < NUM_BUTTONS; b++) {
        _buttons[b]->scan();
    }

    int16_t w = _analogAxis[0]->read(_percent);
    int16_t z = _analogAxis[1]->read(_percent);
    int16_t x = _analogAxis[3]->read(_percent);
    int16_t y = _analogAxis[2]->read(_percent);
    int16_t wheel = (z == 0 ? 0 : (z > 0 ? -1 : 1));
    if (_mouseActive && (x != 0 || y != 0 || z != 0)) {
        Mouse.move(x, y, wheel);
    }

    _percent = (_percent + 1) % 100;
    delay(1);
}

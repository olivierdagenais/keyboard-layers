// Arduino sketch to enhance an ergonomic keyboard.
// Copyright (C) 2021  Olivier Dagenais. All rights reserved.
// Licensed under the GNU General Public License. See LICENSE in the project
// root.

#include "Mouse.h"

void setup() {
    // TODO: initialize hardware
    Serial.begin(115200);
    Mouse.begin();
}

void loop() {
    // TODO: scan inputs and translate them to outputs
    delay(1);
}

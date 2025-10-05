/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


// Blinking rate in milliseconds
#define BLINKING_RATE     500ms

DigitalOut led(LED1);
InterruptIn button(p30);

void light_on() {led=1;}
void light_off() {led=0;}

int main()
{
    button.rise(callback(light_off));
    button.fall(callback(light_on));


    while (true) {
        // led = !button;
        ThisThread::sleep_for(10ms);
    }
}

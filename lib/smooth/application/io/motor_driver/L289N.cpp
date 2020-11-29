/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "smooth/application/io/motor_driver/L289N.h"

namespace smooth::application::io
{
    static const char* TAG = "L289N";
    static const bool PIN_HIGH = true;
    static const bool PIN_LOW = false;
    
    L289N::L289N(gpio_num_t en, gpio_num_t in1, gpio_num_t in2, gpio_num_t sens): 
        en_pin(en, true, false, true),
        in1_pin(in1, true, false, true),
        in2_pin(in2, true, false, true),
        sens_pin(sens)
    {
    }

    void L289N::start(uint8_t direction, uint8_t speed) 
    {
        // set speed (TODO: PWM)
        en_pin.set(PIN_HIGH);
        
        // reset directions 
        in1_pin.clr();
        in2_pin.clr();

        // set direction
        if(direction) {
            in1_pin.set(PIN_HIGH);
        }
        else {
            in2_pin.set(PIN_HIGH);
        }
    }

    void L289N::set_speed(uint8_t speed)
    {
        // TODO: For now only ON-OFF => use PWM 
        if(speed) {
            en_pin.set(PIN_HIGH);
        }
        else {
            en_pin.clr();
        }
    }

    void L289N::stop()
    {
        // set speed to 0
        en_pin.clr();
        
        // reset directions 
        in1_pin.clr();
        in2_pin.clr();
    }
}
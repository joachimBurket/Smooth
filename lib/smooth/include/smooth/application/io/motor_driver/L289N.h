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

#pragma once

#include <driver/gpio.h>
#include "smooth/core/io/Output.h"
#include "smooth/core/io/Input.h"

namespace smooth::application::io
{
    class L289N
    {
        public:
            L289N(gpio_num_t en_pin, gpio_num_t in1_pin, gpio_num_t in2_pin, gpio_num_t sens_pin);

            void start(uint8_t direction, uint8_t speed);

            void set_speed(uint8_t speed);

            void stop();

        private:
            smooth::core::io::Output en_pin;
            smooth::core::io::Output in1_pin;
            smooth::core::io::Output in2_pin;
            smooth::core::io::Input sens_pin;       // TODO: create AnalogInput class? 
    };
}
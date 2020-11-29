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

#include "smooth/core/Application.h"
#include "smooth/core/task_priorities.h"
#include "smooth/application/io/motor_driver/L289N.h"

namespace l289n_test
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

            /// Creates an instance of the motor driver
            void init_l289n();

        private:

            smooth::application::io::L289N l289n{GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_34};
            uint8_t direction = 0;
            uint8_t count = 0;
    };
}

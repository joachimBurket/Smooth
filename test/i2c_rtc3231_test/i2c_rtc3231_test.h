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
#include "smooth/core/io/i2c/Master.h"
#include "smooth/application/io/i2c/DS3231.h"

namespace i2c_rtc3231_test
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

            void set_time();

            void get_time();

            void set_alarm1();

            void get_alarm1();

            bool is_alarm1_active();

            void clear_alarm1_active();

            void set_alarm2();

            void get_alarm2();

            bool is_alarm2_active();

            void clear_alarm2_active();
        private:
            void init_i2c_rtc3231();

            smooth::core::io::i2c::Master i2c0_master;
            std::unique_ptr<smooth::application::sensor::DS3231> rtc3231{};
            bool rtc3231_initialized{ false };
            int alarm1_active_count{ 0 };
            int alarm2_active_count{ 0 };
    };
}

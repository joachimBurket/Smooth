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

#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/core/util/FixedBuffer.h"
#include "smooth/core/io/RtcDevice.h"

namespace smooth::application::sensor
{
    class PCF8563
        : public core::io::i2c::I2CMasterDevice, core::io::rtc::RTCDevice
    {
        public:
            enum class Rtc8563Register : uint8_t
            {
                ControlStatus1 = 0x00,
                ControlStatus2,
                Seconds,
                Minutes,
                Hours,
                Days,
                Weekdays,
                Months,
                Years,
                MinuteAlarm,
                HourAlarm,
                DayAlarm,
                WeekdayAlarm,
                ClockOutControl,
                TimerControl,
                Timer
            };

            PCF8563(i2c_port_t port, uint8_t address, std::mutex& guard);

            /// Get the rtc time
            /// \param rtc_time The RtcTime struct that will contain the time data
            /// \return true on success, false on failure.
            bool get_rtc_time(core::io::rtc::RtcTime& rtc_time) override;

            /// Set the rtc time
            /// \param rtc_time The RtcTime struct that contains the time data
            /// \return true on success, false on failure.
            bool set_rtc_time(core::io::rtc::RtcTime& rtc_time) override;

            /// Get the alarm time
            /// \param alarm_time The AlarmTime struct that will contain the time data
            /// \return true on success, false on failure.
            bool get_alarm_time(core::io::rtc::AlarmTime& alarm_time);

            /// Set the alarm time
            /// \param alarm_time The AlarmTime struct that contains the time data
            /// \return true on success, false on failure.
            bool set_alarm_time(core::io::rtc::AlarmTime& alarm_time);

            /// Is alarm flag active - poll this function to see if alarm time has triggered
            /// \param alarm_flag If true alarm is active if false alarm id decactivated
            /// \return true on success, false on failure.
            bool is_alarm_flag_active(bool& alarm_flag);

            /// Clear the alarm flag
            /// \return true on success, false on failure.
            bool clear_alarm_flag();
    };
}

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

// DS3231 Temperature-Compensated Crystal Oscillator (TCXO) RTC module
//
// Inspired from https://github.com/rodan/ds3231
//
// The Library implements the following features:
// - set and get RTC time
// - enable/disable 32kHz output
// - set square wave frequency and enable/disable the output
// - set and get the alarm1 and alarm2 times, enable/disable the output interrupts and clear them
//

#pragma once

#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/core/util/FixedBuffer.h"
#include "smooth/core/rtc/RtcDevice.h"

namespace smooth::application::sensor
{
    class DS3231
            : public core::io::i2c::I2CMasterDevice, core::rtc::RTCDevice
    {
        public:
            enum class Rtc3231Registers : uint8_t
            {
                    Seconds = 0x00,
                    Minutes,
                    Hours,
                    Weekdays,
                    Days,
                    Months,
                    Year,
                    SecondsAlarm1,
                    MinutesAlarm1,
                    HoursAlarm1,
                    WeekdaysAndDaysAlarm1,
                    MinutesAlarm2,
                    HoursAlarm2,
                    WeekdaysAndDaysAlarm2,
                    Control,
                    Status,
                    AgingOffset,
                    Temperature,
            };

            enum class Rtc3231ControlRegisterBits : uint8_t
            {
                    A1IE = (1 << 0),    // alarm1 interrupt enable
                    A2IE = (1 << 1),    // alarm2 interrupt enable
                    INTCN = (1 << 2),   // Interrupt Control (1: A1F and A2F drive INT/SQW output ; 0: Square wave on
                    // INT/SQW output)
                    RS1 = (1 << 3),     // Square-wave freq select 1
                    RS2 = (1 << 4),     // Square-wave freq select 2
                    CONV = (1 << 5),    // Force temperature conversion
                    BBSQW = (1 << 6),   // Battery-backed Square-wave enable
                    EOSC = (1 << 7),    // Enable oscillator (0 equal ON)
            };

            enum class Rtc3231StatusRegisterBits : uint8_t
            {
                    A1F = (1 << 0),     // alarm1 Flag (1 if alarm1 was triggered)
                    A2F = (1 << 1),     // alarm2 Flag (1 if alarm2 was triggered)
                    BUSY = (1 << 2),    // Device is busy executing TCXO
                    EN32KHZ = (1 << 3), // Enable 32kHz Output (1 to enable)
                    OSF = (1 << 7),     // Oscillator Stop Flag  (if 1 then oscillator has stopped and date might be
                    // inaccurate)
            };

            enum class Rtc3231SquareWaveFreqs : uint8_t
            {
                    F1HZ = 0,   // 1Hz
                    F1KHZ,      // 1.024 kHz
                    F4KHZ,      // 4.096 kHz
                    F8KHZ,      // 8.192 kHz
            };

            DS3231(i2c_port_t port, uint8_t address, std::mutex& guard);

            /*******************************************************************************************************************
             * RTC time functions
             ******************************************************************************************************************/

            /// Get the rtc time
            /// \param rtc_time The RtcTime struct that will contain the time data
            /// \return true on success, false on failure.
            bool get_rtc_time(core::rtc::RtcTime& rtc_time) override;

            /// Set the rtc time
            /// \param rtc_time The RtcTime struct that contains the time data
            /// \return true on success, false on failure.
            bool set_rtc_time(core::rtc::RtcTime& rtc_time) override;

            /*******************************************************************************************************************
             * RTC alarms 1 and 2 functions
             ******************************************************************************************************************/

            /// Get the alarm1 time
            /// \param alarm_time The AlarmTime struct that will contain the time data
            /// \return true on success, false on failure.
            bool get_alarm1_time(core::rtc::AlarmTime& alarm_time);

            /// Set the alarm1 time
            /// \param alarm_time The AlarmTime struct that contains the time data
            /// \return true on success, false on failure.
            bool set_alarm1_time(core::rtc::AlarmTime& alarm_time);

            /// Enable the alarm1 interrupt (if INT/SQW is in INT mode, alarm1 flag drives the ouptut)
            /// \return true on success, false on failure.
            bool enable_alarm1_intr();

            /// Disable the alarm1 interrupt (alarm1 flag doesn't drive the INT/SQW output)
            /// \return true on success, false on failure.
            bool disable_alarm1_intr();

            /// Is alarm1 flag active - poll this function to see if alarm time has triggered
            /// \param alarm_flag If true alarm flag is active if false alarm
            /// \return true on success, false on failure.
            bool is_alarm1_flag_active(bool& alarm_flag);

            /// Clear the alarm1 interrupt flag
            /// \return true on success, false on failure.
            bool clear_alarm1_flag();

            /// Get the alarm2 time
            /// \param alarm_time The AlarmTime struct that will contain the time data
            /// \return true on success, false on failure.
            bool get_alarm2_time(core::rtc::AlarmTime& alarm_time);

            /// Set the alarm2 time
            /// \param alarm_time The AlarmTime struct that contains the time data
            /// \return true on success, false on failure.
            bool set_alarm2_time(core::rtc::AlarmTime& alarm_time);

            /// Enable the alarm2 interrupt (if INT/SQW is in INT mode, alarm2 flag drives the ouptut)
            /// \return true on success, false on failure.
            bool enable_alarm2_intr();

            /// Disable the alarm2 interrupt (alarm2 flag doesn't drive the INT/SQW output)
            /// \return true on success, false on failure.
            bool disable_alarm2_intr();

            /// Is alarm2 flag active - poll this function to see if alarm time has triggered
            /// \param alarm_flag If true alarm flag is active if false alarm
            /// \return true on success, false on failure.
            bool is_alarm2_flag_active(bool& alarm_flag);

            /// Clear the alarm2 interrupt flag
            /// \return true on success, false on failure.
            bool clear_alarm2_flag();

            /*******************************************************************************************************************
             * RTC Square Wave functions
             ******************************************************************************************************************/

            /// Set the square wave frequency
            /// \param freq The frequency
            /// \return true on success, false on failure.
            bool set_square_wave_freq(const enum Rtc3231SquareWaveFreqs freq);

            /// Get the square wave frequency
            /// \param freq The frequency
            /// \return true on success, false on failure.
            bool get_square_wave_freq(enum Rtc3231SquareWaveFreqs& freq);

            /*******************************************************************************************************************
             * Other functions
             ******************************************************************************************************************/

            /// Set the INT/SQW pin mode
            /// \param mode If 1, alarms flags drive the output. If 0, Square Wave enabled on the output.
            /// \return true on success, false on failure.
            bool set_intr_sqw_mode(const uint8_t mode);

            /// Enable/Disable the 32kHz output
            /// \param on The output state
            /// \return true on success, false on failure.
            bool set_32khz_output(const bool on);
        private:
            /*******************************************************************************************************************
             * RTC registers helpers
             ******************************************************************************************************************/

            /// Set a register value
            /// \param reg The register to modify
            /// \param val The value to set
            /// \return true on success, false on failure.
            bool set_reg(const Rtc3231Registers reg, const uint8_t val);

            /// Get a register value
            /// \param reg The register to modify
            /// \param val The value to set
            /// \return true on success, false on failure.
            bool get_reg(const Rtc3231Registers reg, uint8_t& val);

            /// Set bits of a register
            /// \param reg The register to modify
            /// \param bits The bits to be set
            /// \return true on success, false on failure.
            bool set_reg_bits(const Rtc3231Registers reg, const uint8_t bits);

            /// Clear bits of a register
            /// \param reg The register to modify
            /// \param bits The bits to be cleared
            /// \return true on success, false on failure.
            bool clear_reg_bits(const Rtc3231Registers reg, const uint8_t bits);

            /// Read OSF (Oscillator Stop Flag). If 1, date might be inaccurate
            /// \param flag The flag value
            /// \return true on success, false on failure.
            bool get_osf(bool& flag);
    };
}

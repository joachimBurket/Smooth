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

#include <vector>
#include "smooth/application/io/i2c/DS3231.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core;
using namespace smooth::core::logging;
using namespace smooth::core::io::rtc;

namespace smooth::application::sensor
{
    // Class constants
    static const char* TAG = "DS3231";

    // Constructor
    DS3231::DS3231(i2c_port_t port, uint8_t address, std::mutex& guard)
            : I2CMasterDevice(port, address, guard)
    {
    }

    // Get time
    bool DS3231::get_rtc_time(RtcTime& rtc_time)
    {
        util::FixedBuffer<uint8_t, 7> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc3231Registers::Seconds), rd_data);

        bool oscillator_stopped;
        res &= get_osf(oscillator_stopped);

        if (res)
        {
            if (oscillator_stopped)
            {
                Log::warning(TAG, "Oscillator has stopped, date might be inaccurate");
            }
            else
            {
                rtc_time.seconds = bcd_to_decimal(rd_data[0] & 0x7F);
                rtc_time.minutes = bcd_to_decimal(rd_data[1] & 0x7F);
                rtc_time.hours24 = bcd_to_decimal(rd_data[2] & 0x3F);
                rtc_time.weekdays = static_cast<DayOfWeek>(bcd_to_decimal(rd_data[3] & 0x07));
                rtc_time.days = bcd_to_decimal(rd_data[4] & 0x3F);
                rtc_time.months = static_cast<Month>(bcd_to_decimal(rd_data[5] & 0x1F));
                rtc_time.years = static_cast<uint16_t>(2000 + bcd_to_decimal(rd_data[6]));
            }
        }

        return res & !oscillator_stopped;
    }

    // Set time
    bool DS3231::set_rtc_time(RtcTime& rtc_time)
    {
        // validate rtc time values
        validate_time(rtc_time.seconds, "RTC seconds", 0, 59);
        validate_time(rtc_time.minutes, "RTC minutes", 0, 59);
        validate_time(rtc_time.hours24, "RTC hours", 0, 23);
        validate_year(rtc_time.years);
        validate_time(rtc_time.days, "RTC days", 1,
                      number_of_days_in_month(rtc_time.months, rtc_time.years));

        // write rtc time values to chip
        std::vector<uint8_t> data{ static_cast<uint8_t>(Rtc3231Registers::Seconds) };
        data.push_back(decimal_to_bcd(rtc_time.seconds));
        data.push_back(decimal_to_bcd(rtc_time.minutes));
        data.push_back(decimal_to_bcd(rtc_time.hours24));
        data.push_back(static_cast<uint8_t>(rtc_time.weekdays));
        data.push_back(decimal_to_bcd(rtc_time.days));
        data.push_back(decimal_to_bcd(static_cast<uint8_t>(rtc_time.months)));
        data.push_back(decimal_to_bcd(static_cast<uint8_t>(rtc_time.years - 2000)));
        bool res = write(address, data);

        if (res)
        {
            // Clear 'Oscillator Stopped' flag
            res = clear_reg_bits(Rtc3231Registers::Status, static_cast<uint8_t>(Rtc3231StatusRegisterBits::OSF));
        }

        return res;
    }

    // Get alarm1 time
    bool DS3231::get_alarm1_time(AlarmTime& alarm_time)
    {
        util::FixedBuffer<uint8_t, 4> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc3231Registers::SecondsAlarm1), rd_data);

        if (res)
        {
            alarm_time.ena_alrm_second = (~rd_data[0] & 0x80);
            alarm_time.second = bcd_to_decimal(rd_data[0] & 0x7F);

            alarm_time.ena_alrm_minute = (~rd_data[1] & 0x80);
            alarm_time.minute = bcd_to_decimal(rd_data[1] & 0x7F);

            alarm_time.ena_alrm_hour = (~rd_data[2] & 0x80);
            alarm_time.hour24 = bcd_to_decimal(rd_data[2] & 0x3F);

            if (rd_data[3] & 0x40)  // weekday
            {
                alarm_time.ena_alrm_weekday = (~rd_data[3] & 0x80);
                alarm_time.weekday = static_cast<DayOfWeek>(bcd_to_decimal(rd_data[3] & 0x07));
                alarm_time.ena_alrm_day = false;
            }
            else    // date
            {
                alarm_time.ena_alrm_day = (~rd_data[3] & 0x80);
                alarm_time.day = bcd_to_decimal(rd_data[3] & 0x3F);
                alarm_time.ena_alrm_weekday = false;
            }
        }

        return res;
    }

    // Set alarm1 time
    bool DS3231::set_alarm1_time(AlarmTime& alarm_time)
    {
        // validate alarm time values
        validate_time(alarm_time.second, "ALARM second", 0, 59);
        validate_time(alarm_time.minute, "ALARM minute", 0, 59);
        validate_time(alarm_time.hour24, "ALARM hour", 0, 23);
        validate_time(alarm_time.day, "ALARM day", 1, 31);
        auto weekday = static_cast<uint8_t>(alarm_time.weekday);
        validate_time(weekday, "ALARM weekday", 1, 7);

        // write alarm time values to chip
        std::vector<uint8_t> data{ static_cast<uint8_t>(Rtc3231Registers::SecondsAlarm1) };
        data.push_back(decimal_to_bcd(alarm_time.second)
                       | static_cast<uint8_t>(alarm_time.ena_alrm_second ? 0x00 : 0x80));
        data.push_back(decimal_to_bcd(alarm_time.minute)
                       | static_cast<uint8_t>(alarm_time.ena_alrm_minute ? 0x00 : 0x80));
        data.push_back(decimal_to_bcd(alarm_time.hour24)
                       | static_cast<uint8_t>(alarm_time.ena_alrm_hour ? 0x00 : 0x80));

        if (alarm_time.ena_alrm_day)
        {
            data.push_back((decimal_to_bcd(alarm_time.day)
                            & static_cast<uint8_t>(0xBF))    // DY/DT = 0
                           | static_cast<uint8_t>(alarm_time.ena_alrm_day ? 0x00 : 0x80));
        }
        else
        {
            data.push_back(decimal_to_bcd(static_cast<uint8_t>(alarm_time.weekday))
                           | static_cast<uint8_t>(alarm_time.ena_alrm_weekday ? 0x00 : 0x80)
                           | static_cast<uint8_t>(0x40));       // DY/DT = 1
        }

        return write(address, data);
    }

    // Check to see if alarm1 flag is active
    bool DS3231::is_alarm1_flag_active(bool& alarm_flag)
    {
        uint8_t val;
        bool res = get_reg(Rtc3231Registers::Status, val);
        alarm_flag = (val & static_cast<uint8_t>(Rtc3231StatusRegisterBits::A1F));

        return res;
    }

    // Clear alarm1 flag
    bool DS3231::clear_alarm1_flag()
    {
        return clear_reg_bits(Rtc3231Registers::Status, static_cast<uint8_t>(Rtc3231StatusRegisterBits::A1F));
    }

    // alarm1 flag drives the INT/SQW output
    bool DS3231::enable_alarm1_intr()
    {
        return set_reg_bits(Rtc3231Registers::Control, static_cast<uint8_t>(Rtc3231ControlRegisterBits::A1IE));
    }

    // alarm1 flag DOESN'T drive the INT/SQW output
    bool DS3231::disable_alarm1_intr()
    {
        return clear_reg_bits(Rtc3231Registers::Control, static_cast<uint8_t>(Rtc3231ControlRegisterBits::A1IE));
    }

    // Get alarm2 time
    bool DS3231::get_alarm2_time(AlarmTime& alarm_time)
    {
        util::FixedBuffer<uint8_t, 3> rd_data;
        bool res = read(address, static_cast<uint8_t>(Rtc3231Registers::MinutesAlarm2), rd_data);

        if (res)
        {
            alarm_time.ena_alrm_minute = (~rd_data[0] & 0x80);
            alarm_time.minute = bcd_to_decimal(rd_data[0] & 0x7F);

            alarm_time.ena_alrm_hour = (~rd_data[1] & 0x80);
            alarm_time.hour24 = bcd_to_decimal(rd_data[1] & 0x3F);

            if (rd_data[2] & 0x40)  // weekday
            {
                alarm_time.ena_alrm_weekday = (~rd_data[2] & 0x80);
                alarm_time.weekday = static_cast<DayOfWeek>(bcd_to_decimal(rd_data[2] & 0x07));
                alarm_time.ena_alrm_day = false;
            }
            else    // date
            {
                alarm_time.ena_alrm_day = (~rd_data[2] & 0x80);
                alarm_time.day = bcd_to_decimal(rd_data[2] & 0x3F);
                alarm_time.ena_alrm_weekday = false;
            }
        }

        return res;
    }

    // Set alarm2 time
    bool DS3231::set_alarm2_time(AlarmTime& alarm_time)
    {
        // validate alarm time values
        validate_time(alarm_time.minute, "ALARM minute", 0, 59);
        validate_time(alarm_time.hour24, "ALARM hour", 0, 23);
        validate_time(alarm_time.day, "ALARM day", 1, 31);
        auto weekday = static_cast<uint8_t>(alarm_time.weekday);
        validate_time(weekday, "ALARM weekday", 1, 7);

        // write alarm time values to chip
        std::vector<uint8_t> data{ static_cast<uint8_t>(Rtc3231Registers::MinutesAlarm2) };
        data.push_back(decimal_to_bcd(alarm_time.minute)
                       | static_cast<uint8_t>(alarm_time.ena_alrm_minute ? 0x00 : 0x80));
        data.push_back(decimal_to_bcd(alarm_time.hour24)
                       | static_cast<uint8_t>(alarm_time.ena_alrm_hour ? 0x00 : 0x80));

        if (alarm_time.ena_alrm_day)
        {
            data.push_back((decimal_to_bcd(alarm_time.day)
                            & static_cast<uint8_t>(0xBF))         // DY/DT = 0
                           | static_cast<uint8_t>(alarm_time.ena_alrm_day ? 0x00 : 0x80));
        }
        else
        {
            data.push_back(decimal_to_bcd(static_cast<uint8_t>(alarm_time.weekday))
                           | static_cast<uint8_t>(alarm_time.ena_alrm_weekday ? 0x00 : 0x80)
                           | static_cast<uint8_t>(0x40));       // DY/DT = 1
        }

        return write(address, data);
    }

    // Check to see if alarm2 flag is active
    bool DS3231::is_alarm2_flag_active(bool& alarm_flag)
    {
        uint8_t val;
        bool res = get_reg(Rtc3231Registers::Status, val);
        alarm_flag = (val & static_cast<uint8_t>(Rtc3231StatusRegisterBits::A2F));

        return res;
    }

    // Clear alarm2 flag
    bool DS3231::clear_alarm2_flag()
    {
        return clear_reg_bits(Rtc3231Registers::Status, static_cast<uint8_t>(Rtc3231StatusRegisterBits::A2F));
    }

    // alarm2 flag drives the INT/SQW output
    bool DS3231::enable_alarm2_intr()
    {
        return set_reg_bits(Rtc3231Registers::Control, static_cast<uint8_t>(Rtc3231ControlRegisterBits::A2IE));
    }

    // alarm2 flag DOESN'T drive the INT/SQW output
    bool DS3231::disable_alarm2_intr()
    {
        return clear_reg_bits(Rtc3231Registers::Control, static_cast<uint8_t>(Rtc3231ControlRegisterBits::A2IE));
    }

    // Enable/Disable 32khz output
    bool DS3231::enable_32khz_output(const bool on)
    {
        bool res;
        if (on)
        {
            res = set_reg_bits(Rtc3231Registers::Status, static_cast<uint8_t>(Rtc3231StatusRegisterBits::EN32KHZ));
        }
        else
        {
            res = clear_reg_bits(Rtc3231Registers::Status, static_cast<uint8_t>(Rtc3231StatusRegisterBits::EN32KHZ));
        }
        return res;
    }

    // Set a register's value
    bool DS3231::set_reg(const DS3231::Rtc3231Registers reg, const uint8_t val)
    {
        std::vector<uint8_t> data{ static_cast<uint8_t>(reg) };
        data.push_back(val);

        return write(address, data);
    }

    // Get a register's value
    bool DS3231::get_reg(const DS3231::Rtc3231Registers reg, uint8_t& val)
    {
        util::FixedBuffer<uint8_t, 1> rd_data;
        bool res = read(address, static_cast<uint8_t>(reg), rd_data);
        val = rd_data[0];

        return res;
    }

    // Set bits of a register
    bool DS3231::set_reg_bits(const DS3231::Rtc3231Registers reg, const uint8_t bits)
    {
        uint8_t val;
        bool res = get_reg(reg, val);
        val |= bits;

        return res && set_reg(reg, val);
    }

    // Clear bits of a register
    bool DS3231::clear_reg_bits(const DS3231::Rtc3231Registers reg, const uint8_t bits)
    {
        uint8_t val;
        bool res = get_reg(reg, val);
        val = static_cast<uint8_t>(val) & static_cast<uint8_t>(~bits);

        return res && set_reg(reg, val);
    }

    // Get "Oscillator Stop Flag"
    bool DS3231::get_osf(bool& flag)
    {
        uint8_t val;
        bool res = get_reg(Rtc3231Registers::Status, val);
        flag = val & static_cast<uint8_t>(Rtc3231StatusRegisterBits::OSF);

        return res;
    }

    // Choose the mode of the INT/SQW output (either alarms interrupt or Square Wave)
    bool DS3231::set_intr_sqw_mode(const uint8_t mode)
    {
        bool res;
        if (mode)
        {
            res = set_reg_bits(Rtc3231Registers::Control, static_cast<uint8_t>(Rtc3231ControlRegisterBits::INTCN));
        }
        else
        {
            res = clear_reg_bits(Rtc3231Registers::Control, static_cast<uint8_t>(Rtc3231ControlRegisterBits::INTCN));
        }
        return res;
    }

    // Set the Square Wave frequency
    bool DS3231::set_square_wave_freq(const DS3231::Rtc3231SquareWaveFreqs freq)
    {
        return set_reg_bits(Rtc3231Registers::Control, static_cast<uint8_t>(static_cast<uint8_t>(freq) << 3));
    }

    // Set the Square Wave frequency
    bool DS3231::get_square_wave_freq(DS3231::Rtc3231SquareWaveFreqs& freq)
    {
        uint8_t val;
        bool res = get_reg(Rtc3231Registers::Control, val);
        val = (val & 0x1F) >> 3;
        freq = static_cast<Rtc3231SquareWaveFreqs>(val);

        return res;
    }
}

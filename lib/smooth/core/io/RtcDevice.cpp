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

#include <cstdint>
#include <string>
#include <iostream>

#include "smooth/core/io/RtcDevice.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::io::rtc {

static const char* TAG = "RtcDevice";

// Compare two RtcTimes
bool RtcTime::operator==(const RtcTime &rhs) {
    return (this->seconds == rhs.seconds) && (this->minutes == rhs.minutes)
        && (this->hours24 == rhs.hours24) && (this->days == rhs.days)
        && (this->weekdays == rhs.weekdays) && (this->months == rhs.months)
        && (this->years == rhs.years);
}

// Print RtcTime
std::ostream &operator<<( std::ostream &output, const RtcTime &time ) {
    output << DayOfWeekStrings[static_cast<uint8_t>(time.weekdays)] << " " << +time.days << " "
    << MonthStrings[static_cast<uint8_t>(time.months)] << " " << time.years << ", "
    << +time.hours24 << "h" << +time.minutes << "m" << +time.seconds << "s";
    return output;
}

// Compare two AlarmTimes
bool AlarmTime::operator==(const AlarmTime &rhs) {
    return (this->ena_alrm_second == rhs.ena_alrm_second) && (this->second == rhs.second)
            && (this->ena_alrm_minute == rhs.ena_alrm_minute) && (this->minute == rhs.minute)
            && (this->ena_alrm_hour == rhs.ena_alrm_hour) && (this->hour24 == rhs.hour24)
            && (this->ena_alrm_day == rhs.ena_alrm_day) && (this->day == rhs.day)
            && (this->ena_alrm_weekday == rhs.ena_alrm_weekday) && (this->weekday == rhs.weekday);
}

// Print AlarmTime
std::ostream &operator<<( std::ostream &output, const AlarmTime &time ) {
    int alarm_mask =    static_cast<int>(time.ena_alrm_second) 
                            + (static_cast<int>(time.ena_alrm_minute) << 1) 
                            + (static_cast<int>(time.ena_alrm_hour) << 2) 
                            + (static_cast<int>(time.ena_alrm_day) << 3)
                            + (static_cast<int>(time.ena_alrm_weekday) << 4);

    Log::info("Alarm_mask = {}", alarm_mask);

    switch(alarm_mask) {
        case 0: 
            output << "once per second";
            break;
        case 1: 
            output << "each minute when " << +time.second << "sec match";
            break;
        case 3:
            output << "each hour when " << +time.minute << "min " << +time.second << "sec match";
            break;
        case 7:
            output << "each day at " << +time.hour24 << "h" << +time.minute << "m" << +time.second << "s";
            break;
        case 15: 
            output << "each month the " << +time.day << "th at " << +time.hour24 << "h" << +time.minute << "m" << +time.second << "s";
            break;
        case 23: 
            output << "each " << DayOfWeekStrings[static_cast<uint8_t>(time.weekday)] << " at " << +time.hour24 << "h" << +time.minute << "m" << +time.second << "s";
            break;
        default: 
            Log::warning("Alarm_mask value incorrect: {}", alarm_mask);
    }   
    return output;
}

// Convert BCD to decimal
uint8_t bcd_to_decimal(uint8_t bcd) {
    return static_cast<uint8_t>((10 * ((bcd & 0xf0) >> 4)) + (bcd & 0xf));
}

// Convert decimal to BCD
uint8_t decimal_to_bcd(uint8_t decimal) {
    return static_cast<uint8_t>(((decimal / 10) << 4) | (decimal % 10));
}

// The number of days in the month
uint8_t number_of_days_in_month(Month month, uint16_t year) {
    uint8_t days = 31;

    if ((month == Month::April) | (month == Month::June) | (month == Month::September) | (month == Month::November)) {
        days = 30;
    }

    if (month == Month::February) {
        // if leap year then days = 29 otherwise days = 28
        days = ((year % 4 == 0 && year % 100 != 0) | (year % 400 == 0)) ? 29 : 28;
    }

    return days;
}

// Add colon and zero padding to number
std::string add_colon_zero_padding(uint8_t time)
{
    return time < 10 ? ":0" + std::to_string(time) : ":" + std::to_string(time);
}

// Get 12 hour time string
std::string get_12hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds)
{
    std::string hrs_str = hours_24 == 0 ? std::to_string(12) : std::to_string(hours_24 % 12);
    std::string am_pm_str = hours_24 < 12 ? " AM" : " PM";

    return hrs_str + add_colon_zero_padding(minutes) + add_colon_zero_padding(seconds) + am_pm_str;
}

// Get 24 hour time string
std::string get_24hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds)
{
    return std::to_string(hours_24) + add_colon_zero_padding(minutes) + add_colon_zero_padding(seconds);
}

// Validate time
void validate_time(uint8_t& time, std::string err_msg, uint8_t min_limit, uint8_t max_limit)
{
    if ((time > max_limit) | (time < min_limit))
    {
        Log::error(TAG,
                   "Error - {} must be between {} and {}, setting to {}",
                   err_msg,
                   min_limit,
                   max_limit,
                   min_limit);

        time = min_limit;
    }
}

// Validate year
void validate_year(uint16_t& year)
{
    if ((year > 2099) | (year < 2000))
    {
        Log::error(TAG, "Error - RTC year must be between 2000 and 2099, setting to 2000");
        year = 2000;
    }
}

}
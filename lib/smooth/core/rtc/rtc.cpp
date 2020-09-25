//
// Created by joachim on 30.08.20.
//
// Utility functions for the rtc devices
//

#include "smooth/core/rtc/rtc.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::rtc {

static const char* TAG = "RTC";

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
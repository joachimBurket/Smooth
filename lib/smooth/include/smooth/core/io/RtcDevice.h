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


namespace smooth::core::io::rtc {

enum class DayOfWeek : uint8_t {
    Sunday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday
};

static const constexpr char *DayOfWeekStrings[] =
        {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

enum class Month : uint8_t {
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
};

static const constexpr char *MonthStrings[] =
        {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

struct RtcTime {
    uint8_t seconds = 0;                    // 0-59
    uint8_t minutes = 0;                    // 0-59
    uint8_t hours24 = 0;                    // 0-23
    uint8_t days = 1;                       // 1-31 depending upon month
    DayOfWeek weekdays = DayOfWeek::Sunday; // Sunday-Saturday
    Month months = Month::January;          // January-December
    uint16_t years = 2000;                  // 2000-2099
    
    // Compare two RtcTimes
    bool operator==(const RtcTime &rhs);
    
    // Print RtcTime
    friend std::ostream &operator<<( std::ostream &output, const RtcTime &time );
};

struct AlarmTime {
    bool ena_alrm_second = false;           // true = enable, false = disable
    uint8_t second = 0;                     // 0-59
    bool ena_alrm_minute = false;           // true = enable, false = disable
    uint8_t minute = 0;                     // 0-59
    bool ena_alrm_hour = false;             // true = enable, false = disable
    uint8_t hour24 = 0;                     // 0-23
    bool ena_alrm_day = false;              // true = enable, false = disable
    uint8_t day = 1;                        // 1-31 depending upon month
    bool ena_alrm_weekday = false;          // true = enable, false = disable
    DayOfWeek weekday = DayOfWeek::Sunday;  // Sunday-Saturday

    // Compare two AlarmTimes
    inline bool operator==(const AlarmTime &rhs);

    // Print AlarmTime
    friend std::ostream &operator<<( std::ostream &output, const AlarmTime &time );
};

/// BCD to decimal
/// \param bcd The bcd value to be convert to decimal
/// \return Return the decimal value of bcd
uint8_t bcd_to_decimal(uint8_t bcd);

/// Decimal to BCD
/// \param decimal The decimal value to be converted to BCD
/// \return Return the BCD value of decimal
uint8_t decimal_to_bcd(uint8_t decimal);

/// Number of days in a month
/// \param month The month used to determine the number of days in the month
/// \param year The year used to determine the number of days in the month
/// \return Return the number of days in the month
uint8_t number_of_days_in_month(Month month, uint16_t year);

/// Add leading colon and zero padding if necessary
/// \param time The time unit to add colon and pad with zero if necessary
/// \return Return the colon and zero padded number in string format
std::string add_colon_zero_padding(uint8_t time);

/// Get the 12 hour time string
/// \param hours_24 The decimal 24 hours time
/// \param minutes The decimal minutes time
/// \param seconds The decimal seconds time
/// \return Return the formated time string - hr:min:sec am/pm
std::string get_12hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds);

/// Get the 24 hour time string
/// \param hours_24 The decimal 24 hours time
/// \param minutes The decimal minutes time
/// \param seconds The decimal seconds time
/// \return Return the formated time string - hr:min:sec
std::string get_24hr_time_string(uint8_t hours_24, uint8_t minutes, uint8_t seconds);

/// Validate the unit of time
/// \param time The unit of time to validate
/// \param err_msg The text string to place at the start of the error message
/// \param min_limit The minimum time unit amount
/// \param max_limit The maximum time unit amoint
void validate_time(uint8_t& time, std::string err_msg, uint8_t min_limit, uint8_t max_limit);

/// Validate year
/// \param year The year value that will be validated
void validate_year(uint16_t& year);

struct RTCDevice {
    public:
        virtual ~RTCDevice() = default;

        /// Get the rtc time
        /// \param rtc_time The RtcTime struct that will contain the time data
        /// \return true on success, false on failure.
        virtual bool get_rtc_time(RtcTime& rtc_time) = 0;

        /// Set the rtc time
        /// \param rtc_time The RtcTime struct that contains the time data
        /// \return true on success, false on failure.
        virtual bool set_rtc_time(RtcTime& rtc_time) = 0;
};

}
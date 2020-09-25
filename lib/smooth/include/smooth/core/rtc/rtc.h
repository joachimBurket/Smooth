//
// Created by joachim on 30.08.20.
//
// Utility enums and functions for the rtc devices
//

#include <cstdint>
#include <string>
#include <iostream>

namespace smooth::core::rtc {

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
    uint8_t seconds;        // 0-59
    uint8_t minutes;        // 0-59
    uint8_t hours24;        // 0-23
    uint8_t days;           // 1-31 depending upon month
    DayOfWeek weekdays;     // Sunday-Saturday
    Month months;           // January-December
    uint16_t years;         // 2000-2099

    // Compare two RtcTimes
    inline bool operator==(const RtcTime &rhs) {
        return (this->seconds == rhs.seconds) && (this->minutes == rhs.minutes)
            && (this->hours24 == rhs.hours24) && (this->days == rhs.days)
            && (this->weekdays == rhs.weekdays) && (this->months == rhs.months)
            && (this->years == rhs.years);
    }

    // Print RtcTime
    friend std::ostream &operator<<( std::ostream &output, const RtcTime &time ) {
        output << DayOfWeekStrings[static_cast<uint8_t>(time.weekdays)] << " " << +time.days << " "
        << MonthStrings[static_cast<uint8_t>(time.months)] << " " << time.years << ", "
        << +time.hours24 << "h" << +time.minutes << "m" << +time.seconds << "s";
        return output;
    }
};

struct AlarmTime {
    bool ena_alrm_second;   // true = enable, false = disable
    uint8_t second;         // 0-59
    bool ena_alrm_minute;   // true = enable, false = disable
    uint8_t minute;         // 0-59
    bool ena_alrm_hour;     // true = enable, false = disable
    uint8_t hour24;         // 0-23
    bool ena_alrm_day;      // true = enable, false = disable
    uint8_t day;            // 1-31 depending upon month
    bool ena_alrm_weekday;  // true = enable, false = disable
    DayOfWeek weekday;      // Sunday-Saturday

    // Default values
    AlarmTime()
        : ena_alrm_second(false), second(0), ena_alrm_minute(false), minute(0), ena_alrm_hour(false),
          hour24(0), ena_alrm_day(false), day(1), ena_alrm_weekday(false), weekday(DayOfWeek::Monday)
    {}

    // Compare two AlarmTimes
    inline bool operator==(const AlarmTime &rhs) {
        return (this->ena_alrm_second == rhs.ena_alrm_second) && (this->second == rhs.second)
               && (this->ena_alrm_minute == rhs.ena_alrm_minute) && (this->minute == rhs.minute)
               && (this->ena_alrm_hour == rhs.ena_alrm_hour) && (this->hour24 == rhs.hour24)
               && (this->ena_alrm_day == rhs.ena_alrm_day) && (this->day == rhs.day)
               && (this->ena_alrm_weekday == rhs.ena_alrm_weekday) && (this->weekday == rhs.weekday);
    }

    // Print AlarmTime
    friend std::ostream &operator<<( std::ostream &output, const AlarmTime &alarm_time ) {
        output << +alarm_time.second << " seconds," << (alarm_time.ena_alrm_second ? " enabled \n" : " disabled \n")
               << +alarm_time.minute << " minutes," << (alarm_time.ena_alrm_minute ? " enabled \n" : " disabled \n")
               << +alarm_time.hour24 << " hours, " << (alarm_time.ena_alrm_hour ? " enabled \n" : " disabled \n")
               << "day: " << +alarm_time.day << (alarm_time.ena_alrm_day ? ", enabled \n" : ", disabled \n")
               << "weekday: " << DayOfWeekStrings[static_cast<uint8_t>(alarm_time.weekday)] << (alarm_time.ena_alrm_weekday ? ", enabled \n" : ", disabled \n");
        return output;
    }
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

}
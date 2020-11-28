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

/****************************************************************************************
 * TODO: Redo with DS3231
 * Typical results from running program on M5StickC
 *
 * I (605) cpu_start: Starting scheduler on PRO CPU.
 * I (0) cpu_start: Starting scheduler on APP CPU.
 * I (669) FSLock: Max number of open files set to: 5
 * I (688) APP: Scanning for BM8563 ---- device found
 * cI (702) APP: BM8563 initialization --- Succeeded
 * I (702) APP: ********** Setting Time *********
 * I (703) APP: ********** Setting Alarm *********
 * W (60709) APP: ============ APP TICK TICK  =============
 * I (60710) MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
 * I (60716) MemStat: INTERNAL |      217260 |         113804 |       216088 |      273816 |         113804 |       272636
 * I (60727) MemStat:      DMA |      217260 |         113804 |       216088 |      217260 |         113804 |       216088
 * I (60739) MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
 * I (60750) MemStat:
 * I (60753) MemStat:             Name |      Stack |  Min free stack |  Max used stack
 * I (60761) MemStat:         MainTask |      16384 |           13980 |            2404
 * I (60770) MemStat: SocketDispatcher |      20480 |           18384 |            2096
 * I (60778) APP: ........................................
 * I (60785) APP: Time = Tue 25 Feb 2020 - 1:09:00 PM
 * I (60790) APP: Alarm set for -> 1:12:00 PM
 *
 *
 * W (240967) APP: ============ APP TICK TICK  =============
 * I (240967) MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
 * I (240974) MemStat: INTERNAL |      217260 |         113804 |       216088 |      273816 |         113804 |       272636
 * I (240985) MemStat:      DMA |      217260 |         113804 |       216088 |      217260 |         113804 |       216088
 * I (240997) MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
 * I (241008) MemStat:
 * I (241011) MemStat:             Name |      Stack |  Min free stack |  Max used stack
 * I (241019) MemStat:         MainTask |      16384 |           13532 |            2852
 * I (241028) MemStat: SocketDispatcher |      20480 |           18384 |            2096
 * I (241036) APP: ........................................
 * I (241043) APP: Time = Tue 25 Feb 2020 - 1:12:00 PM
 * I (241048) APP: Alarm set for -> 1:12:00 PM
 * I (241052) APP: The Alarm Active Count = 1
 *
 *
 * I (3846334) APP: ........................................
 * I (3846341) APP: Time = Tue 25 Feb 2020 - 2:12:05 PM
 * I (3846346) APP: Alarm set for -> 1:12:00 PM
 * I (3846351) APP: The Alarm Active Count = 2
 ****************************************************************************************/
#include <vector>
#include <string>
#include <sstream>  
#include "i2c_rtc3231_test.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/SystemStatistics.h"

using namespace smooth::core;
using namespace smooth::core::io;
using namespace std::chrono;
using namespace smooth::application::sensor;

namespace i2c_rtc3231_test
{
    static const char* TAG = "APP";

    App::App() : Application(APPLICATION_BASE_PRIO, seconds(60)),
                 i2c0_master(I2C_NUM_0,                       // I2C Port 0
                             GPIO_NUM_22,                     // SCL pin
                             false,                           // SCL internal pullup NOT enabled
                             GPIO_NUM_21,                     // SDA pin
                             false,                           // SDA internal pullup NOT enabled
                             400 * 1000)                     // clock frequency - 400kHz
    {
    }

    void App::init()
    {
        Application::init();

        init_i2c_rtc3231();

        if (rtc3231_initialized)
        {
            get_time();     // get rtc time. If device haven't any battery, the 'Oscillator stopped` flag should be ON
            set_time();
            rtc3231->clear_alarm1_flag();
            set_alarm1();
            set_alarm2();
            rtc3231->clear_alarm2_flag();
        }
    }

    void App::tick()
    {
        Log::warning(TAG, "============ APP TICK TICK  =============");
        SystemStatistics::instance().dump();
        Log::info(TAG, "........................................" );

        if (rtc3231_initialized)
        {
            get_time();
            get_alarm1();
            get_alarm2();

            if (is_alarm1_active())
            {
                alarm1_active_count += 1;
                Log::info(TAG, "The Alarm1 Active Count = {}", alarm1_active_count);
                clear_alarm1_active();
            }

            if (is_alarm2_active())
            {
                alarm2_active_count += 1;
                Log::info(TAG, "The Alarm2 Active Count = {}", alarm2_active_count);
                clear_alarm2_active();
            }
        }
    }

    // Initialize the RTC3231
    void App::init_i2c_rtc3231()
    {
        auto device = i2c0_master.create_device<DS3231>(0x68);   // i2c device address  0x68
        Log::info(TAG, "Scanning for DS3231 ---- {}", device->is_present() ? "device found" : "device NOT present");

        if (device->is_present())
        {
            rtc3231_initialized = true;
            rtc3231 = std::move(device);
        }

        Log::info(TAG, "DS3231 initialization --- {}", rtc3231_initialized ? "Succeeded" : "Failed");
    }

    // Set the time
    void App::set_time()
    {
        Log::info(TAG, "********** Setting Time *********");
        rtc::RtcTime tm;
        tm.minutes = 11;
        tm.hours24 = 13;
        tm.days = 25;
        tm.weekdays = rtc::DayOfWeek::Tuesday;
        tm.months = rtc::Month::February;
        tm.years = 2020;

        if (!rtc3231->set_rtc_time(tm))
        {
            Log::error(TAG, "Error setting RTC time");
        }
    }

    // Get the time
    void App::get_time()
    {
        rtc::RtcTime tm;

        if (rtc3231->get_rtc_time(tm))
        {
            Log::info(TAG, "Time = {} {} {} {} - {} ",
                      rtc::DayOfWeekStrings[static_cast<int>(tm.weekdays)],
                      tm.days,
                      rtc::MonthStrings[static_cast<int>(tm.months)],
                      tm.years,
                      rtc::get_24hr_time_string(tm.hours24, tm.minutes, tm.seconds));
        }
        else   
        {
            Log::error(TAG, "Error reading RTC time or oscillator has stopped");
        }
    }

    // Set alarm1 - alarm will activate 12 minutes past any hour of any day of any month of any weekday.
    void App::set_alarm1()
    {
        Log::info(TAG, "********** Setting Alarm1 *********");
        rtc::AlarmTime tm;
        tm.second = 10;
        tm.minute = 12;
        tm.hour24 = 13;
        tm.day = 25;
        tm.weekday = rtc::DayOfWeek::Tuesday;
        tm.ena_alrm_second = true;
        tm.ena_alrm_minute = false;
        tm.ena_alrm_hour = false;
        tm.ena_alrm_day = false;
        tm.ena_alrm_weekday = false;

        if (!rtc3231->set_alarm1_time(tm))
        {
            Log::error(TAG, "Error setting ALARM time");
        }
    }

    // Get Alarm1
    void App::get_alarm1()
    {
        rtc::AlarmTime tm;

        if (rtc3231->get_alarm1_time(tm))
        {
            std::stringstream ss; 
            ss << tm;
            Log::info(TAG, "Alarm1: {} ", ss.str());
        }
        else
        {
            Log::error(TAG, "Error reading ALARM time");
        }
    }

    // Is alarm1 active
    bool App::is_alarm1_active()
    {
        bool is_active;
        rtc3231->is_alarm1_flag_active(is_active);

        return is_active;
    }

    // Clear alarm1
    void App::clear_alarm1_active()
    {
        rtc3231->clear_alarm1_flag();
    }

    // Set alarm2 - alarm will activate 12 minutes past any hour of any day of any month of any weekday.
    void App::set_alarm2()
    {
        Log::info(TAG, "********** Setting Alarm2 *********");
        rtc::AlarmTime tm;
        tm.minute = 13;
        tm.hour24 = 13;
        tm.day = 25;
        tm.weekday = rtc::DayOfWeek::Tuesday;
        tm.ena_alrm_minute = true;
        tm.ena_alrm_hour = false;
        tm.ena_alrm_day = false;
        tm.ena_alrm_weekday = false;

        if (!rtc3231->set_alarm2_time(tm))
        {
            Log::error(TAG, "Error setting ALARM time");
        }
    }

    // Get Alarm2
    void App::get_alarm2()
    {
        rtc::AlarmTime tm;

        if (rtc3231->get_alarm2_time(tm))
        {
            std::stringstream ss; 
            ss << tm;
            Log::info(TAG, "Alarm2: {} ", ss.str());
        }
        else
        {
            Log::error(TAG, "Error reading ALARM time");
        }
    }

    // Is alarm2 active
    bool App::is_alarm2_active()
    {
        bool is_active;
        rtc3231->is_alarm2_flag_active(is_active);

        return is_active;
    }

    // Clear alarm2
    void App::clear_alarm2_active()
    {
        rtc3231->clear_alarm2_flag();
    }
}
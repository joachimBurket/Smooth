//
// Created by joachim on 30.08.20.
//
// RTC device interface.
//

#pragma once

#include <string>
#include <mutex>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <fmt/format.h>
#pragma GCC diagnostic pop
#include <iostream>
#include "smooth/core/rtc/rtc.h"

#ifdef ESP_PLATFORM
#include "esp_log.h"
#endif

namespace smooth::core::rtc {

struct RTCDevice {
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
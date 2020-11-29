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
 * Typical results from running program 
 * TODO:
 ****************************************************************************************/
#include "l289n_test.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/SystemStatistics.h"

using namespace smooth::core;
using namespace std::chrono;

namespace l289n_test
{
    static const char* TAG = "APP";

    App::App() : Application(APPLICATION_BASE_PRIO, seconds(10))
    {
    }

    void App::init()
    {
        Application::init();
    }

    void App::tick()
    {
        Log::warning(TAG, "============ APP TICK TICK  =============");
        SystemStatistics::instance().dump();
        Log::info(TAG, "........................................" );

        if((count % 2) == 0) {
            Log::info(TAG, "starting in direction {}", direction);
            direction = !direction;
            l289n.start(direction, 1);
        }
        else {
            Log::info(TAG, "stopping");
            l289n.stop();
        }

        count++;
    }
}
#pragma once
#include "time.h"
#include "string"

class GroupObject;

namespace OpenKNX
{
    class Common;
    class Console;

    namespace Time
    {
        class TimeProvider;
       
        class TimeManager
        {
            friend Common;
            friend Console;
            friend TimeProvider;
                TimeProvider* _timeProvider = nullptr;
            private:
                void setup();
                void loop();
                void processInputKo(GroupObject& ko);
                bool processCommand(std::string& cmd, bool diagnoseKo);
                void setLocalTime(tm& tm);
                void setUtcTime(tm& tm);
                const std::string logPrefix();
            public:
                void setTimeProvider(TimeProvider* timeProvider);
                tm getLocalTime();
                tm getUtcTime();
                bool isTimeValid();
                
        };
    }
}
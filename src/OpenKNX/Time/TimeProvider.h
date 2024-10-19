#pragma once
#include "OpenKNX.h"

namespace OpenKNX
{
    namespace Time
    {
        class TimeProvider
        {
            public:
                virtual void loop() {}
                virtual void processInputKo(GroupObject& groupObject) {}
                void setLocalTime(tm& localTime);
                void setUtcTime(tm& utcTime);
        };
    } // namespace Time
} // namespace OpenKNX
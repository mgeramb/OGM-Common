#pragma once
#include "TimeProvider.h"

namespace OpenKNX
{
    namespace Time
    {
        class TimeProviderKnx : public TimeProvider
        {
                bool _configured = false;
                bool _hasDate = false;
                bool _hasTime = false;
                unsigned long _timeStampTimeReceived = 0;
                bool _hasSummertimeFlag = false;
                tm _dateTime = { 0 };
                void checkHasAllDateTimeParts();
                bool isSummerTimeActive(int year, int month, int day, int hour, int minute);
            public:
                void loop() override;
                void processInputKo(GroupObject& ko);
        };
    } // namespace Time
} // namespace OpenKNX
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
            tm _dateTime = {0};
            unsigned long _readRequestTimerStart = 0;
            void checkHasAllDateTimeParts();
            bool isSummerTimeActive(int year, int month, int day, int hour, int minute);

          public:
            /*
             * called by the framework after the knx configuration was loaded
             */
            void setup() override;
            /*
            * called by the framework in the loop for core0
            */
            void loop() override;
            /*
             * Called on incoming/changing GroupObject
             * @param GroupObject
             */
            void processInputKo(GroupObject& ko) override;
        };
    } // namespace Time
} // namespace OpenKNX
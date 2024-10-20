#pragma once
#include "Arduino.h"
#include "string"
#include "time.h"

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
            bool _setupCalled = false;
            unsigned long _timeCacheTimeStamp = 0;
            tm _utcTime = {0};
            tm _localTime{0};
            bool _isTimeValid = false;
            inline void checkCacheAge()
            {
                if (_timeCacheTimeStamp == 0 || millis() - _timeCacheTimeStamp > 50)
                {
                    updateDateTimeCache();
                }
            }
            void setup();
            void loop();
            void processInputKo(GroupObject& ko);
            bool processCommand(std::string& cmd, bool diagnoseKo);
            void setLocalTime(tm& tm);
            void setUtcTime(tm& tm);
            const std::string logPrefix();
            void updateDateTimeCache();

          public:
            /*
             * set a time provider, a previous set time provider will be deleted
             */
            void setTimeProvider(TimeProvider* timeProvider);
            /*
             * returns the local time
             */
            tm& getLocalTime()
            {
                checkCacheAge();
                return _localTime;
            }
            /*
             * returns the UTC time
             */
            tm& getUtcTime()
            {
                checkCacheAge();
                return _utcTime;
            }
            /*
             * returns true, if the time was a least one time set
             */
            inline bool isTimeValid()
            {
                checkCacheAge();
                return _isTimeValid;
            }

            static tm convertUtcToLocalTime(tm& utcTime);
        };
    } // namespace Time
} // namespace OpenKNX
#pragma once
#include "Arduino.h"
#include "string"
#include "time.h"
#include "chrono"

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
           
            void setup();
            void loop();
            void processInputKo(GroupObject& ko);
            bool processCommand(std::string& cmd, bool diagnoseKo);
            void setLocalTime(tm& tm, unsigned long miilisReceivedTimestamp);
            void setUtcTime(tm& tm, unsigned long miilisReceivedTimestamp);
            void setTime(std::time_t epoch, timezone* tz, unsigned long miilisReceivedTimestamp);
            const std::string logPrefix();
          
          public:
            /*
             * set a time provider, a previous set time provider will be deleted
             */
            void setTimeProvider(TimeProvider* timeProvider);
            /*
             * returns the local time
             */
            tm getLocalTime();
            /*
             * returns the UTC time
             */
            tm getUtcTime();
            /*
             * returns true, if the time was a least one time set
             */
            bool isTimeValid();
           
            /*
             * Converts a UTC time to local time
             */
            static tm convertUtcToLocalTime(tm& utcTime);
            /*
             * Converts a local time to UTC time
             */
            static tm convertLocalTimeToUtc(tm& tmLocalTime);
            /*
             * Returns for the provided local time
             * 1 if it is in summertime
             * 0 if it is in wintertime
             * -1 for the switching hour in the auntum which can be summer or winter time
             */
            static int isSummerTime(int year, int month, int day, int hour, int minute);
     
        };
    } // namespace Time
} // namespace OpenKNX
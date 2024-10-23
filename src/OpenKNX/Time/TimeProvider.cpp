#include "TimeProvider.h"


namespace OpenKNX
{
    namespace Time
    {
        void TimeProvider::setLocalTime(tm& localTime, unsigned long millisReceivedTimestamp)
        {
            openknx.time.setLocalTime(localTime, millisReceivedTimestamp);
        }

        void TimeProvider::setUtcTime(tm& utcTime, unsigned long millisReceivedTimestamp)
        {
            openknx.time.setUtcTime(utcTime, millisReceivedTimestamp);
        }
    }
}
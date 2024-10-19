#include "TimeProvider.h"


namespace OpenKNX
{
    namespace Time
    {
        void TimeProvider::setLocalTime(tm& localTime)
        {
            openknx.time.setLocalTime(localTime);
        }

        void TimeProvider::setUtcTime(tm& utcTime)
        {
            openknx.time.setUtcTime(utcTime);
        }
    }
}
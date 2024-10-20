#pragma once
#include "Arduino.h"
#include "string"

namespace OpenKNX
{
    class Common;
    class Console;

    namespace Sun
    {
        class SunCalculation
        {
            friend Common;
            friend Console;
            uint8_t _lastHour = 0;
            uint8_t _lasMinute = 0;
            bool _sunCalculationValid = false;
            float _azimut = 0;
            float _elevation = 0;
            const std::string logPrefix();
            void setup();
            void loop();
            bool processCommand(std::string& cmd, bool diagnoseKo);
            void recalculateSunCalculation();
            tm _sunRiseUtc;
            tm _sunSetUtc;
            tm _sunRiseLocalTime;
            tm _sunSetLocalTime;

          public:
            /*
             * Returns true, if the sun position is calculated which requires a valid time
             */
            inline bool isSubPositionValid() { return _sunCalculationValid; }
            /*
             * Returns the azimut
             */
            inline float azimut() { return _azimut; }
            /*
             * Returns the elevation
             */
            inline float elevation() { return _elevation; }
        };
    } // namespace Sun
} // namespace OpenKNX
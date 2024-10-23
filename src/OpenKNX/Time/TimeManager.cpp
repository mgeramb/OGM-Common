#include "TimeManager.h"
#include "../Log/Logger.h"
#include "TimeProvider.h"

namespace OpenKNX
{
    namespace Time
    {
        const std::string TimeManager::logPrefix()
        {
            return std::string("Time");
        }
        bool TimeManager::processCommand(std::string& cmd, bool diagnoseKo)
        {
            if (cmd.rfind("tm") == 0)
            {
                if (cmd == "tm ?")
                {
                    openknx.console.printHelpLine("tm", "Show current local time");
                    openknx.console.printHelpLine("tm u", "Show current UTC time");
                    openknx.console.printHelpLine("tm tz", "Show current posix timezone string");
                    openknx.console.printHelpLine("tm x", "Set time to 2024-07-01 15:00 UTC (for testing)");
                    openknx.console.printHelpLine("tm y", "Set time to 2024-12-01 15:00 UTC (for testing)");
                    openknx.console.printHelpLine("tm hhmm", "Set time to hh:mm UTC (for testing)");
                    openknx.console.printHelpLine("tm YYMMDDhhmm", "Set time to 20YY-MM-DD hh:mm UTC (for testing)");
                    return true;
                }
                if (cmd == "tm")
                {
                    if (openknx.time.isTimeValid())
                    {
                        auto time = getLocalTime();
                        logInfoP("%04d-%02d-%02d %02d:%02d:%02d (%s)", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_min, (int)time.tm_sec, time.tm_isdst ? "Summertime" : "Wintertime");
                    }
                    else
                        logInfoP("No valid time");
                    return true;
                }
                if (cmd == "tm u")
                {
                    if (openknx.time.isTimeValid())
                    {
                        auto time = getUtcTime();
                        logInfoP("%04d-%02d-%02d %02d:%02d:%02d UTC", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_min, (int)time.tm_sec);
                    }
                    else
                        logInfoP("No valid time");
                    return true;
                }
                if (cmd == "tm tz")
                {
                    auto tz = getenv("TZ");
                    if (tz == nullptr)
                        logInfoP("No timezone set");
                    else
                        logInfoP(tz);
                    return true;
                }
                if (cmd.rfind("tm ") == 0)
                {
                    logInfoP("Set date/time");
                    tm tm = {0};
                    if (cmd == "tm x")
                    {
                        tm.tm_year = 2024 - 1900;
                        tm.tm_mon = 7 - 1;
                        tm.tm_mday = 1;
                        tm.tm_hour = 15;
                        tm.tm_min = 0;
                    }
                    else if (cmd == "tm y")
                    {
                        tm.tm_year = 2024 - 1900;
                        tm.tm_mon = 12 - 1;
                        tm.tm_mday = 1;
                        tm.tm_hour = 15;
                        tm.tm_min = 0;
                    }
                    else if (cmd.length() == 7)
                    {
                        tm.tm_year = isTimeValid() ? getLocalTime().tm_year : 2024 - 1900;
                        tm.tm_mon = isTimeValid() ? getLocalTime().tm_mon : 7 - 1;
                        tm.tm_mday = isTimeValid() ? getLocalTime().tm_mday : 1;
                        tm.tm_hour = stoi(cmd.substr(3, 2));
                        tm.tm_min = stoi(cmd.substr(5, 2));
                    }
                    else if (cmd.length() == 13)
                    {
                        tm.tm_year = stoi(cmd.substr(3, 2)) + 2000 - 1900;
                        tm.tm_mon = stoi(cmd.substr(5, 2)) - 1;
                        tm.tm_mday = stoi(cmd.substr(7, 2)) - 1;
                        tm.tm_hour = stoi(cmd.substr(9, 2));
                        tm.tm_min = stoi(cmd.substr(11, 2));
                    }
                    else
                    {
                        logInfoP("Invalid time format");
                        return true;
                    }
                    logInfoP("%04d-%02d-%02d %02d:%02d UTC", (int)tm.tm_year + 1900, (int)tm.tm_mon + 1, (int)tm.tm_mday, (int)tm.tm_hour, (int)tm.tm_min);
                    setUtcTime(tm, millis());
                    return true;
                }
            }
            return false;
        }
        void TimeManager::setTimeProvider(TimeProvider* timeProvider)
        {
            if (_timeProvider != nullptr)
                delete _timeProvider;
            _timeProvider = timeProvider;
            if (_setupCalled && _timeProvider != nullptr)
                _timeProvider->setup();
        }

        void TimeManager::setup()
        {
            // <Enumeration Text="Midway-Inseln (-11 Stunden)" Value="27" Id="%ENID%" />
            // <Enumeration Text="Honolulu (-10 Stunden)" Value="26" Id="%ENID%" />
            // <Enumeration Text="Anchorage (-9 Stunden)" Value="25" Id="%ENID%" />
            // <Enumeration Text="Los Angeles (-8 Stunden)" Value="24" Id="%ENID%" />
            // <Enumeration Text="Denver (-7 Stunden)" Value="23" Id="%ENID%" />
            // <Enumeration Text="Chicago (-6 Stunden)" Value="22" Id="%ENID%" />
            // <Enumeration Text="New York (-5 Stunden)" Value="21" Id="%ENID%" />
            // <Enumeration Text="Santo Domingo (-4 Stunden)" Value="20" Id="%ENID%" />
            // <Enumeration Text="Rio de Janeiro (-3 Stunden)" Value="19" Id="%ENID%" />
            // <Enumeration Text="(-2 Stunden)" Value="18" Id="%ENID%" />
            // <Enumeration Text="Azoren (-1 Stunde)" Value="17" Id="%ENID%" />
            // <Enumeration Text="London (+0 Stunden)" Value="0" Id="%ENID%" />
            // <Enumeration Text="Berlin (+1 Stunde)" Value="1" Id="%ENID%" />
            // <Enumeration Text="Athen (+3 Stunden)" Value="2" Id="%ENID%" />
            // <Enumeration Text="Moskau (+4 Stunden)" Value="3" Id="%ENID%" />
            // <Enumeration Text="Dubai (+5 Stunden)" Value="4" Id="%ENID%" />
            // <Enumeration Text="Karatschi (+6 Stunden)" Value="5" Id="%ENID%" />
            // <Enumeration Text="Dhaka (+7 Stunden)" Value="6" Id="%ENID%" />
            // <Enumeration Text="Bangkok (+8 Stunden)" Value="7" Id="%ENID%" />
            // <Enumeration Text="Peking (+9 Stunden)" Value="8" Id="%ENID%" />
            // <Enumeration Text="Tokio (+10 Stunden)" Value="9" Id="%ENID%" />
            // <Enumeration Text="Sydney (+11 Stunden)" Value="10" Id="%ENID%" />
            // <Enumeration Text="Nouméa (+12 Stunden)" Value="11" Id="%ENID%" />
            // <Enumeration Text="Wellington (+12 Stunden)" Value="12" Id="%ENID%" />

            const char* timezoneString = "CET-1CEST,M3.5.0/02,M10.5.0/03"; // UTC
            switch (ParamBASE_TimezoneValue)
            {
                case 27:
                    timezoneString = "NUT11"; // America Samoa
                    break;
                case 26:
                    timezoneString = "HST11HDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Hawai
                    break;
                case 25:
                    timezoneString = "ASKT9AKDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Alaska
                    break;
                case 24:
                    timezoneString = "PST8PDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Los Angeles
                    break;
                case 23:
                    timezoneString = "MST7MDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Denver
                    break;
                case 22:
                    timezoneString = "CST6CDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Chicago
                    break;
                case 21:
                    timezoneString = "EST5EDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // New York
                    break;
                case 20:
                    timezoneString = "GMT-4"; // GMT-4
                    break;
                case 19:
                    timezoneString = "ART3"; // Argentina
                    break;
                case 18:
                    timezoneString = "WGST3WGT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Greenland
                    break;
                case 17:
                    timezoneString = "CVT1"; // Cabo Verde
                    break;
                case 0:
                    timezoneString = "BST0GMT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // UK
                    break;
                case 1:
                    timezoneString = "CEST-1CET,M3.5.0/2:00:00,M10.5.0/3:00:00"; // Germany
                    break;
                case 2:
                    timezoneString = "EET-2EEST,M3.5.0/3,M10.5.0/4"; // Athen
                    break;
                case 3:
                    timezoneString = "MSK-3MSD,M3.5.0,M10.5.0/3"; // Moscow
                    break;
                case 4:
                    timezoneString = "UZT-4"; // Azerbaijan
                    break;
                case 5:
                    timezoneString = "UZT-5"; // Pakistan
                    break;
                case 6:
                    timezoneString = "BDT-6"; // Bangladesh
                    break;
                case 7:
                    timezoneString = "WIB-7"; // Indonesia
                    break;
                case 8:
                    timezoneString = "CST-8"; // China
                    break;
                case 9:
                    timezoneString = "JST-9"; // Japan
                    break;
                case 10:
                    timezoneString = "AEST-9AEDT,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Eastern Australia
                    break;
                case 11:
                    timezoneString = "SBT-11"; // Solomon Islands
                    break;
                case 12:
                    timezoneString = "ANAT-12"; // New Zealand
                    break;
            }
            logDebugP("Using timezone:");
            logDebugP(timezoneString);
            setenv("TZ", timezoneString, 1);
            if (_timeProvider != nullptr)
                _timeProvider->setup();
            _setupCalled = true;
        }

        void TimeManager::loop()
        {
            if (_timeProvider != nullptr)
                _timeProvider->loop();
        }

        bool TimeManager::isTimeValid()
        {
            time_t now;
            time(&now);
            return now > 1704070800; // 2024-01-01

        }

        tm TimeManager::getLocalTime()
        {
            time_t now;
            time(&now);
            tm localTime;
            localtime_r(&now, &localTime);
            return localTime;
        }
         
        tm TimeManager::getUtcTime()
        {
            time_t now;
            time(&now);
            tm utcTime;
            gmtime_r(&now, &utcTime);
            return utcTime;
        }

        void TimeManager::processInputKo(GroupObject& ko)
        {
            if (_timeProvider != nullptr)
                _timeProvider->processInputKo(ko);
        }

        void TimeManager::setLocalTime(tm& tm, unsigned long millisReceivedTimestamp)
        {
            std::time_t epoch = mktime(&tm);
            setTime(epoch, nullptr, millisReceivedTimestamp);
        }

        void TimeManager::setUtcTime(tm& tmx, unsigned long millisReceivedTimestamp)
        {
            timezone timezoneUtc{0};
            std::time_t epoch = mktime(&tmx) - _timezone;
            setTime(epoch, &timezoneUtc, millisReceivedTimestamp);

            tm tm1 = {0};
            tm1.tm_hour = 15;
            tm1.tm_mon = 7;
            tm1.tm_mday = 0;
            tm1.tm_year = 2024 - 1900;

            auto time = convertUtcToLocalTime(tm1);
            logDebugP("%04d-%02d-%02d %02d:%02d:%02d (%s) converted", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_sec, (int)time.tm_min, time.tm_isdst ? "Summertime" : "Wintertime");

            time = convertLocalTimeToUtc(time);
            logDebugP("%04d-%02d-%02d %02d:%02d:%02d (UTC) converted", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_sec, (int)time.tm_min);

            tm tm2 = {0};
            tm2.tm_hour = 15;
            tm2.tm_mon = 11;
            tm2.tm_mday = 0;
            tm2.tm_year = 2024 - 1900;

            time = convertUtcToLocalTime(tm2);
            logDebugP("%04d-%02d-%02d %02d:%02d:%02d (%s) converted", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_sec, (int)time.tm_min, time.tm_isdst ? "Summertime" : "Wintertime");

            time = convertLocalTimeToUtc(time);
            logDebugP("%04d-%02d-%02d %02d:%02d:%02d (UTC) converted", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_sec, (int)time.tm_min);

            logDebugP("29.3.2024 23:59 %d 0", (int)isSummerTime(2024, 3, 29, 23, 59));
            logDebugP("30.3.2024 00:00 %d 0", (int)isSummerTime(2024, 3, 30, 0, 0));

            logDebugP("30.3.2024 01:59 %d 0", (int)isSummerTime(2024, 3, 30, 1, 59));

            logDebugP("31.3.2024 01:59 %d 0", (int)isSummerTime(2024, 3, 31, 1, 59));
            logDebugP("31.3.2024 02:00 %d X", (int)isSummerTime(2024, 3, 31, 2, 00));
            logDebugP("31.3.2024 02:01 %d X", (int)isSummerTime(2024, 3, 31, 2, 01));

            logDebugP("31.3.2024 02:59 %d X", (int)isSummerTime(2024, 3, 31, 2, 59));
            logDebugP("31.3.2024 03:00 %d 1", (int)isSummerTime(2024, 3, 31, 3, 00));
            logDebugP("31.3.2024 03:01 %d 1", (int)isSummerTime(2024, 3, 31, 4, 01));

            logDebugP("27.10.2024 01:59 %d 1", (int)isSummerTime(2024, 10, 27, 1, 59));
            logDebugP("27.10.2024 02:00 %d ?", (int)isSummerTime(2024, 10, 27, 2, 00));
            logDebugP("27.10.2024 02:01 %d ?", (int)isSummerTime(2024, 10, 27, 2, 01));

            logDebugP("27.10.2024 02:59 %d ?", (int)isSummerTime(2024, 10, 27, 2, 59));
            logDebugP("27.10.2024 03:00 %d 0", (int)isSummerTime(2024, 10, 27, 3, 00));
            logDebugP("27.10.2024 03:01 %d 0", (int)isSummerTime(2024, 10, 27, 3, 01));
            logDebugP("27.10.2024 23:59 %d 0", (int)isSummerTime(2024, 10, 27, 23, 59));
            logDebugP("28.10.2024 00:00 %d 0", (int)isSummerTime(2024, 10, 28, 00, 00));
            logDebugP("29.10.2024 00:00 %d 0", (int)isSummerTime(2024, 10, 29, 00, 00));
            logDebugP("03.11.2024 00:00 %d 0", (int)isSummerTime(2024, 11, 3, 00, 00));
            logDebugP("04.11.2024 00:00 %d 0", (int)isSummerTime(2024, 11, 4, 00, 00));
        }

        void TimeManager::setTime(std::time_t epoch, timezone* tz, unsigned long millisReceivedTimestamp)
        {
            auto now = millis();
            auto millisOffset = now - millisReceivedTimestamp;
            auto seconds = (long)millisOffset / 1000;
            auto milliseconds = (long)millisOffset % 1000;
            struct timeval tv;
            tv.tv_sec = epoch + seconds;
            tv.tv_usec = milliseconds * 1000;
            settimeofday(&tv, tz);
            getLocalTime(); // call to update _isTimeValid

            // DEGUB CODE BEGIN
            auto time = getLocalTime();
            logDebugP("%04d-%02d-%02d %02d:%02d:%02d (%s)", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_min, (int)time.tm_sec, time.tm_isdst ? "Summertime" : "Wintertime");
            time = getUtcTime();
            logDebugP("%04d-%02d-%02d %02d:%02d:%02d UTC", (int)time.tm_year + 1900, (int)time.tm_mon + 1, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_min, (int)time.tm_sec);
        }

        int TimeManager::isSummerTime(int year, int month, int day, int hour, int minute)
        {
            tm timeinfo = {0};
            timeinfo.tm_year = year - 1900; // Year since 1900
            timeinfo.tm_mon = month - 1;    // Month (0-based, January is 0)
            timeinfo.tm_mday = day;         // Day of the month (1-31)
            timeinfo.tm_hour = hour;
            timeinfo.tm_min = minute;

            // Convert the date to time_t
            time_t rawtime = mktime(&timeinfo);
            // Convert back to local time and check DST
            tm localTimeinfo;
            localtime_r(&rawtime, &localTimeinfo);

            // check one hour before
            timeinfo.tm_sec -= 3600;
            // Convert the date to time_t
            time_t rawtimeMinusOneHour = mktime(&timeinfo);
            // Convert back to local time and check DST
            tm localTimeinfoMinusOnHour;
            localtime_r(&rawtimeMinusOneHour, &localTimeinfoMinusOnHour);
            if (localTimeinfo.tm_isdst != localTimeinfoMinusOnHour.tm_isdst)
            {
                return -1;
            }
            // Check if DST is active (tm_isdst == 1 means DST is active)
            return localTimeinfo.tm_isdst > 0;
        }

        tm TimeManager::convertUtcToLocalTime(tm& tmUtc)
        {
            auto utc = mktime(&tmUtc) - _timezone;
            tm localTime;
            localtime_r(&utc, &localTime);
            return localTime;
        }

        tm TimeManager::convertLocalTimeToUtc(tm& tmLocalTime)
        {
            auto localTime = mktime(&tmLocalTime);
            tm utc;
            gmtime_r(&localTime, &utc);
            return utc;
        }

    } // namespace Time
} // namespace OpenKNX
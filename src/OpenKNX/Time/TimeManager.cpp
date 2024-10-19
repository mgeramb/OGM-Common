#include "TimeManager.h"
#include "../Log/Logger.h"
#include "TimeProvider.h"
#include "chrono"
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
                    openknx.console.printHelpLine("tm hhmm", "Set time to hh:mm UTC (for testing)");
                    openknx.console.printHelpLine("tm YYMMDDhhmm", "Set time to 20YY-MM-DD hh:mm UTC (for testing)");        
                    return true;
                }
                if (cmd == "tm")
                {
                    if (openknx.time.isTimeValid())
                    {
                        auto time = getLocalTime();
                        logInfoP("%04d-%02d-%02d %02d:%02d", (int)time.tm_year, (int)time.tm_mon, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_min);
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
                        logInfoP("%04d-%02d-%02d %02d:%02d UTC", (int)time.tm_year, (int)time.tm_mon, (int)time.tm_mday, (int)time.tm_hour, (int)time.tm_min);
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
                        tm.tm_year = 2024;
                        tm.tm_mon = 7;
                        tm.tm_mday = 1;
                        tm.tm_hour = 15;
                        tm.tm_min = 0;
                    }
                    else if (cmd.length() == 7)
                    {
                        tm.tm_year = isTimeValid() ? getLocalTime().tm_year : 2024;
                        tm.tm_mon = isTimeValid() ? getLocalTime().tm_mon : 7;
                        tm.tm_mday = isTimeValid() ? getLocalTime().tm_mday : 1;
                        tm.tm_hour = stoi(cmd.substr(3, 2));
                        tm.tm_min = stoi(cmd.substr(5, 2));
                    }
                    else if (cmd.length() == 13)
                    {
                        tm.tm_year = stoi(cmd.substr(3, 2)) + 2000;
                        tm.tm_mon = stoi(cmd.substr(5, 2));
                        tm.tm_mday = stoi(cmd.substr(7, 2));
                        tm.tm_hour = stoi(cmd.substr(9, 2));
                        tm.tm_min = stoi(cmd.substr(11, 2));
                    }
                    else
                    {
                        logInfoP("Invalid time format");
                        return true;
                    }
                    logInfoP("%04d-%02d-%02d %02d:%02d UTC", (int)tm.tm_year, (int)tm.tm_mon, (int)tm.tm_mday, (int)tm.tm_hour, (int)tm.tm_min);
                    setUtcTime(tm);
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
            // <Enumeration Text="London (+1 Stunden)" Value="0" Id="%ENID%" />
            // <Enumeration Text="Berlin (+2 Stunde)" Value="1" Id="%ENID%" />
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
                    timezoneString = "CEST-1CET,M3.2.0/2:00:00,M11.1.0/2:00:00"; // Germany
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
        }

        void TimeManager::loop()
        {
            if (_timeProvider != nullptr)
                _timeProvider->loop();
        }

        void TimeManager::processInputKo(GroupObject& ko)
        {
            if (_timeProvider != nullptr)
                _timeProvider->processInputKo(ko);
        }

        void TimeManager::setLocalTime(tm& tm)
        {
            std::time_t epoch = mktime(&tm);
            struct timeval tv;
            tv.tv_sec = epoch;
            tv.tv_usec = 0;

            settimeofday(&tv, NULL);
        }

        void TimeManager::setUtcTime(tm& tm)
        {
            std::time_t epoch = mktime(&tm);
            struct timeval tv;
            tv.tv_sec = epoch;
            tv.tv_usec = 0;

            timezone timezoneUtc{0};
            settimeofday(&tv, &timezoneUtc);
        }

        bool TimeManager::isTimeValid()
        {
            auto utcTime = getUtcTime();
            if (utcTime.tm_year < 2024)
                return false;
            return true;
        }

        tm TimeManager::getLocalTime()
        {
            time_t now;
            time(&now);
            return *localtime(&now);
        }

        tm TimeManager::getUtcTime()
        {
            time_t now;
            time(&now);
            return *gmtime(&now);
        }

    } // namespace Time
} // namespace OpenKNX
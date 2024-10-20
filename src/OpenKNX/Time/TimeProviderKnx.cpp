#include "TimeProviderKnx.h"
#include "OpenKNX.h"

// DPT19 special flags
#ifndef DPT19_FAULT
    #define DPT19_FAULT 0x80
#endif
#ifndef DPT19_WORKING_DAY
    #define DPT19_WORKING_DAY 0x40
#endif
#ifndef DPT19_NO_WORKING_DAY
    #define DPT19_NO_WORKING_DAY 0x20
#endif
#ifndef DPT19_NO_YEAR
    #define DPT19_NO_YEAR 0x10
#endif
#ifndef DPT19_NO_DATE
    #define DPT19_NO_DATE 0x08
#endif
#ifndef DPT19_NO_DAY_OF_WEEK
    #define DPT19_NO_DAY_OF_WEEK 0x04
#endif
#ifndef DPT19_NO_TIME
    #define DPT19_NO_TIME 0x02
#endif
#ifndef DPT19_SUMMERTIME
    #define DPT19_SUMMERTIME 0x01
#endif

#ifndef VAL_STIM_FROM_DPT19
    #define VAL_STIM_FROM_DPT19 1
#endif

namespace OpenKNX
{
    namespace Time
    {
        void TimeProviderKnx::setup()
        {
#ifdef BASE_KoTime
            if (KoBASE_Time.initialized())
                processInputKo(KoBASE_Time);
            if (!ParamBASE_CombinedTimeDate && KoBASE_Date.initialized())
                processInputKo(KoBASE_Date);
            // <Enumeration Text="Kommunikationsobjekt 'Sommerzeit aktiv'" Value="0" Id="%ENID%" />
            // <Enumeration Text="Kombiniertes Datum/Zeit-KO (DPT 19)" Value="1" Id="%ENID%" />
            // <Enumeration Text="Interne Berechnung (nur in Deutschland)" Value="2" Id="%ENID%" />
            if (ParamBASE_SummertimeAll == 0 && KoBASE_IsSummertime.initialized())
                processInputKo(KoBASE_IsSummertime);

            if (!_hasDate || !_hasTime || !_hasSummertimeFlag)
                _readRequestTimerStart = max(millis(), 1UL); // start read request timer
#endif
        }

        void TimeProviderKnx::loop()
        {
#ifdef BASE_KoTime
            if (_readRequestTimerStart != 0 && ParamBASE_ReadTimeDate)
            {
                if (millis() - _readRequestTimerStart > 30000)
                {
                    _readRequestTimerStart = max(millis(), 1UL);
                    if (ParamBASE_CombinedTimeDate)
                    {
                        // combined date and time
                        if (!_hasTime)
                            KoBASE_Time.requestObjectRead();
                    }
                    else
                    {
                        // date and time from separate KOs
                        if (!_hasTime)
                            KoBASE_Time.requestObjectRead();
                        if (!_hasDate)
                            KoBASE_Date.requestObjectRead();
                    }
                    if (!_hasSummertimeFlag && ParamBASE_SummertimeAll == 0)
                    {
                        KoBASE_IsSummertime.requestObjectRead();
                    }
                }
            }
#endif
        }

        void TimeProviderKnx::processInputKo(GroupObject &ko)
        {
#ifdef BASE_KoTime
            if (ko.asap() == BASE_KoTime)
            {
                if (ParamBASE_CombinedTimeDate)
                {
                    KNXValue value = "";
                    // first ensure we have a valid data-time content
                    // (including the correct length)
                    if (ko.tryValue(value, DPT_DateTime))
                    {

                        // use raw value, as current version of knx do not provide access to all fields
                        // TODO DPT19: check integration of extended DPT19 access into knx or OpenKNX-Commons
                        // size is ensured to be 8 Byte
                        uint8_t *raw = ko.valueRef();

                        /*
                        const bool flagFault = raw[6] & 0x80;
                        // ignore working day (WD, NWD): raw[6] & 0x40, raw[6] & 0x20
                        const bool flagNoYear = raw[6] & 0x10;
                        const bool flagNoDate = raw[6] & 0x08;
                        // ignore NDOW: raw[6] & 0x04
                        const bool flagNoTime = raw[6] & 0x02;
                        const bool flagSuti = raw[6] & 0x01;
                        // ignore quality of clock (CLQ): raw[7] & 0x80
                        // ignore synchronisation source reliablity (SRC): raw[7] & 0x40
                        */

                        // ignore inputs with:
                        // * F - fault
                        // * NY - missing year
                        // * ND - missing date
                        // * NT - missing time
                        if (!(raw[6] & (DPT19_FAULT | DPT19_NO_YEAR | DPT19_NO_DATE | DPT19_NO_TIME)))
                        {
                            _timeStampTimeReceived = millis();
                            struct tm lTmp = value;
                            _dateTime.tm_year = lTmp.tm_year;
                            _dateTime.tm_mon = lTmp.tm_mon;
                            _dateTime.tm_mday = lTmp.tm_mday;
                            _hasDate = true;
                            _dateTime.tm_hour = lTmp.tm_hour;
                            _dateTime.tm_min = lTmp.tm_min;
                            _dateTime.tm_sec = lTmp.tm_sec;
                            _hasTime = true;

                            const bool lSummertime = raw[6] & DPT19_SUMMERTIME;
                            // <Enumeration Text="Kommunikationsobjekt 'Sommerzeit aktiv'" Value="0" Id="%ENID%" />
                            // <Enumeration Text="Kombiniertes Datum/Zeit-KO (DPT 19)" Value="1" Id="%ENID%" />
                            // <Enumeration Text="Interne Berechnung (nur in Deutschland)" Value="2" Id="%ENID%" />
                            if (ParamBASE_SummertimeAll == 1)
                            {
                                _dateTime.tm_isdst = lSummertime;
                                _hasSummertimeFlag = true;
                            }
                            else if (!_hasSummertimeFlag && openknx.time.isTimeValid())
                            {
                                // We have a valid time, use the current summertime flag
                                _dateTime.tm_isdst = openknx.time.getLocalTime().tm_isdst;
                                _hasSummertimeFlag = true;
                            }
                            checkHasAllDateTimeParts();
                        }
                    }
                }
                else
                {
                    KNXValue value = "";
                    // ensure we have a valid time content
                    if (ko.tryValue(value, DPT_TimeOfDay))
                    {
                        _timeStampTimeReceived = millis();
                        struct tm lTmp = value;
                        _dateTime.tm_hour = lTmp.tm_hour;
                        _dateTime.tm_min = lTmp.tm_min;
                        _dateTime.tm_sec = lTmp.tm_sec;
                        _hasTime = true;
                        if (openknx.time.isTimeValid())
                        {
                            // time is already valid, use current date
                            auto now = openknx.time.getLocalTime();
                            if (lTmp.tm_hour == 0 && now.tm_hour == 23)
                            {
                                // New day started, correct date to use it
                                now.tm_mday += 1;
                                mktime(&now); // normalize
                            }
                            _dateTime.tm_year = now.tm_year;
                            _dateTime.tm_mon = now.tm_mon;
                            _dateTime.tm_mday = now.tm_mday;
                            _hasDate = true;
                            if (!_hasSummertimeFlag)
                            {
                                // use the current summertime flag
                                _dateTime.tm_isdst = now.tm_isdst;
                                _hasSummertimeFlag = true;
                            }
                        }
                        checkHasAllDateTimeParts();
                    }
                }
            }
            else if (ko.asap() == BASE_KoDate)
            {
                KNXValue value = "";
                // ensure we have a valid date content
                if (ko.tryValue(value, DPT_Date))
                {
                    struct tm lTmp = value;
                    _dateTime.tm_year = lTmp.tm_year;
                    _dateTime.tm_mon = lTmp.tm_mon;
                    _dateTime.tm_mday = lTmp.tm_mday;
                    _hasDate = true;
                    if (openknx.time.isTimeValid())
                    {
                        auto now = openknx.time.getLocalTime();
                        if (now.tm_year == _dateTime.tm_year && now.tm_mon == _dateTime.tm_mon && now.tm_mday == _dateTime.tm_mday)
                        {
                            // day not changed, wait for receiving time
                        }
                        else
                        {
                            now.tm_mday += 1;
                            mktime(&now);
                            if (now.tm_year == _dateTime.tm_year && now.tm_mon == _dateTime.tm_mon && now.tm_mday == _dateTime.tm_mday && (now.tm_hour == 23 || now.tm_hour == 0))
                            {
                                // Next day has start, correct the time
                                _dateTime.tm_hour = 0;
                                _dateTime.tm_min = 0;
                                _dateTime.tm_sec = 0;
                                _timeStampTimeReceived = millis();
                                _hasTime = true;
                                if (!_hasSummertimeFlag)
                                {
                                    _dateTime.tm_isdst = now.tm_isdst;
                                    _hasSummertimeFlag = true;
                                }
                            }
                        }
                    }
                }
            }
            else if (ko.asap() == BASE_KoIsSummertime)
            {
                // <Enumeration Text="Kommunikationsobjekt 'Sommerzeit aktiv'" Value="0" Id="%ENID%" />
                // <Enumeration Text="Kombiniertes Datum/Zeit-KO (DPT 19)" Value="1" Id="%ENID%" />
                // <Enumeration Text="Interne Berechnung (nur in Deutschland)" Value="2" Id="%ENID%" />
                if (ParamBASE_SummertimeAll == 0)
                {
                    _dateTime.tm_isdst = ko.value(DPT_Switch);
                    _hasSummertimeFlag = true;            
                    checkHasAllDateTimeParts();
                }
            }
#endif
        }

        void TimeProviderKnx::checkHasAllDateTimeParts()
        {
            // <Enumeration Text="Kommunikationsobjekt 'Sommerzeit aktiv'" Value="0" Id="%ENID%" />
            // <Enumeration Text="Kombiniertes Datum/Zeit-KO (DPT 19)" Value="1" Id="%ENID%" />
            // <Enumeration Text="Interne Berechnung" Value="2" Id="%ENID%" />
            if (ParamBASE_SummertimeAll == 1 && _hasDate && _hasTime)
            {
                bool isActive = isSummerTimeActive(_dateTime.tm_year, _dateTime.tm_mon, _dateTime.tm_mday, _dateTime.tm_hour, _dateTime.tm_min);
                _dateTime.tm_isdst = isActive;
                _hasSummertimeFlag = true;
            }
            if (_hasDate && _hasTime && _hasSummertimeFlag)
            {
                tm tmp = _dateTime;
                tmp.tm_sec += (millis() - _timeStampTimeReceived) / 1000;
                mktime(&tmp); // normalize

                setLocalTime(tmp);
                _readRequestTimerStart = 0;
                _hasDate = false;
                _hasTime = false;
                _hasSummertimeFlag = false;
            }
        }

        bool TimeProviderKnx::isSummerTimeActive(int year, int month, int day, int hour, int minute)
        {
            struct tm timeinfo = {0};
            timeinfo.tm_year = year - 1900; // Year since 1900
            timeinfo.tm_mon = month - 1;    // Month (0-based, January is 0)
            timeinfo.tm_mday = day;         // Day of the month
            timeinfo.tm_hour = hour;
            timeinfo.tm_min = minute;

            // Convert the date to time_t
            time_t rawtime = mktime(&timeinfo);
            if (rawtime == -1)
            {
                return false;
            }

            // Convert back to local time and check DST
            struct tm *local_timeinfo = localtime(&rawtime);
            if (local_timeinfo == nullptr)
            {
                return false;
            }
            // Check if DST is active (tm_isdst == 1 means DST is active)
            return local_timeinfo->tm_isdst > 0;
        }
    } // namespace Time
} // namespace OpenKNX
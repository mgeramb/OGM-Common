OpenKNX Common
===

OpenKNX Common is a library meant to be used once in every OpenKNX Device firmware.

The main functions are:
- setup and calling the knx stack
- setup and calling of the OpenKNX Modules
- User flash handling for persistent data of the OpenKNX Modules

It is written for use with RP2040 (arduino-pico core) or SAMD21 (arduino core).

It is designed for usage with the headerfile generated by openknxproducer, which provides the neccessary defines:
```
MAIN_OpenKnxId
MAIN_ApplicationNumber
MAIN_ApplicationVersion

# optional (delivered by OAM-LogicModule)
LOG_StartupDelayBase
ParamLOG_StartupDelayTimeMS
LOG_HeartbeatDelayBase
KoLOG_Heartbeat
ParamLOG_HeartbeatDelayTimeMS
```

OpenKNX Common is configured by following defines:

```
WATCHDOG
WATCHDOG_MAX_PERIOD_MS
OPENKNX_MAX_MODULES
DEBUG_LOOP_TIME
DEBUG_DELAY

SAVE_INTERRUPT_PIN
INFO_LED_PIN
PROG_BUTTON_PIN
PROG_LED_PIN_ACTIVE_ON
PROG_LED_PIN
KNX_UART_RX_PIN
KNX_UART_TX_PIN
```
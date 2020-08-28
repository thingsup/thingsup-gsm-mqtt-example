# Thingsup : GSM-MQTT-Example

This example demonstrates Thingsup MQTT Service with GSM Modules.

## Installation

1. Install Arduino with ESP32 Board [Refer Here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
2. Install "TinyGSM" and "PubSubClient" libraries to Arduino.
3. Plug in BLYNK_V1.3 board with simcard
4. Change the APN and other account related parameters in code
5. Compile/Download the code in BLYNK board

## Usage

Send Commands from Thingsup MQTT Client to Board.

## Supported GSM Modules

The code is tested with SIM800L and should also work with following modems
SIMCom SIM800 series (SIM800A, SIM800C, SIM800L, SIM800H, SIM808, SIM868)
SIMCom SIM900 series (SIM900A, SIM900D, SIM908, SIM968)
SIMCom WCDMA/HSPA/HSPA+ Modules (SIM5360, SIM5320, SIM5300E, SIM5300E/A)
SIMCom LTE Modules (SIM7100E, SIM7500E, SIM7500A, SIM7600C, SIM7600E)
SIMCom SIM7000E/A/G CAT-M1/NB-IoT Module
Ai-Thinker A6, A6C, A7, A20
ESP8266 (AT commands interface, similar to GSM modems)
Digi XBee WiFi and Cellular (using XBee command mode)
Neoway M590
u-blox 2G, 3G, 4G, and LTE Cat1 Cellular Modems (many modules including LEON-G100, LISA-U2xx, SARA-G3xx, SARA-U2xx, TOBY-L2xx, LARA-R2xx, MPCI-L2xx)
u-blox LTE-M/NB-IoT Modems (SARA-R4xx, SARA-N4xx, but NOT SARA-N2xx)
Sequans Monarch LTE Cat M1/NB1 (VZM20Q)
Quectel BG96
Quectel M95
Quectel MC60 (alpha)

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
This Project uses [TinyGSM](https://github.com/vshymanskyy/TinyGSM) and [PubSubClient](https://github.com/knolleary/pubsubclient.git) libraries.

Read [Getting Started](https://thingsup.io/getting-started/).

[Thingsup](https://thingsup.io) is an Enterprise IoT Platform developed by [iobot technologies](https://www.iobot.in)

## License
MIT

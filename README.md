# mqtt-to-smartevse

This is a small project to interface with the SmartEVSE from Stegen. Disclaimer: I'm very very bad at C so the code is very rudimentary. You can help me clean it up if you are more fluent in C than me.

It was written for an ESP32 with a ttl->RS485 (like the max3485) transceiver on Serial2. It uses the ModbusMaster library and the pubsubclient library.

Pin reference ESP32 -> MAX3485 module:
* ESP32 3.3V -> MAX3485 VCC
* ESP32 GND -> MAX3485 GND
* ESP32 TX2 -> MAX3485 TXD
* ESP32 RX2 -> MAX3485 RXD
* ESP32 GPIO4 -> MAX3485 DE/RE (connect these two pins together with some solder). This is used to switch between receiving and transmitting, RS485 is half duplex.

MAX3485 -> SmartEVSE:
* A -> A
* B -> B

Currently it exposes three functions over mqtt:
* smartevse/start -> start charging (disable locking feature of SmartEVSE), payload does not matter
* smartevse/stop -> stop charging (enable locking feature of SmartEVSE), payload does not matter
* smartevse/current -> set current in amps (6A -> post 6 to this topic, integers only)

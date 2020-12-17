# mqtt-to-smartevse

This is a small project to interface with the SmartEVSE from Stegen. Disclaimer: I'm very very bad at C so the code is very rudimentary. You can help me clean it up if you are more fluent in C than me.

It was written for an ESP32 with a ttl->RS485 (like the max3485) transceiver on Serial2. It uses the ModbusMaster library and the pubsubclient library.

Pin reference ESP32 -> MAX3485 module:
* ESP32 3.3V -> VCC
* ESP32 GND -> GND
* ESP32 TX2 -> TXD
* ESP32 RX2 -> RXD
* ESP32 GPIO4 -> DE/RE (connect these two pins together with some solder)

MAX3485 -> SmartEVSE:
* A -> A
* B -> B

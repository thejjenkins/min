There are three branches:
1. Master
2. AVR-architecture
3. STM-architecture

Master is not used.
AVR-architecture was made to run on the Arduino UNO. It uses altsoftserial library for the protocol (https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html).
STM-architecture was made to run on the Nucleo STM32L476RG. It uses HardwareSerial USART1 for the protocol and USART2 for serial monitor. 

Both codes work the same way, but the STM microcontroller is more powerful and the code runs noticeably faster on it. Most of my time was spent in this branch, so from this point on I will only be referring to it.

To run this code, you will need to search "STM32 MCU based boards" (https://github.com/stm32duino) on the boards manager section of the Arduino IDE.
After that, get a 3V3 or 5V FTDI cable and connect the TX/RX/GND pins to their appropriate counterparts PA10 (RX) and PA9 (TX) on the STM32. 

Verify and upload the sketch and run the threadedMIN.py. The first few messages should show up in the python IDE: "Sending MIN ID = 5... Sequence number sent = ...."
If the Arduino serial monitor is not displaying anything try pushing the reset button.

High-level explanations can be found on Ken Tindell's GitHub: https://github.com/min-protocol/min

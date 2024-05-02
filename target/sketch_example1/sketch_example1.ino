//
// Example program for the STM32L476RG 
//
// Uses Serial2 for programming the board and debug printing, and Serial1 for
// running the MIN protocol. A programming running on a PC in Python is used to
// exercise this code.
//
// The example does the following:
//
// Every 2 seconds the Python program on the host will send a frame and this program
// will respond to each frame ID uniquely. The min.cpp file contains the min_application_handler
// function which handles each frame ID and how to respond. 
//
// See also the Python program that runs on the host.

// This is an easy way to bring the MIN code into an Arduino project. It's better
// to use a Makefile or IDE project file if the application is to be written in C.

#include "min.h"
HardwareSerial Serial1(USART1); // STM32L476RG uses PA10(RX)/PA9(TX) for serial 1.
//HardwareSerial Serial2(USART2); // STM32L476RG uses the micro USB connection as serial 2

// A MIN context (we only have one because we're going to use a single port).
// MIN 2.0 supports multiple contexts, each on a separate port, but in this example
// we will use just SerialUSB.
struct min_context min_ctx;

// This is used to keep track of when the next example message will be sent
uint32_t last_sent;

////////////////////////////////// CALLBACKS ///////////////////////////////////

// Tell MIN how much space there is to write to the serial port. This is used
// inside MIN to decide whether to bother sending a frame or not.
uint16_t min_tx_space(uint8_t port)
{
  // Ignore 'port' because we have just one context. But in a bigger application
  // with multiple ports we could make an array indexed by port to select the serial
  // port we need to use.
  uint16_t n = Serial1.availableForWrite();

  return n;
}

// Send a character on the designated port.
void min_tx_byte(uint8_t port, uint8_t byte)
{
  // Ignore 'port' because we have just one context.
  Serial1.write(&byte, 1U);  
}

// Tell MIN the current time in milliseconds.
uint32_t min_time_ms(void)
{
  return millis();
}

// // Handle the reception of a MIN frame. This is the main interface to MIN for receiving
// // frames. It's called whenever a valid frame has been received (for transport layer frames
// // duplicates will have been eliminated).
// void min_application_handler(struct min_ctx, uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload, uint8_t port)
// {
//   // We ignore the port because we have one context, but we could use it to index an array of
//   // contexts in a bigger application.
//   // This simple example receives the min_id, adds 1 to it, then immediately sends it back as a datagram
//   Serial.println("MIN frame with ID %d", min_id);
//   Serial.println(min_id);
//   Serial.println(" received at %d\n", millis());
//   Serial.println(millis());
//   min_id++;
//   // The frame echoed back doesn't go through the transport protocol: it's send back directly
//   // as a datagram (and could be lost if there were noise on the serial line).
//   min_send_frame(&min_ctx, min_id, min_payload, len_payload);
// }

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200);
  while(!Serial2) {
    ; // Wait for serial port
  }
  Serial1.begin(115200);

  // Initialize the single context. Since we are going to ignore the port value we could
  // use any value. But in a bigger program we would probably use it as an index.
  min_init_context(&min_ctx, Serial1);

  last_sent = millis();
  delay(1000);
}

void loop() {
  char buf[32]; // each char is 1 byte so this is 32 bytes
  size_t buf_len;

  // Read some bytes from the USB serial port..
  if(Serial1.available() > 0) {
    buf_len = Serial1.readBytes(buf, 32U);
    //Serial1.flush();
    //Serial2.println(buf);
  }
  else {
    buf_len = 0;
    //Serial1.flush();
  }
  // .. and push them into MIN. It doesn't matter if the bytes are read in one by
  // one or in a chunk (other than for efficiency) so this can match the way in which
  // serial handling is done (e.g. in some systems the serial port hardware register could
  // be polled and a byte pushed into MIN as it arrives).
  min_poll(&min_ctx, (uint8_t *)buf, (uint8_t)buf_len);

  // Every 1s send a MIN frame using the reliable transport stream.
  uint32_t now = millis();
  // // Use modulo arithmetic so that it will continue to work when the time value wraps
  if (now - last_sent > 1000U) {
    // Send a MIN frame with ID 0x33 (51 in decimal) and with a 4 byte payload of the 
    // the current time in milliseconds. The payload will be in this machine's
    // endianness - i.e. little endian - and so the host code will need to flip the bytes
    // around to decode it. It's a good idea to stick to MIN network ordering (i.e. big
    // endian) for payload words but this would make this example program more complex.
    // if(!min_queue_frame(&min_ctx, 0x33U, (uint8_t *)&now, 4U)) {
    //   // The queue has overflowed for some reason
    //   Serial.print("Can't queue at time ");
    //   Serial.println(millis());
    //   //Serial.println(min_ctx.transport_fifo.dropped_frames);
    //   //min_tx_byte(&altSerial, '1');
    // }
    last_sent = now;
  }
}



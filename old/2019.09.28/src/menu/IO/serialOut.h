/* -*- C++ -*- */
#pragma once
/**
* @file serialOut.h
* @author Rui Azevedo
* @brief Use arduino serial as menu output
*/

#include <menu.h>

// namespace Menu {
#if defined(ARDUINO_attiny)
  //cheating for attin13, just to check memmory requirements
  #include <SoftwareSerial.h>
  constexpr byte rxPin = 3;
  constexpr byte txPin = 4;
  SoftwareSerial Serial(rxPin, txPin);
  // #if defined(ARDUINO) && defined(__AVR_ATtiny13__)
  // uint8_t EIMSK=*(uint8_t*)0x13;
  // uint8_t EIFR=*(uint8_t*)0x14;
  // uint8_t EICRA=*(uint8_t*)0x15;
  // #define PROGMEM
  // #define F(x) x
  // #include <serial-tiny10.h>
  // class TinySerial {
  //   public:
  //     //using 600 baud!
  //     static inline void begin(long int=600) {serial_setup();}
  //     inline void write(uint8_t o) {serial_send(o);}
  //     inline void print(char o) {serial_send(o);}
  //     inline void print(const char* o) {serial_send_string(o);}
  //     inline void println() {write('\n');}
  //     inline void println(char o) {serial_send(o);println();}
  //     inline void println(const char* o) {serial_send_string(o);println();}
  //     inline operator bool() const {return true;}
  //     inline int available() const {return !bit_index;}
  //     inline uint8_t read() {return bit_index?-1:outval;}
  //   protected:
  //     // static bool ok;
  // } Serial;
#endif

  template<typename Dev=decltype(Serial),Dev& dev=Serial,typename O=TextMeasure>
  struct SerialOut:public RawOut<Dev&,dev,O> {
    static inline void nl() {dev.println();}
    template<typename T> static inline void raw(T o) {dev.print(o);}
  };
// }

/* -*- C++ -*- */
#pragma once

#if defined(ARDUINO)&defined(ARDUINO_AVR_ATTINY13)
  #include <uart.h>//https://github.com/lpodkalicki/attiny13-software-uart-library
  struct SwSerial {
     inline void begin(long baud) {}//just ignore speed
     inline constexpr operator bool() {return true;}
     inline int available() const {return last!=-1;}
     inline int read() {return last==-1?uart_getc():last;}
     inline static size_t write(uint8_t byte) {uart_putc(byte);return 1;}
     inline static size_t print(char o) {return write(o);}
     static size_t print(const char* str) {
       size_t cnt=0;
       while(str[0]){write(str[0]);str++;cnt++;}
       return cnt;
     }
     static size_t print(const __FlashStringHelper* str) {
       const char* at=reinterpret_cast<const char*>(str);
       while(pgm_read_byte(at)) {write(pgm_read_byte(at));at++;}
       return ((long)at)-((long)str);
     }
     template<typename T>
     inline static size_t println(T o) {size_t c=print(o);print("\n");return ++c;}
     inline static size_t println() {print("\n");return 1;}
   protected:
     int last=-1;
   };
#endif

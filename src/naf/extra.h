/* -*- C++ -*- */
/*
this are some non-standard function included on AVR stdlib.h (I think)
all because I'm using original arduino files as a quick hack,
they can be replaced for more optimized classes that deal with iostream and string instead
*/
#pragma once

#include <stdio.h>
#include <ctime>

#define CHAR_BITS 8

//unsigned long millis();
inline unsigned long millis() {return time(0)*1000;}

template<typename T>
const char* ntoa_buf(T num,char *buf=NULL, size_t len=0)
{
  snprintf(buf, len, "%d", num);
  return buf;
}

//char * utoa(unsigned int n, char * buffer, int radix);
inline const char* utoa(unsigned int num, char* buf, int radix) {
  sprintf(buf, "%ud", num);
  return buf;
}
inline const char* itoa(unsigned int num, char* buf, int radix) {
  sprintf(buf, "%d", num);
  return buf;
}
inline const char* ltoa(unsigned int num, char* buf, int radix) {
  sprintf(buf, "%d", num);
  return buf;
}
inline const char* ultoa(unsigned int num, char* buf, int radix) {
  sprintf(buf, "%ud", num);
  return buf;
}

inline char * dtostrf(
	double __val,
	signed char __width,
	unsigned char __prec,
	char * __s) {
  sprintf(__s, "%lf", __val);
  return __s;
}

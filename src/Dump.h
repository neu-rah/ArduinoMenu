#include <Arduino.h>
#include <HardwareSerial.h>

unsigned char memByteRam(const void* x);
unsigned char memBytePgm(const void* x);
void dump(Print& out,void const*at,int sz,unsigned char (*memByte)(const void*));
void dumpRam(Print& out,void const*at,int sz);
void dumpPgm(Print& out,void const*at,int sz);

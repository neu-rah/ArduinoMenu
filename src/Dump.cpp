#include "Dump.h"
#include <Streaming.h>//https://github.com/scottdky/Streaming

unsigned char memByteRam(const void* x) {return *(char*)x;}
unsigned char memBytePgm(const void* x) {return pgm_read_byte(x);}

void dump(Print& out,void const*at,int sz,unsigned char (*memByte)(const void*)) {
  while(sz>0) {
    out<<"0x"<<_HEX((unsigned long)at)<<": ";
    for(int c=0;c<16;c++) {
      if (c==8) out<<" ";
      if (sz-c>0) {
        unsigned char v=memByte(at+c);
        out<<(char)(v>=32&&v<='z'?v:'.');
      } else out<<" ";
    }
    out<<" ";
    for(int c=0;c<16&&sz;c++,sz--,at+=1) {
      if (c==8) out<<" ";
      unsigned char v=memByte(at);
      out<<(v<16?"0":"")<<_HEX(v)<<(v==0x97?"=":" ");
    }
    out<<endl;
  }
}

void dumpRam(Print& out,void const*at,int sz) {return dump(out,at,sz,memByteRam);}
void dumpPgm(Print& out,void const*at,int sz) {return dump(out,at,sz,memBytePgm);}

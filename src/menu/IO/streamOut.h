/* -*- C++ -*- */
#pragma once

namespace Menu {
  
  template<typename Dev,Dev& dev>
  struct StreamOut {
    template<typename O>
    struct Part:O {
      void write(const char* o,Idx len){dev.write(o,len);}
      template<typename Data>
      void print(Data o,bool toPrint=true) {dev<<o;dev.flush();}
      void nl() {dev<<"\r\n";}
    };
  };

};//Menu namespace

/* -*- C++ -*- */
#pragma once

namespace Menu {
  template<typename Dev,Dev& dev>
  struct SerialOut {
    template<typename O>
    struct Part:O {
      void write(const char* o,Idx len){dev.write(o,len);}
      template<typename Data,typename... Args>
      void print(Data o,Args... args) {dev.print(o,args...);}
      void nl() {dev.println();}
    };
  };

};//Menu namespace

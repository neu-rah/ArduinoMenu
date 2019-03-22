/* -*- C++ -*- */
#pragma once

namespace AM5 {
  //menu output with root device
  template<typename Device,Device& dev,typename O>
  struct RawOut:public O {
    using This=RawOut<Device,dev,O>;
    using Root=typename O::Root;
    using Item=typename Root::Item;
    template<typename T>
    static inline void raw(T o) {
      Serial<<"RawOut::raw(T)"<<endl;
      dev<<o;
    }
    inline void out(Item& i) {
      Serial<<"RawOut::out(Item)"<<endl;
      i.out(*this);
    }
    // template<typename T> static inline void out(T o) {dev<<o;}
  };
};

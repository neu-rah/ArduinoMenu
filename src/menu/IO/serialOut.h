/* -*- C++ -*- */
#pragma once

namespace AM5 {
  //menu output with system raw output device
  template<typename Device,Device& dev,typename O>
  struct SerialOut:public O {
    using This=SerialOut<Device,dev,O>;
    using Root=typename O::Root;
    using Item=typename Root::Item;

    inline size_t out(Item& i) {return i.out(*this);}

    inline size_t raw(const char o[]) override {return dev.print(o);}
    inline size_t raw(char o) override {return dev.print(o);}
    inline size_t raw(unsigned char o, int f= DEC) override {return dev.print(o,f);}
    inline size_t raw(int o, int f= DEC) override {return dev.print(o,f);}
    inline size_t raw(unsigned int o, int f= DEC) override {return dev.print(o,f);}
    inline size_t raw(long o, int f= DEC) override {return dev.print(o,f);}
    inline size_t raw(unsigned long o, int f= DEC) override {return dev.print(o,f);}
    inline size_t raw(double o, int f= 2) override {return dev.print(o,f);}
    inline size_t raw(const __FlashStringHelper *o) override {return dev.print(o);}
    inline size_t raw(const String &o) override {return dev.print(o);}
    inline size_t raw(const Printable&o) override {return dev.print(o);}
  };
};

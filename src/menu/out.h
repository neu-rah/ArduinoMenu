/* -*- C++ -*- */
#pragma once

#include "menu.h"

namespace Menu {
  ///////////////////////////////////////////////////////////////////
  // output interface
  struct MenuOut {
    virtual MenuOut& operator<<(Item&) {return *this;}
    virtual MenuOut& operator<<(const char*) {return *this;}
    virtual MenuOut& operator<<(char) {return *this;}
    virtual MenuOut& operator<<(unsigned char) {return *this;}
    virtual MenuOut& operator<<(const __FlashStringHelper *i) {return *this;}
    template<Roles role>
    void fmt(bool io) {}
    virtual void fmtTitle(bool io) {}
  };

  template<typename O>
  using asTitle=Role<Roles::Title,O,&MenuOut::fmtTitle>;

  template<typename O>
  struct MenuOutCap:public MenuOut,public O {
    MenuOut& operator<<(Item& i) override;
    MenuOut& operator<<(const char* i) override {O::raw(i);return *this;}
    MenuOut& operator<<(char i) override {O::raw(i);return *this;}
    MenuOut& operator<<(unsigned char i) override {O::raw(i);return *this;}
    MenuOut& operator<<(const __FlashStringHelper * i) override {O::raw(i);return *this;}
    void fmtTitle(bool io) override {O::fmtTitle(io);}
  };

  //base for output combinators --------------------------
  struct Void {
    // inline void out(Item& i);
    template<typename T>
    void raw(T) {}//just ignore stuff
    void fmtTitle(bool io) {}
    enum OUTPUT_BASE {};//do not define this elsewhere
  };

  //just and example of wrapper/formnat
  template<typename O,char pref='[',char suf=']'>
  struct WrapTitle:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void fmtTitle(bool io) {//io: true->start, false->end
      this->O::raw(io?pref:suf);
    }
  };

  //bind output to existing device ---------------------------
  //use any stream as menu output
  template<typename Dev, Dev& dev, typename O>
  struct OutDev:public O {
    enum RAW_DEVICE {};
    static inline void raw(const char*i) {dev<<i;}
    static inline void raw(char i) {dev<<i;}
    static inline void raw(unsigned char i) {dev<<i;}
    static inline void raw(const __FlashStringHelper * i) {dev<<i;}
    //.. add more type here
  };
};//Menu

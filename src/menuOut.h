/* -*- C++ -*- */
#pragma once

#include "menu.h"

///////////////////////////////////////////////////////////////////
// output interface
struct MenuOut {
  virtual MenuOut& operator<<(Item& i) {return *this;}
  virtual MenuOut& operator<<(const char* i) {return *this;}
  virtual MenuOut& operator<<(char i) {return *this;}
  virtual MenuOut& operator<<(unsigned char i) {return *this;}
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
  void fmtTitle(bool io) override {O::fmtTitle(io);}
};

//base for output combinators --------------------------
struct Void {
  // inline void out(Item& i);
  template<typename T>
  void raw(T) {}//just ignore stuff
  void fmtTitle(bool io) {}
};

//just and example of wrapper/formnat
template<typename O,char pref='[',char suf=']'>
struct WrapTitle:public O {
  void fmtTitle(bool io) {//io: true->start, false->end
    this->O::raw(io?pref:suf);
  }
};

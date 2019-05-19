/* -*- C++ -*- */
#pragma once
/**
* @file out.h
* @author Rui Azevedo
* @brief ArduinoMenu output implementations
*/

#include "base.h"

template<typename Dev,Dev dev,typename O=FullPrinter<>>
struct RawOut:public O {
  template<typename T> static inline void raw(T o) {dev<<o;}
};

//dynamic output --------------------------------
template<typename O>
struct MenuOutDef:public O,public MenuOut {
  inline void nl() override {O::nl();}
  inline void raw(char c) override {O::raw(c);}
  inline void raw(const char*text) override {O::raw(text);}
  inline void raw(int n) override {O::raw(n);}
  virtual inline void raw(unsigned int n) {O::raw(n);}
  virtual inline void raw(long n) {O::raw(n);}
  virtual inline void raw(unsigned long n) {O::raw(n);}
  virtual inline void raw(double n) {O::raw(n);}
  inline void printItem(NavNode& nav,Item& i,idx_t n) override {i.printItem(nav,*this,n);}
  inline void fmt(Roles role,bool io,NavNode& nav,MenuOut& out,Item& i,idx_t n) override {
    switch(role) {
      case Roles::Panel: io?O::template fmtPanel<true>(nav,out,i,n):O::template fmtPanel<false>(nav,out,i,n);break;
      case Roles::Menu: io?O::template fmtMenu<true>(nav,out,i,n):O::template fmtMenu<false>(nav,out,i,n);break;
      case Roles::Title: io?O::template fmtTitle<true>(nav,out,i,n):O::template fmtTitle<false>(nav,out,i,n);break;
      case Roles::Body: io?O::template fmtBody<true>(nav,out,i,n):O::template fmtBody<false>(nav,out,i,n);break;
      case Roles::Item: io?O::template fmtItem<true>(nav,out,i,n):O::template fmtItem<false>(nav,out,i,n);break;
      case Roles::Index: io?O::template fmtIndex<true>(nav,out,i,n):O::template fmtIndex<false>(nav,out,i,n);break;
      case Roles::Cursor: io?O::template fmtCursor<true>(nav,out,i,n):O::template fmtCursor<false>(nav,out,i,n);break;
      case Roles::Name: io?O::template fmtName<true>(nav,out,i,n):O::template fmtName<false>(nav,out,i,n);break;
      case Roles::Mode: io?O::template fmtMode<true>(nav,out,i,n):O::template fmtMode<false>(nav,out,i,n);break;
      case Roles::Value: io?O::template fmtValue<true>(nav,out,i,n):O::template fmtValue<false>(nav,out,i,n);break;
      case Roles::Unit: io?O::template fmtUnit<true>(nav,out,i,n):O::template fmtUnit<false>(nav,out,i,n);break;
    }
  }
};

/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu output base

#include "base.h"

namespace Menu {
  //output base-------------------------------------------------------
  template<typename O=Nil>
  struct Void {
    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(NavHead& nav,OutHead& out) {}
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtPanel(NavHead& nav,OutHead& out) {}
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtMenu(NavHead& nav,OutHead& out) {}
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtTitle(NavHead& nav,OutHead& out) {}
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtMenuBody(NavHead& nav,OutHead& out) {}
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtItem(NavHead& nav,OutHead& out) {}
  };

  struct MenuOutBase {
    // virtual inline void printItem(Item&) {}
    virtual inline void raw(const char* o) const {}
    virtual inline void raw(char o) const {}
    virtual inline void nl() const {}
    virtual inline bool enabled(size_t i) const {return true;};
  };

  template<typename O>
  struct MenuOut:public MenuOutBase,public O {
    inline void raw(const char* o) const override {O::raw(o);}
    inline void raw(char o) const override {O::raw(o);}
    inline void nl() const override {O::nl();}
  };
};

/* -*- C++ -*- */
#pragma once
/**
* @file out.h
* @author Rui Azevedo
* @brief ArduinoMenu output components
*/

#include "nav.h"

///////////////////////////////////////////////////////
template<typename O> struct Void:O {
  template<typename T>
  static inline void raw(T o) {}
  static inline void nl() {}
  static inline void newView() {nl();}
  static inline void setCursor(idx_t x,idx_t y) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtPanel(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMenu(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtTitle(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtBody(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtItem(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtIndex(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtCursor(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtName(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMode(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtValue(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtUnit(Nav&,Out&,I&,idx_t) {}
};

/**
* The TextMeasure class is an output component that helps output devices to measure the output data
*/
struct TextMeasure:public Void<> {
  template<typename T>
  static inline idx_t measure(T o) {
    #ifdef ARDUINO
      return String(o).length();
    #else
      return _str(o);
    #endif
  }
  protected:
    #ifndef ARDUINO
    static inline idx_t _str(const char*o){return std::string(o).length();}
    template<typename T>
    static inline idx_t _str(T i){return std::string(std::to_string(i)).length();}
    #endif
};

/// a specialization for char measure... we might have to change this for UTF8 and surrogate character systems
template<> idx_t TextMeasure::measure<const char>(const char o) {return 1;}

//////////////////////////////////////////////////////////////////
template<typename Dev,Dev& dev,typename O=Void<>>
struct RawOut:public O {
  template<typename T>
  static inline void raw(T o) {dev<<o;}
  static inline void nl() {}
};

//////////////////////////////////////////////////////////////////
template<typename Dev,Dev& dev,typename O=Nil<>>
struct Out:public O {};

/**
* The IMenuOut is the virtual output interface definition
*/
struct IMenuOut {
  virtual void raw(const char* o)=0;
  #ifdef ARDUINO
    virtual inline void raw(const __FlashStringHelper* n) {}
  #endif
  virtual inline void fmt(Roles role,bool io,INavNode& nav,IMenuOut&,Item& i,idx_t) {}
  //just a comodity to inline a role (open/close)
  inline void fmt(Roles role,INavNode& nav,IMenuOut& out,Item& i,idx_t n) {
    fmt(role,true,nav,out,i,n);
    fmt(role,false,nav,out,i,n);
  }
};

/**
* The MenuOut class is the top level static component for the menu output system
*/
template<typename O>
struct MenuOut:public O {
  void raw(const char* o) override {O::raw(o);}
  #ifdef ARDUINO
    inline void raw(const __FlashStringHelper* n) override {O::raw(n);};
  #endif
};

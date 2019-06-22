/* -*- C++ -*- */
#pragma once
/**
* @file out.h
* @author Rui Azevedo
* @brief ArduinoMenu output components
*/

#include "nav.h"

//INT16_MAX this oes not work well with variable type
constexpr idx_t idx_max=((idx_t)~0)>>1;//assuming signed

//////////////////////////////////////////////
/**
* The Void class is output system terminal
*/
template<typename O>
struct Void:public O {
  template<typename T>
  static inline void raw(T o) {}
  static inline void nl() {}
  static inline void newView() {nl();}
  static inline void setCursor(idx_t x,idx_t y) {}
  template<typename Out>
  static inline void clrLine(Out& out,idx_t n) {}
  template<typename N,typename H,typename I>
  static void print(N,H,I) {}
  constexpr static inline bool isRange() {return true;}
  constexpr static inline bool isViewport() {return false;}
  constexpr static inline idx_t height() {return 0;}
  constexpr static inline idx_t top() {return 0;}
  static inline void setTop(idx_t) {}
  constexpr static inline idx_t posX() {return 0;}
  constexpr static inline idx_t posY() {return 0;}
  constexpr static inline idx_t freeX() {return idx_max;}
  constexpr static inline idx_t freeY() {return idx_max;}
  constexpr static inline idx_t free() {return idx_max;}
  static inline void useX(idx_t ux=1) {}
  static inline void useY(idx_t uy=1) {}
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
  template<typename N,typename H,typename I>
  inline void fmt(Roles role,N& nav,O& out,I& i,idx_t n) {
    fmt(role,true,nav,out,i,n);
    fmt(role,false,nav,out,i,n);
  }
  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,bool io,Nav& nav,Out& out,I& i,idx_t n) {
    //we could do better with templates, but we need this to be compatible
    //with virtual interface too without expanding the vtable too much
    switch(role) {
      case Roles::Panel:  io?out.template fmtPanel <true>(nav,out,i,n):out.template fmtPanel <false>(nav,out,i,n);break;
      case Roles::Menu:   io?out.template fmtMenu  <true>(nav,out,i,n):out.template fmtMenu  <false>(nav,out,i,n);break;
      case Roles::Title:  io?out.template fmtTitle <true>(nav,out,i,n):out.template fmtTitle <false>(nav,out,i,n);break;
      case Roles::Body:   io?out.template fmtBody  <true>(nav,out,i,n):out.template fmtBody  <false>(nav,out,i,n);break;
      case Roles::Prompt:   io?out.template fmtItem  <true>(nav,out,i,n):out.template fmtItem  <false>(nav,out,i,n);break;
      case Roles::Index:  io?out.template fmtIndex <true>(nav,out,i,n):out.template fmtIndex <false>(nav,out,i,n);break;
      case Roles::Cursor: io?out.template fmtCursor<true>(nav,out,i,n):out.template fmtCursor<false>(nav,out,i,n);break;
      case Roles::Name:   io?out.template fmtName  <true>(nav,out,i,n):out.template fmtName  <false>(nav,out,i,n);break;
      case Roles::Mode:   io?out.template fmtMode  <true>(nav,out,i,n):out.template fmtMode  <false>(nav,out,i,n);break;
      case Roles::Value:  io?out.template fmtValue <true>(nav,out,i,n):out.template fmtValue <false>(nav,out,i,n);break;
      case Roles::Unit:   io?out.template fmtUnit  <true>(nav,out,i,n):out.template fmtUnit  <false>(nav,out,i,n);break;
    }
  }
};

/**
* The RawOut class is the base for system's output devices encapsulation
*/
template<typename Dev,Dev& dev,typename O=Void<>>
struct RawOut:public O {
  template<typename T>
  inline void raw(T o) {dev<<o;}
};

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

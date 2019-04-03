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
    #ifdef ARDUINO
      virtual MenuOut& operator<<(endlObj) {return *this;}
      virtual MenuOut& operator<<(const __FlashStringHelper *i) {return *this;}
    #endif
    virtual void fmtMenu(bool io) {}
    virtual void fmtPanel(bool io) {}
    virtual void fmtTitle(bool io) {}
    virtual void fmtItem(bool io) {}
    virtual void fmtAccel(bool io) {}
    virtual void fmtCursor(bool io) {}
    virtual void fmtLabel(bool io) {}
    virtual void fmtMode(bool io) {}
    virtual void fmtValue(bool io) {}
    virtual void fmtUnit(bool io) {}
    virtual void printMenu(Item&) {}
  };

  template<typename O> using asMenu=Role<Roles::Menu,O,&MenuOut::fmtMenu>;
  template<typename O> using asPanel=Role<Roles::Panel,O,&MenuOut::fmtPanel>;
  template<typename O> using asTitle=Role<Roles::Title,O,&MenuOut::fmtTitle>;
  template<typename O> using asItem=Role<Roles::Item,O,&MenuOut::fmtItem>;
  template<typename O> using asAccel=Role<Roles::Accel,O,&MenuOut::fmtAccel>;
  template<typename O> using asCursor=Role<Roles::Cursor,O,&MenuOut::fmtCursor>;
  template<typename O> using asLabel=Role<Roles::Label,O,&MenuOut::fmtLabel>;
  template<typename O> using asMode=Role<Roles::Mode,O,&MenuOut::fmtMode>;
  template<typename O> using asValue=Role<Roles::Value,O,&MenuOut::fmtValue>;
  template<typename O> using asUnit=Role<Roles::Unit,O,&MenuOut::fmtUnit>;

  struct PrintHead {
    MenuOut& printer;
    size_t pos;
  };

  template<typename O>
  struct MenuOutCap:public MenuOut,public O {
    using This=MenuOutCap<O>;
    MenuOut& operator<<(Item& i) override;
    MenuOut& operator<<(const char* i) override {O::raw(i);return *this;}
    MenuOut& operator<<(char i) override {O::raw(i);return *this;}
    MenuOut& operator<<(unsigned char i) override {O::raw(i);return *this;}
    #ifdef ARDUINO
      MenuOut& operator<<(endlObj) override {O::raw("\n");return *this;}
      MenuOut& operator<<(const __FlashStringHelper * i) override {O::raw(i);return *this;}
    #endif
    void fmtMenu(bool io) override {O::fmtMenu(io);}
    void fmtPanel(bool io) override {O::fmtPanel(io);}
    void fmtTitle(bool io) override {O::fmtTitle(io);}
    void fmtItem(bool io) override {O::fmtItem(io);}
    void fmtAccel(bool io) override {O::fmtAccel(io);}
    void fmtCursor(bool io) override {O::fmtCursor(io);}
    void fmtLabel(bool io) override {O::fmtLabel(io);}
    void fmtMode(bool io) override {O::fmtMode(io);}
    void fmtValue(bool io) override {O::fmtValue(io);}
    void fmtUnit(bool io) override {O::fmtUnit(io);}
    void printMenu(Item& i) override {O::printMenuRaw(PrintHead{*this,0},i);}
  };

  //base for output combinators --------------------------
  struct Void {
    // inline void out(Item& i);
    template<typename T>
    void raw(T) {}//just ignore stuff
    void fmtMenu  (bool io) {}
    void fmtPanel (bool io) {}
    void fmtTitle (bool io) {}
    void fmtItem  (bool io) {}
    void fmtAccel (bool io) {}
    void fmtCursor(bool io) {}
    void fmtLabel (bool io) {}
    void fmtMode  (bool io) {}
    void fmtValue (bool io) {}
    void fmtUnit  (bool io) {}
    void printMenuRaw(PrintHead,Item&) {}
    enum OUTPUT_BASE {};//do not define this elsewhere
    constexpr static inline bool canNav() {return false;}
};

  //just and example of wrapper/formnat
  template<typename O,char pref='[',char suf=']'>
  struct WrapTitle:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void fmtTitle(bool io) {//io: true->start, false->end
      // Serial<<(io?"{":"|")<<"WrapTitle"<<(io?"|":"}")<<endl;
      if (io) {
        O::raw(pref);
        O::fmtTitle(io);
      } else {
        O::fmtTitle(io);
        O::raw(suf);
      }
    }
  };

  //bind output to existing device ---------------------------
  //use any stream as menu output
  template<typename Dev, Dev& dev, typename O>
  struct OutDev:public O {
    enum RAW_DEVICE {};
    constexpr bool hasRawDevice() {return true;}
    static inline void raw(const char*i) {dev<<i;}
    static inline void raw(char i) {dev<<i;}
    static inline void raw(unsigned char i) {dev<<i;}
    #ifdef ARDUINO
      static inline void raw(endlObj i) {dev<<i;}
      static inline void raw(const __FlashStringHelper * i) {dev<<i;}
    #endif
    //.. add more type here
  };

};//Menu

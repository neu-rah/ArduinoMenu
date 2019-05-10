/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu main include file
*/

#include <vector>
#include <iostream>
using namespace std;

template<typename O> using Id=O;
struct Nil {};

struct Item;

// output ////////////////////////////////////////////////////////////////

template<typename O=Nil> struct Void:public O {
  template<typename T> static inline void raw(T) {}
  template<typename Nav,typename Out,typename I>
  static inline void printMenu(Nav&,Out& out,I& i) {i.printItems(out);}
};

//static output
template<typename Dev,Dev dev,typename O=Void<>>
struct RawOut:public O {
  template<typename T>
  static inline void raw(T o) {dev<<o;}
  template<typename Nav,typename Out,typename I>
  static inline void printMenuRaw(Nav& nav,Out& out,I& i) {
    Out::printMenu(nav,out,i);
  }
};

//dynamic output
struct MenuOut {
  virtual inline void raw(const char*) {};
  virtual inline void printItem(Item&)=0;
};

template<typename O>
struct MenuOutDef:public O,public MenuOut {
  virtual inline void raw(const char*text) {O::raw(text);}
  inline void printItem(Item& i) override;
};

// menu items //////////////////////////////////////////////////////////////
template<typename O=Nil> struct Empty:public O {
  template<typename Out> static inline void print(Out&) {}
  // template<typename Out> static inline void printItems(Out& out) {print(out);}
};

//static ------------------------------------------------------------
template<const char**text,typename O=Empty<>>
struct StaticText:public O {
  template<typename Out>
  static inline void print(Out& out) {
    out.raw(text[0]);
    O::print(out);
  }
};

template<typename O,typename... OO>
struct StaticMenu:public StaticMenu<O> {
  using Next=StaticMenu<OO...>;
  template<typename Out>
  static inline void printItems(Out& out) {
    O::print(out);
    Next::printItems(out);
  }
};

template<typename O>
struct StaticMenu<O>:public O {
  template<typename Out>
  static inline void printItems(Out& out) {O::print(out);}
};

//dynamic -----------------------------------------------------------
struct Item {
  virtual inline void print(MenuOut& out) const {}
};

template<typename O>
struct Prompt:public Item,public O {
  virtual inline void print(MenuOut& out) const override {
    O::print(out);
  }
};

template<typename O>
struct VectorMenu:public O,vector<Item*> {
  using vector<Item*>::vector;
  template<typename... OO>
  inline VectorMenu(OO... oo):vector<Item*>{oo...} {}
  template<typename Out>
  inline void printItems(Out& out) {
    for(auto i: *this) {
      i->print(out);
    }
  }
};

// navigation ////////////////////////////////////////////

template<typename O=Nil> struct Drift:public O {};

template<typename Out,typename Data,typename O=Drift<>>
class StaticNav:public O {
  public:
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      // data.printItems(out);
      out.printMenuRaw(*this,out,data);
    }
  protected:
    Out out;
    Data data;
};

template<typename Out,typename Data,typename O=Drift<>>
class DynamicNav:public O {
  public:
    DynamicNav(Data& o):data(&o) {}
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      // data.printItems(out);
      out.printMenuRaw(*this,out,*data);
    }
  protected:
    Out out;
    Data* data;
};

#include "menu.hpp"

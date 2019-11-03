/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename Part,typename I> struct RoleChk:I {
  template<Roles P,bool io,typename It,typename Out,typename Nav>
  inline static void fmt(Idx n,const Nav& nav) {
    if (Part::template allowed<P>()||P==Roles::Raw) I::template fmt<P,io,It,Out,Nav>(n);
  }
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    if (Part::template allowed<P>()||P==Roles::Raw) I::template printTo<O,P,N>();
  }
};

template<Roles Part,typename I>
using Role=RoleChk<AllowRole<Part>,I>;

template<Roles Part,typename I>
using NoRole=RoleChk<DenyRole<Part>,I>;

template<typename Dev, Dev& dev,typename O=Void<>>
struct StreamOut:O {
  inline static void nl() {dev<<"\n\r";dev.flush();}
  template<typename T> inline static void raw(T o) {dev<<o;}
};


template<typename O=Void<>>
struct StaticMenuOut:Data<O> {
  inline static Idx top() {return Data<O>::obj.top();}
  inline static void setTop(Idx n) {Data<O>::obj.setTop(n);}
  inline static void newView() {Data<O>::obj.newView();}
  inline static void useY(Idx uy=1) {Data<O>::obj.useY(uy);}
  inline static void nl() {Data<O>::obj.nl();}
  inline static Idx freeY() {return Data<O>::obj.freeY();}
  template<typename Nav>
  inline static void posTop(Nav& nav) {Data<O>::obj.posTop(nav);}
};

//top level printer, using static data
template<typename O=Void<>>
struct MenuOut:O,IMenuOut {
  inline void print(char o)                 const override {O::raw(o);};
  inline void print(int o)                  const override {O::raw(o);}
  inline void print(long o)                 const override {O::raw(o);}
  inline void print(unsigned char o)        const override {O::raw(o);}
  inline void print(unsigned int o)         const override {O::raw(o);}
  inline void print(unsigned long o)        const override {O::raw(o);}
  inline void print(const char* o)          const override {O::raw(o);}
  inline void print(const unsigned char* o) const override {O::raw(o);}
  //-----------------------
  inline void nl() override {return O::nl();}
  inline void fmt(Roles r,bool io,INav& nav,IMenuOut& out,Idx n) override {
    switch(r) {
      case Roles::Panel:  io?O::template fmtPanel <true,IMenuOut,INav>(nav,out,n):O::template fmtPanel <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Menu:   io?O::template fmtMenu  <true,IMenuOut,INav>(nav,out,n):O::template fmtMenu  <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Title:  io?O::template fmtTitle <true,IMenuOut,INav>(nav,out,n):O::template fmtTitle <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Body:   io?O::template fmtBody  <true,IMenuOut,INav>(nav,out,n):O::template fmtBody  <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Prompt: io?O::template fmtItem  <true,IMenuOut,INav>(nav,out,n):O::template fmtItem  <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Index:  io?O::template fmtIndex <true,IMenuOut,INav>(nav,out,n):O::template fmtIndex <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Cursor: io?O::template fmtCursor<true,IMenuOut,INav>(nav,out,n):O::template fmtCursor<false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Name:   io?O::template fmtName  <true,IMenuOut,INav>(nav,out,n):O::template fmtName  <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Mode:   io?O::template fmtMode  <true,IMenuOut,INav>(nav,out,n):O::template fmtMode  <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Value:  io?O::template fmtValue <true,IMenuOut,INav>(nav,out,n):O::template fmtValue <false,IMenuOut,INav>(nav,out,n);break;
      case Roles::Unit:   io?O::template fmtUnit  <true,IMenuOut,INav>(nav,out,n):O::template fmtUnit  <false,IMenuOut,INav>(nav,out,n);break;
      default:break;
    }
  }
  inline void clrLine(Idx n) override {return O::clrLine(n);}
  inline Idx top() override {return O::top();};
  inline void posTop(INav& nav) override {O::posTop(nav);}
  inline Idx posY() override {return O::posY();}
  inline Idx freeY() override {return O::freeY();}
  inline virtual void newView() {return O::newView();}
  inline virtual void printMenu(IItem& it,INav& nav) {O::template printMenu<IItem,INav,IMenuOut>(it,nav,*this);}
};

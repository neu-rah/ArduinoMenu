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

//top level printer, using static data
template<typename O=Void<>>
struct MenuOut:Data<O> {
  inline static Idx top() {return Data<O>::obj.top();}
  inline static void setTop(Idx n) {Data<O>::obj.setTop(n);}
  inline static void newView() {Data<O>::obj.newView();}
  inline static void useY(Idx uy=1) {Data<O>::obj.useY(uy);}
  inline static void nl() {Data<O>::obj.nl();}
  inline static Idx freeY() {return Data<O>::obj.freeY();}
  template<typename Nav>
  inline static void posTop(Nav& nav) {Data<O>::obj.posTop(nav);}
};

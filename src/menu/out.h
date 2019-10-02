/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename Part,typename I> struct RoleChk:I {
  template<Roles P,bool io,typename It,typename Out,typename Nav>
  static inline void fmt(Idx n,const Nav& nav) {
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
  inline static void nl() {dev<<"\n\r";}
  template<typename T> inline static void raw(T o) {dev<<o;}
};

//top level printer
template<typename O>
struct MenuOut:O {};

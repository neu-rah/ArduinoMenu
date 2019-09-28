/* -*- C++ -*- */
#pragma once
#include "api.h"

template<const char** text,typename I=Empty<>>
struct StaticText:I {
  template<typename Out> inline static void print() {Out::raw(text[0]);}
  template<typename Out> inline static void print(Out& out) {out.raw(text[0]);}
  template<typename It,typename Nav,typename Out>
  inline static void print(It&,Nav&,Out& out,Ref,Idx) {print(out);}
};

template<typename I,typename... II>
struct StaticData:I {
  using This=StaticData<I,II...>;
  using Head=I;
  using Tail=StaticData<II...>;
  template<typename It,typename Nav,typename Out>
  inline static void print(It&,Nav&,Out& out,Ref,Idx) {
    I::print(out);
  }

  template<typename It,typename Nav>
  inline bool enter(It&,Nav&,Ref ref,Idx n) {}
};

template<typename I>
struct StaticData<I>:I {
  using This=StaticData<I>;
  template<typename It,typename Nav,typename Out>
  inline static void print(It&,Nav&,Out& out,Ref,Idx) {print(out);}
};

template<typename T,typename B>
struct StaticMenu:B {
  using Title=T;
  using Body=B;
};

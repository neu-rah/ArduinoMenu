/* -*- C++ -*- */
#pragma once

#include "api.h"

template<const char** text,typename I=Empty<>>
struct StaticText:I,lambda::StaticText<text> {
  using This=StaticText<text,I>;
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    O::raw(text[0]);
    I::template printTo<O,P,N>();
  }
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu() {
  //   // It::template printTo<Out,Nav>();
  //   I::template printMenu<It,Out,Nav>();
  // }
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu() {Out::template printMenu<This,Out,Nav>(This());}
};

// using StaticText=lambda::Curry<_StaticText,2>;//using this invalidates the default argument!

template<typename I,typename... II>
struct StaticMenuData:StaticMenuData<I> {
  using This=StaticMenuData<I,II...>;
  using Head=I;
  using Tail=StaticMenuData<II...>;
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printItem(size_t i) {
    i?Tail::template printItem<O,P,N>(i-1):I::template printTo<O,P,N>();
  }
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu() {Out::template printMenu<This,Out,Nav>(This());}
  inline static constexpr idx_t size() {return Tail::size()+1;}
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  inline static void fmtItem(idx_t at,idx_t n,const Nav& nav,Out& out) {
    if (at) Tail::template fmtItem<r,io,It,Out,Nav>(at-1,n,nav,out);
    else Out::template fmt<r,io,It,Out,Nav>(n,nav,out);
  }
};

template<typename I>
struct StaticMenuData<I>:I {
  using This=StaticMenuData<I>;
  template<typename O,Roles P,typename N=Drift<>>
  inline static void printTo() {}
  template<typename O,Roles P,typename N=Drift<>>
  inline static void printItem(size_t i) {I::template printTo<O,P,N>();}
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu(Nav& nav) {Out::template printMenu<This,Out,Nav>(nav);}
  inline static constexpr idx_t size() {return 1;}
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  inline static void fmtItem(idx_t at,idx_t n,const Nav& nav,Out& out) {Out::template fmt<r,io,It,Out,Nav>(n,nav,out);}
};

template<typename Title,typename Data>
struct StaticMenu:Title, Data {
  using This=StaticMenu<Title,Data>;
  using Title::printTo;
  using Data::printItem;
  using Data::size;
  using Title::fmt;
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu(Nav& nav) {Out::template printMenu<It,Nav>(This(),nav);}
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  inline static void fmt(idx_t n,const Nav& nav,Out& out) {Data::template fmtItem<r,io,It,Out,Nav>(n,n,nav,out);}
};

//this is the top level of static items
template<typename I>
struct Item:I {
  template<typename Out,typename Nav=Drift<>>
  inline static void printMenu(Nav& nav) {I::template printMenu<I,Out,Nav>(nav);}
};

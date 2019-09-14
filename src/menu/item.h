/* -*- C++ -*- */
#pragma once

#include "api.h"

template<const char** text,typename I=Empty<>>
struct StaticText:I,lambda::StaticText<text> {
  using This=StaticText<text,I>;
  template<typename O,typename N=Drift<>>
  inline static void printTo() {
    O::raw(text[0]);
    I::template printTo<O,N>();
  }
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu() {
  //   // It::template printTo<Out,Nav>();
  //   I::template printMenu<It,Out,Nav>();
  // }
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu() {Out::template printMenu<This,Out,Nav>(This());}
};

template<typename I,typename... II>
struct StaticMenu:I {
  using This=StaticMenu<I,II...>;
  using Head=I;
  using Tail=StaticMenu<II...>;
  template<typename O,typename N=Drift<>>
  inline static void printItem(size_t i) {i?Tail::template printItem<O,N>(i-1):I::template printTo<O,N>();}
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu() {Out::template printMenu<This,Out,Nav>(This());}
  inline static constexpr idx_t size() {return Tail::size()+1;}
};

template<typename I>
struct StaticMenu<I>:I {
  using This=StaticMenu<I>;
  template<typename O,typename N=Drift<>>
  inline static void printTo() {}
  template<typename O,typename N=Drift<>>
  inline static void printItem(size_t i) {I::template printTo<O,N>();}
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu() {Out::template printMenu<This,Out,Nav>(This());}
  inline static constexpr idx_t size() {return 1;}
};

//this is the top level of static items
template<typename I>
struct Item:I {
  template<typename Out,typename Nav=Drift<>>
  inline static void printMenu() {I::template printMenu<I,Out,Nav>();}
};

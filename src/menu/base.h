/* -*- C++ -*- */
#pragma once

#include <lpp.h>
#include "debug.h"

//config
using TinyCore=lambda::False;
#define MDO cout
constexpr bool useItemFmt=true;

//aux stuff
// using Nil=lambda::Nil;
struct Nil{};
using idx_t=lambda::Expr<TinyCore,lambda::As<uint8_t>,lambda::Int>::App::Type;

template<typename T>
constexpr T T_MAX=(((long long)1)<<((sizeof(T)<<3)-1))-1;

//INT16_MAX this oes not work well with variable type
constexpr idx_t idx_max=T_MAX<idx_t>;//(1<<((sizeof(idx_t)<<3)-1))-1;//((idx_t)~0)>>1;//assuming signed


enum class Modes {Normal,Edit,Tune};
enum class Roles:idx_t {Panel,Menu,Title,Body,Prompt,Index,Cursor,Name,Mode,Value,Unit,Raw};

constexpr char* roleNames[]{
  "Panel","Menu","Title","Body","Prompt","Index",
  "Cursor","Name","Mode","Value","Unit","Raw"
};

template<typename O>
constexpr inline O& operator<<(O& o,Roles r) {return o<<roleNames[(idx_t)r];}

struct TextMeasure;
template<typename N=Nil> struct Drift;
template<typename N=Nil> struct Void;
template<typename N=Nil> struct Empty;

template<Roles Part>
struct AllowRole {
  template<Roles P> inline static constexpr bool allowed() {return Part==P;}
};

template<Roles Part>
struct DenyRole {
  template<Roles P> inline static constexpr bool allowed() {return Part!=P;}
};

template<typename Part,typename I> struct RoleChk:I {
  template<Roles P,bool io,typename It,typename Out,typename Nav>
  static inline void fmt(idx_t n,const Nav& nav) {
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

//plug filters---------------------------------------------------------
//compose them on top of menu items to filter printing by role
//allows disabling menu title print,
// however title will be used as prompt
// this can also allow to differentiate prompt and menu title texts...

//only print as role...
template<typename I> using AsPanel =Role<Roles::Panel ,I>;
template<typename I> using AsMenu  =Role<Roles::Menu  ,I>;
template<typename I> using AsTitle =Role<Roles::Title ,I>;
template<typename I> using AsBody  =Role<Roles::Body  ,I>;
template<typename I> using AsPrompt=Role<Roles::Prompt,I>;
template<typename I> using AsIndex =Role<Roles::Index ,I>;
template<typename I> using AsCursor=Role<Roles::Cursor,I>;
template<typename I> using AsName  =Role<Roles::Name  ,I>;
template<typename I> using AsMode  =Role<Roles::Mode  ,I>;
template<typename I> using AsValue =Role<Roles::Value ,I>;
template<typename I> using AsUnit  =Role<Roles::Unit  ,I>;

//print on all roles except...
template<typename I> using NoMenu  =NoRole<Roles::Menu  ,I>;
template<typename I> using NoTitle =NoRole<Roles::Title ,I>;
template<typename I> using NoBody  =NoRole<Roles::Body  ,I>;
template<typename I> using NoPrompt=NoRole<Roles::Prompt,I>;
template<typename I> using NoIndex =NoRole<Roles::Index ,I>;
template<typename I> using NoCursor=NoRole<Roles::Cursor,I>;
template<typename I> using NoName  =NoRole<Roles::Name  ,I>;
template<typename I> using NoMode  =NoRole<Roles::Mode  ,I>;
template<typename I> using NoValue =NoRole<Roles::Value ,I>;
template<typename I> using NoUnit  =NoRole<Roles::Unit  ,I>;
template<typename I> using NoPanel =NoRole<Roles::Panel ,I>;

//map Item API (void returning) calls over an agregate of items
//void returning because this is not a reduce or fold and not even a map to something
//so this is all about side-effects :D
template<typename O,typename... OO>
struct Map:O {
  using This=Map<O,OO...>;
  using Tail=Map<OO...>;

  template<typename It,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    O::template printTo<It,P,N>();
    Tail::template printTo<It,P,N>();
  }

  template<typename It,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printItem(size_t i) {
    O::template printItem<It,P,N>(i);
    Tail::template printItem<It,P,N>(i);
  }

  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu() {
  //   It::template printMenu<It,Out,Nav>();
  //   Tail::template printMenu<It,Out,Nav>();
  // }

  template<Roles r,bool io,typename It,typename Out,typename Nav>
  inline static void fmt(idx_t n,const Nav& nav) {
    It::template fmt<r,io,It,Out,Nav>(n,nav);
    Tail::template fmt<r,io,It,Out,Nav>(n,nav);
  }
};

template<typename O> struct Map<O>:O {};

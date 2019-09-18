/* -*- C++ -*- */
#pragma once

#include <lpp.h>
#include "debug.h"

//config
using TinyCore=lambda::False;
#define MDO cout
constexpr bool useItemFmt=true;

//aux stuff
using Nil=lambda::Nil;
using idx_t=lambda::Expr<TinyCore,lambda::As<uint8_t>,lambda::Int>::App::Type;

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
  static inline void fmt(idx_t n) {
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

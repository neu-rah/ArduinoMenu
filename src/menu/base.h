/* -*- C++ -*- */
#pragma once

#include <lpp.h>
#include "debug.h"

//config
using TinyCore=lambda::False;
#define MDO cout

//aux stuff
using Nil=lambda::Nil;
using idx_t=lambda::Expr<TinyCore,lambda::As<uint8_t>,lambda::Int>::App::Type;

enum class Modes {Normal,Edit,Tune};
enum class Roles {Panel,Menu,Title,Body,Prompt,Index,Cursor,Name,Mode,Value,Unit,Raw};

struct TextMeasure;
template<typename N=Nil> struct Drift;
template<typename N=Nil> struct Void;
template<typename N=Nil> struct Empty;

template<Roles Part,typename I> struct Role:I {
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    if (Part==P||P==Roles::Raw) I::template printTo<O,P,N>();
  }
};

template<Roles Part,typename I> struct NoRole:I {
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    if (Part==P||P==Roles::Raw) I::template printTo<O,P,N>();
  }
};

//plug fileters
//only print on role
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
//print all roles except
template<typename I> using NoMenu  =Role<Roles::Menu  ,I>;
template<typename I> using NoTitle =Role<Roles::Title ,I>;
template<typename I> using NoBody  =Role<Roles::Body  ,I>;
template<typename I> using NoPrompt=Role<Roles::Prompt,I>;
template<typename I> using NoIndex =Role<Roles::Index ,I>;
template<typename I> using NoCursor=Role<Roles::Cursor,I>;
template<typename I> using NoName  =Role<Roles::Name  ,I>;
template<typename I> using NoMode  =Role<Roles::Mode  ,I>;
template<typename I> using NoValue =Role<Roles::Value ,I>;
template<typename I> using NoUnit  =Role<Roles::Unit  ,I>;
template<typename I> using NoPanel =Role<Roles::Panel ,I>;

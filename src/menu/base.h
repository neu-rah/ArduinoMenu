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

enum class Roles {Panel,Menu,Title,Body,Prompt,Index,Cursor,Name,Mode,Value,Unit};
enum class Modes {Normal,Edit,Tune};

struct TextMeasure;
template<typename N=Nil> struct Drift;
template<typename N=Nil> struct Void;
template<typename N=Nil> struct Empty;

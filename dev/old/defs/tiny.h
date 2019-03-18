/* -*- C++ -*- */
#pragma once
#include <Arduino.h>
#include <menu.h>
#include <streamFlow.h>

/*
simplest text menu
ram based
static size and options structure
no associated actions
no accels
no menu titles
raw print to serial
*/

using serialOut=decltype(Serial);

using Sys=SysDef<serialOut,Nil,uint8_t>;
using idx_t=Sys::Idx;//its uint8_t we just sent it

// template<typename O> using Prompt=Sys::Prompt<O>;
// using Static=Sys::Static<>;

template<const char** text>
using Title=Sys::asTitle<Sys::Static<>::Text<text>>;

template<const char** text>
using OP=Sys::Prompt<
  Title<text>
>;

template<idx_t n>
using MENU=Sys::Prompt<
  Sys::Static<>::Menu<n>
>;

inline serialOut& operator<<(serialOut& o, typename SysDef<serialOut,Nil,idx_t>::IfPrompt& i) {return i.operator<<(o);}

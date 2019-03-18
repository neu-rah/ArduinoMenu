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
menu title
raw print to serial
*/

using serialOut=decltype(Serial);

using Sys=SysDef<serialOut,Nil,uint8_t>;
using idx_t=Sys::Idx;//its uint8_t we just sent it
using Title=Sys::Text<>;
using OP=Sys::Prompt<Title>;

template<idx_t n>
using MENU=Sys::Static<Title>::Menu<n>;

inline serialOut& operator<<(serialOut& o, typename SysDef<serialOut,Nil,idx_t>::IfPrompt& i) {return i.operator<<(o);}

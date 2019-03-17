/* -*- C++ -*- */
#pragma once
#include <menu.h>

/*
simplest text menu
ram based
static size and options structure
no associated actions
no accels
no menu titles
raw print to serial
*/
using Sys=SysDef<decltype(Serial),uint8_t>;
using idx_t=Sys::Idx;//its uint8_t we just sent it

// template<typename O> using Prompt=Sys::Prompt<O>;
// using Static=Sys::Static<>;

template<const char** text>
using Title=Sys::asTitle<Sys::Static<>::Text<text[0]>>;

template<const char** text>
using OP=Sys::Prompt<
  Title<text[0]>
>;

template<const char** title,idx_t n>
using MENU=Sys::Prompt<
  Sys::Static<>::Menu<n>
>;

/* -*- C++ -*- */
// #pragma once

#include <menu.h>

using Out=typename ArduinoMenu::Framework::Arduino::Out;
using Def=MenuSystemDef<Out>;
using Base=Def::Base;
using Empty=typename Def::Empty;

template<typename O>
using Item=Def::Item<O>;

template<const char** text>
using Text=typename Def::StaticText<text,Empty>;

template<const char** text>
using Title=asTitle<Text<text>>;

template<const char** text>
using Op=Item<Title<text>>;

template<const char** text,size_t n>
using Menu=Def::StaticMenu<n,Title<text>>;

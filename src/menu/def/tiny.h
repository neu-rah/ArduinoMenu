/* -*- C++ -*- */
// #pragma once

#include <menu.h>
#include <menu/component/flashText.h>

// using Out=typename ArduinoMenu::Framework::Arduino::Out;
using Def=MenuSystemDef<Out>;
using Base=Def::Base;
using Empty=typename Def::Empty;

template<typename O>
using Item=Def::Item<O>;

using Text=FlashTextDef<Empty>;

using Title=asTitle<Text>;

using Op=Item<Title>;

template<size_t n>
using Menu=Def::StaticMenu<n,Title>;

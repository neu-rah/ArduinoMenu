/* -*- C++ -*- */
#pragma once

#include <streamFlow.h>//https://github.com/neu-rah/streamFlow
#include "../../menu.h"
#include "../IO/serialOut.h"
#include "../comp/flashText.h"

template<typename O>
using MenuOut=Menu::MenuOutCap<O>;

//describing an output -----------------------------------------
//MenuOutCap - top level adapter for menu output, wraps a type-level composition
//WrapTitle - type level block will format all titles with surrounding []
//SerialOutDev - an output device bound to a serial port (arduino)
using SerialOut=Menu::SerialOutDev<Serial>;

template<typename O>
using asTitle=Menu::asTitle<O>;
using Text=Menu::Text<Menu::Empty>;

//describing an option ------------------------------------
// asTitle - role description, its meaning is interpreted by
//           an inner output device/format/filter (output composition chain)
using Op=Menu::asItem<Text>;

using FlashText=Menu::FlashTextDef<Menu::Empty>;

//a menu option using flash text
using FlashOp=Menu::asItem<FlashText>;

using Item=Menu::Item;
template<typename O>
using Prompt=Menu::Prompt<O>;

template<size_t n>
using StaticMenu=Menu::asMenu<
  Menu::StaticMenu<
    n,
    asTitle<Text>
  >
>;

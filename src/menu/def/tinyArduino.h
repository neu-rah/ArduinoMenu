/* -*- C++ -*- */
#pragma once

#include <streamFlow.h>//https://github.com/neu-rah/streamFlow
#include "../../menu.h"
#include "../IO/serialOut.h"
#include "../comp/flashText.h"
#include "../printers.h"

template<typename O>
using MenuOut=Menu::MenuOutCap<O>;

//describing an output -----------------------------------------
template<typename P=Menu::DeviceParts<>>
using SerialOut=Menu::SerialOutDev<P,Serial>;

//common role
template<typename O>
using asTitle=Menu::asTitle<O>;
//common element
using Text=Menu::Text<Menu::Empty>;

//describing an option ------------------------------------
using Op=Menu::asItem<Text>;

using FlashText=Menu::FlashTextDef<Menu::Empty>;

//a menu option using flash text
using FlashOp=Menu::asItem<FlashText>;

using Item=Menu::Item;
template<typename O>
using Prompt=Menu::Prompt<O>;

template<size_t n>
using StaticMenu=Menu::StaticMenu<n,asTitle<Text>>;

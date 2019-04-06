/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// common definitions for arduino framework

#include <streamFlow.h>//https://github.com/neu-rah/streamFlow
#include "../../menu.h"
#include "../printers.h"
#include "../comp/flashText.h"
#include "../comp/flashMenu.h"

template<typename O>
using MenuOut=Menu::MenuOutCap<O>;

using FlashText=Menu::FlashTextDef<Menu::Empty>;

using Text=Menu::Text<Menu::Empty>;

using Item=Menu::Item;

template<typename O>
using Prompt=Menu::Prompt<O>;

template<size_t n>
using StaticMenu=Menu::StaticMenu<n,Text>;

template<size_t n>
using FlashMenu=Menu::StaticMenu<n,FlashText>;

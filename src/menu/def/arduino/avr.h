/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// common definitions for arduino framework on atmega device

#include <streamFlow.h>//https://github.com/neu-rah/streamFlow
#include "../../../menu.h"
#include "../../printers.h"
#include "../../comp/flashText.h"
#include "../../comp/flashMenu.h"

namespace Menu {

  using namespace Menu;

  template<typename O>
  using MenuOut=AM5::MenuOutCap<O>;

  using FlashText=AM5::FlashTextDef<AM5::Empty>;

  using Text=AM5::Text<AM5::Empty>;

  using Item=AM5::Item;

  template<typename O>
  using Prompt=AM5::Prompt<O>;

  template<size_t n>
  using StaticMenu=AM5::StaticMenu<n,Text>;

  template<size_t n>
  using FlashMenu=AM5::StaticMenu<n,FlashText>;
};

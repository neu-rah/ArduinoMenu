/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// common definitions for arduino framework

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

//common element
using Text=Menu::Text<Menu::Empty>;

//describing an option ------------------------------------
using FlashText=Menu::FlashTextDef<Menu::Empty>;

using Item=Menu::Item;

template<typename O>
using Prompt=Menu::Prompt<O>;

template<size_t n>
using StaticMenu=Menu::StaticMenu<n,Text>;

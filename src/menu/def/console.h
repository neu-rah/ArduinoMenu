/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// common definitions for standard output

#include <iostream>
using namespace std;

#include "../../menu.h"
#include "../IO/consoleOut.h"

/*
tipical definitions for standard output
*/

//this must be the final class for menu output
template<typename O>
using MenuOut=Menu::MenuOutCap<O>;

//describing an output -----------------------------------------
template<typename O>
using ConsoleOut=Menu::ConsoleOutDef<O>;

using Text=Menu::Text<Menu::Empty>;

template<typename O>
using asTitle=Menu::asTitle<O>;

//describing an option ------------------------------------
using Op=Menu::asItem<Text>;//option will be formatted as title

template<typename O>
using Prompt=Menu::Prompt<O>;

template<size_t n>
using StaticMenu=Menu::StaticMenu<n,asTitle<Text>>;

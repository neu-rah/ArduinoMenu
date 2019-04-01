/* -*- C++ -*- */
#pragma once

#include <iostream>
using namespace std;

#include "../../menu.h"
#include "../IO/consoleOut.h"

/*
tipical definitions for standard output
*/

//describing an output -----------------------------------------
using ConsoleOut=Menu::MenuOutCap<Menu::WrapTitle<Menu::ConsoleOutDef>>;

using Text=Menu::Text<Menu::Empty>;

template<typename O>
using asTitle=Menu::asTitle<O>;

//describing an option ------------------------------------
using Op=asTitle<Text>;//option will be formatted as title

template<typename O>
using Prompt=Menu::Prompt<O>;

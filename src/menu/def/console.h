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

namespace Menu {
  /*
  tipical definitions for standard output
  */

  //this must be the final class for menu output
  template<typename O>
  using MenuOut=AM5::MenuOutCap<O>;

  //describing an output -----------------------------------------
  // template<typename O>
  // using ConsoleOut=AM5::ConsoleOutDef<O>;

  using Text=AM5::Text<AM5::Empty>;

  template<typename O>
  using Prompt=AM5::Prompt<O>;

  template<size_t n>
  using StaticMenu=AM5::StaticMenu<n,Text>;
};

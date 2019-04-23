/* -*- C++ -*- */
#pragma once

#include "menuAssert.h"

namespace AM5 {
  using idx_t=int;
  using ConstText=const char[];
  template<typename O> using Id=O;
  struct Nil {};

  // enum class Roles {Root,Panel,Menu,Title,Item,Index,Cursor,Label,Value,Unit};
  //
  // template<Roles role>
  // struct Role {};
  //
  // using asItem=Role<Roles::Item>;

  // template<typename N,typename O,typename I>
  // struct PrintHead {
  //   using Nav=N;
  //   using Printer=O;//this might be different from Nav::rawOut
  //   using Item=I;//the current Item type
  //   Nav& nav;
  //   Printer& printer;
  //   Item& item;
  // };

};

/* -*- C++ -*- */
#pragma once

// #include "sys/kernel/base.h"

namespace Menu {
  ////////////////////////////////////////////////////////////////////////////////
  //basic input, just be quiet
  template<typename In=Nil>
  struct Quiet:In {
    using Base=In;
    template<typename Nav>
    inline static constexpr bool parseKey(Nav&) {return false;}
    template<typename Nav>
    inline static constexpr bool parseCmd(Nav&,Key,bool=false) {return false;}
    template<typename Nav>
    inline bool cmd(Nav& nav,int code=0) {return Base::obj().parseKey(nav);}
  };

  template<template<typename> class... I>
  using InDef=Composition<Quiet,I...>;

};//menu namespace
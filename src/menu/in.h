/* -*- C++ -*- */
#pragma once

#include "sys/base.h"

namespace Menu {
  ////////////////////////////////////////////////////////////////////////////////
  //basic input, just be quiet
  template<typename In=Nil>
  struct Quiet:In {
    using Base=In;
    template<typename Dev> inline static void poll(Dev&) {}
    inline static void begin() {}
    inline static void relax() {}
    template<typename Nav>
    inline static constexpr bool parseKey(Nav&) {return false;}
    template<typename Nav>
    inline static constexpr bool parseCmd(Nav&,Key,bool=false) {return false;}
    template<typename Nav>
    inline bool cmd(Nav& nav,int code=0) {return Base::obj().parseKey(nav);}
  };

  template<template<typename> class Base,template<typename> class... I>
  using InDefBase=Composition<Base,I...,Quiet>;

};//menu namespace
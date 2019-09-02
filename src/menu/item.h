/* -*- C++ -*- */
#pragma once
/**
* @file item.h
* @author Rui Azevedo
* @brief ArduinoMenu data/structure representation and base components
*/


#include "base.h"

/** \defgroup base menu items
*  @{
*/
//////////////////////////////////////////////////////////////////
template<typename... OO>
struct StaticMenu:List<OO...> {};

/**
* The Item class is the virtual data interface definition
*/
struct Item {
 virtual inline void print(INavNode&,IMenuOut&) {}
};

/**
* The Prompt class is the top level virtual data implementation
* it should encapsulate a data compsition and abide to the virtual interface
*/
template<typename I>
struct Prompt:public Item, public I {
 inline void print(INavNode& nav,IMenuOut& out) override {I::print(nav,out,(I&)*this);}
 template<typename N,typename O,typename H>
 inline void print(N& nav,O& out,H& i) {I::print(nav,out,i);}
};

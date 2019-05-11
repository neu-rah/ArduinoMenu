/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu interfaces (API's)
*/

template<typename O> using Id=O;
struct Nil {};

struct Item;
struct MenuOut;
struct NavNode;

//navigation interface
struct NavNode {};

//output interface
struct MenuOut {
  virtual inline void nl() {}
  virtual inline void raw(char) {};
  virtual inline void raw(const char*) {};
  virtual inline void printItem(NavNode&,Item&)=0;
  //TODO: reduce vtable, use a bool parameter and demux on static side instead!
  virtual inline void fmtPanelStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtPanelEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtMenuStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtMenuEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtTitleStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtTitleEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtBodyStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtBodyEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtItemStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtItemEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtIndexStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtIndexEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtCursorStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtCursorEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtNameStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtNameEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtModeStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtModeEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtValueStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtValueEnd(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtUnitStart(NavNode&,MenuOut&,Item&) {}
  virtual inline void fmtUnitEnd(NavNode&,MenuOut&,Item&) {}
};

//item interface
struct Item {
  virtual inline void print(NavNode&,MenuOut& out) {}
  virtual inline void printItem(NavNode&,MenuOut& out) {}
};

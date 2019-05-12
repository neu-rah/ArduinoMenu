/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu interfaces (API's)
*/

using idx_t=int;
using ConstText=const char[];

enum class Roles {Panel,Menu,Title,Body,Item,Index,Cursor,Name,Mode,Value,Unit};

template<typename O> using Id=O;
struct Nil {};

struct Item;
struct MenuOut;
struct NavNode;

//navigation interface
struct NavNode {
  virtual inline bool selected(idx_t) const {return false;}
  virtual inline bool enabled(idx_t) const {return true;}
  virtual inline bool up()=0;
  virtual inline bool down()=0;
  virtual inline bool left()=0;
  virtual inline bool right()=0;
  virtual inline bool enter()=0;
  virtual inline bool esc()=0;
};

//output interface
struct MenuOut {
  virtual inline void nl() {}
  virtual inline void raw(char) {};
  virtual inline void raw(const char*) {};
  virtual inline void raw(int) {};
  virtual inline void raw(unsigned int) {};
  virtual inline void raw(long) {};
  virtual inline void raw(unsigned long) {};
  virtual inline void raw(double) {};
  virtual inline void printItem(NavNode&,Item&,idx_t)=0;
  //TODO: reduce vtable, use a bool parameter and demux on static side instead!
  virtual inline void fmt(Roles role,bool io,NavNode& nav,MenuOut&,Item& i,idx_t) {}
  inline void fmt(Roles role,NavNode& nav,MenuOut& out,Item& i,idx_t n) {
    fmt(role,true,nav,out,i,n);
    fmt(role,false,nav,out,i,n);
  }
};

//item interface
struct Item {
  virtual inline void print(NavNode&,MenuOut& out) {}
  virtual inline void printItem(NavNode&,MenuOut& out,idx_t n) {}
};

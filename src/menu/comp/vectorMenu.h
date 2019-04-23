/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use std::vector to hold menu data
// with full power of std::vector
// TODO: generalize for other containers

#include <vector>
using namespace std;
#include <menu.h>

namespace AM5 {

  struct Item {
    // template<typename N>
    // static inline void out() {
    // can not parameterize this function to output because it is a virtual
    // dont want to parameterize the class to output because it will have data... we dont want duplicates
    virtual void out(MenuOut& o) const {}
      // constexpr static inline size_t size() {return Next::size()+1;}
    virtual size_t size() const {return 1;}
    //?
    // virtual Item& operator[](size_t)=0;
    // virtual NavAgent activate()=0;
  };

  //static composition blocks -----------------------
  // struct Empty {
  //   inline Empty() {}
  //   inline Empty(Empty&) {}
  //   static inline void out(MenuOut&) {}
  //   static inline size_t size() {return 1;}
  //   inline Item& operator[](size_t n) {return *reinterpret_cast<Item*>(this);}
  //   static inline NavAgent activate();
  //   static inline bool up() {return false;}
  //   static inline bool down() {return false;}
  //   static inline bool enter() {return false;}
  //   static inline bool esc() {return false;}
  //   static EmptyCmds<false> cmds;
  // };

  //adapt specific types as menu items
  //provide virtual overrides for them
  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    using This=Prompt<O>;
    inline void out(MenuOut& o) const override {O::out(o);}
    size_t size() const override {return O::size();}
    // Item& operator[](size_t n) override {return O::operator[](n);}
    // inline NavAgent activate() override {return O::activate();}
    //not used yet --
    template<template<typename> class T>
    inline void stack(MenuOut& o) const {Prompt<T<O>>(*this).out(o);}
  };

  template<typename O>
  struct VectorMenuDef:public O,vector<Item*> {
    template<typename... OO>
    inline VectorMenuDef(OO... oo):vector<Item*>{oo...} {}
    template<typename... OO>
    inline VectorMenuDef(const char*title,OO... oo):O(title),vector<Item*>{oo...} {}
    inline size_t size() const {return vector<Item*>::size();}
    inline Item& operator[](size_t n) const {
      return *vector<Item*>::operator[](n);
    }
  };

};//AM5 namespace

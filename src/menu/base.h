/* -*- C++ -*- */
#pragma once

using idx_t=int;

struct Nil {};

template<typename I=Nil> struct Empty;
template<typename I=Nil> struct Void;
template<typename I=Nil> struct Drift;
template<typename Data,typename N=Nil> struct StaticNav;
template<typename N=Drift<>> class NavPos;

struct IItem;
struct IMenuOut;
struct INavNode;

template<typename O,typename... OO>
struct Node:public Node<O> {
  using This=O;
  using Next=Node<OO...>;
  void map(void(*f)(O)) {
    f(*this);
    next.map(f);
  }
  Next next;
};

template<typename O>
struct Node<O>:public O {
  using This=O;
  void map(void(*f)(O)) {f(this);}
};

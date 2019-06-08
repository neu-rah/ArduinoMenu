/* -*- C++ -*- */
#pragma once

using idx_t=int;

struct Nil {};

template<typename I=Nil> struct Empty;
template<typename I=Nil> struct Void;
template<typename I=Nil> struct Drift;
template<typename Data,typename N=Drift<>> struct StaticNav;
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

/**
* The Chain class is an utility to make composition nesting cleaner
* and easier to maintain.
*/
///type nesting from a list of templates+defered terminal type
#define Expr template<typename> class
#define Term typename

template<Expr... OO>
struct Chain {
  template<Term _T,Expr _O, Expr... _OO>
  struct Links:public _O<Links<_T,_OO...>> {using _O<Links<_T,_OO...>>::_O;};

  template<Term _T,Expr _O>
  struct Links<_T,_O>:public _O<_T> {using _O<_T>::_O;};

  template<Term T>
  struct To:public Links<T,OO...> {};
  //provide a continuation
  template<Expr _O>
  using With=Chain<OO...,_O>;
};

#undef Expr
#undef Term

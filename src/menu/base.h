/* -*- C++ -*- */
#pragma once
/**
* @file base.h
* @author Rui Azevedo
* @brief generic definitions and aux. stuff
*/


using idx_t=int;

enum class Roles {Panel,Menu,Title,Body,Prompt,Index,Cursor,Name,Mode,Value,Unit};
enum class Modes {Normal,Edit,Tune};

struct Nil {};

#if defined(ARDUINO) && (defined(__AVR_ATtiny13__) || defined(ATTINY_CORE))
using __FlashStringHelper=const char*;
#endif

template<typename I=Nil> struct Empty;
template<typename I=Nil> struct Void;
template<typename I=Nil> struct Drift;
template<typename N> class StaticNav;
template<typename N=Drift<>,typename Idx=idx_t> class NavPosBase;

struct Item;
struct IMenuOut;
struct INavNode;
struct NavAgent;

// template<typename O,typename... OO>
// struct Node:public Node<O> {
//   using This=O;
//   using Next=Node<OO...>;
//   void map(void(*f)(O)) {
//     f(*this);
//     next.map(f);
//   }
//   Next next;
// };
//
// template<typename O>
// struct Node<O>:public O {
//   using This=O;
//   void map(void(*f)(O)) {f(this);}
// };

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

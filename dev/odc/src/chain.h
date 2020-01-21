/* -*- C++ -*- */
#pragma once

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

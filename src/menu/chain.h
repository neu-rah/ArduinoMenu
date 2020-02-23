/* -*- C++ -*- */
#pragma once


// template<templateA,B>
// struct Glue:A {
//   template<typename Nav,typename Out,Op op=Op::Printing>
//   inline void print(Nav& nav,Out& out,PathRef ref=self) {

//   }
//   template<typename Nav,typename Out,Op op=Op::Printing>
//   inline void glue_print(Nav& nav,Out& out,PathRef ref=self) {
//     A::
//   }    
// };

namespace Menu {
  #define Expr template<typename> class
  #define Term typename

  template<Expr... OO>
  struct Chain {
    template<Term _T,Expr _O, Expr... _OO>
    struct Links:_O<Links<_T,_OO...>> {using _O<Links<_T,_OO...>>::_O;};

    template<Term _T,Expr _O>
    struct Links<_T,_O>: _O<_T> {using _O<_T>::_O;};

    template<Term T>
    struct To:Links<T,OO...> {
      using Links<T,OO...>::Links;
    };
    //provide a continuation
    template<Expr _O>
    using With=Chain<OO...,_O>;
  };
};

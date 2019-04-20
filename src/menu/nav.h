/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {
  ////////////////////////////////////////////////////////////
  template<typename O=Nil>
  struct Drift {};

  template<typename Menu, typename Out,typename O=Drift<>>
  class Nav:public O {
    public:
      using This=Nav<Menu,Out,O>;
      static inline Menu& getMenu() {return menu;}
      static inline Out& getRaw() {return rawOut;}
      template<size_t idx>
      static inline bool selected() {return nav.pos()==idx;}
      template<size_t idx>
      static inline bool enabled() {return menu.template enabled<idx>();}
      template<size_t idx>
      static inline void enable(bool o=true) {menu.template enable<idx>(o);}
      //output proxy
      template<typename T>
      static inline void raw(T o) {rawOut.raw(o);}
      template<typename I>
      static inline void out(I& i) {i.template out<This>();}
      static inline void nl() {rawOut.nl();}
      // formats ---------------------------
      template<typename I,bool io,size_t idx>
      static inline void fmtItem() {rawOut.template fmtItem<This,I,io,idx>();}
      template<typename I,bool io,size_t idx>
      static inline void fmtIndex() {rawOut.template fmtIndex<This,I,io,idx>();}
      template<typename I,bool io,size_t idx>
      static inline void fmtCursor() {rawOut.template fmtCursor<This,I,io,idx>();}
    protected:
      static Menu menu;
      static Out rawOut;
      static This nav;
  };

  template<typename O=Drift<>>
  class PosDef:public O {
    public:
      template<size_t idx>
      inline bool selected() const {return at==idx;}
      inline bool up() {
        if (at<O::size()-1) {at++;return true;}
        return O::up();
      }
      inline bool down() {
        if (at>0) {at--;return true;}
        return O::down();
      }
      inline size_t pos() const {return at;}
    protected:
      size_t at=0;
  };

};

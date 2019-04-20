/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {
  ////////////////////////////////////////////////////////////
  template<typename O=Nil>
  struct Drift {
    //navigation API ------------------------
    constexpr static inline size_t size() {return 0;}
    // constexpr static inline size_t pos() {return 0;}
    template<typename> constexpr static inline bool _up() {return false;}
    template<typename> constexpr static inline bool _down() {return false;}
    template<typename N> constexpr static inline bool _left() {return N::down();}
    template<typename N> constexpr static inline bool _right() {return N::up();}
    template<typename> constexpr static inline bool _enter() {return false;}
    template<typename> constexpr static inline bool _esc() {return false;}
  };

  template<typename Menu, typename Out,typename O>
  class Nav:public O {
    public:
      using This=Nav<Menu,Out,O>;
      // using NavBase=O;
      //navigation API ------------------------
      static inline bool up() {return nav.template _up<This>();}
      static inline bool down() {return nav.template _down<This>();}
      static inline bool left() {return nav.template _left<This>();}
      static inline bool right() {return nav.template _right<This>();}
      static inline bool enter() {return nav.template _enter<This>();}
      static inline bool esc() {return nav.template _esc<This>();}
      // static inline Menu& getMenu() {return menu;}
      // static inline Out& getRaw() {return rawOut;}
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
      static inline size_t top() {return rawOut.top();}
      static inline size_t height() {return rawOut.height();}
      // formats ---------------------------
      template<typename I,bool io,size_t idx=0>
      static inline void fmtMenu() {rawOut.template fmtMenu<This,I,io,idx>();}
      template<typename I,bool io,size_t idx=0>
      static inline void fmtMenuBody() {rawOut.template fmtMenuBody<This,I,io,idx>();}
      template<typename I,bool io,size_t idx=0>
      static inline void fmtTitle() {rawOut.template fmtTitle<This,I,io,idx>();}
      template<typename I,bool io,size_t idx>
      static inline void fmtItem() {rawOut.template fmtItem<This,I,io,idx>();}
      template<typename I,bool io,size_t idx>
      static inline void fmtIndex() {rawOut.template fmtIndex<This,I,io,idx>();}
      template<typename I,bool io,size_t idx>
      static inline void fmtCursor() {rawOut.template fmtCursor<This,I,io,idx>();}
      // printer -----------------------------------------
      static inline void printMenu() {
        if (rawOut.isRange()) {
          //ensure that selection option is withing range
          while(rawOut.top()>nav.pos())
            rawOut.setTop(rawOut.top()-1);
          while(nav.pos()>=rawOut.top()+rawOut.height())
            rawOut.setTop(rawOut.top()+1);
        }
        fmtMenu<Menu,true>();
        fmtTitle<Menu,true>();
        out(menu);
        fmtTitle<Menu,false>();
        fmtMenuBody<Menu,true>();
        menu.template printItems<This,Menu>();
        fmtMenuBody<Menu,false>();
        fmtMenu<Menu,false>();
      }
      // items ---------------------------
      constexpr static inline size_t size() {return menu.size();}
    protected:
      static Menu menu;
      static Out rawOut;
      static Nav<Menu,Out,O> nav;
  };

  template<typename O=Drift<>>
  class PosDef:public O {
    public:
      template<size_t idx>
      inline bool selected() const {return at==idx;}
      template<typename Nav>
      inline bool _up() {
        if (at<Nav::size()-1) {at++;return true;}
        return O::template _up<Nav>();
      }
      template<typename Nav>
      inline bool _down() {
        if (at>0) {at--;return true;}
        return O::template _down<Nav>();
      }
      inline size_t pos() const {return at;}
    protected:
      size_t at=0;
  };

};

// auto nl=out.endl;

template<typename Menu, typename Out,typename O,typename T>
AM5::Nav<Menu,Out,O>& operator<<(AM5::Nav<Menu,Out,O>& o,T x) {
  o.raw(x);
  return o;
}

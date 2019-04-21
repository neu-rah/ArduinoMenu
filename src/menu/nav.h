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
    template<size_t idx> constexpr static inline bool selected() {return false;}
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
      static inline bool selected() {return O::template selected<idx>();}
      template<size_t idx>
      static inline bool enabled() {return menu.template enabled<idx>();}
      template<size_t idx>
      static inline void enable(bool o=true) {menu.template enable<idx>(o);}
      //output proxy -----------------------------
      template<typename T>
      static inline void raw(T o) {rawOut.template raw<This>(o);}
      static inline void raw(This&(*f)(This&)) {(*f)(nav);}
      static inline This& endl(This& o) {nl();return o;}
      template<typename I>
      static inline void out(I& i) {i.template out<This>();}
      static inline void nl() {rawOut.nl();}
      static inline size_t top() {return rawOut.top();}
      static inline size_t height() {return rawOut.height();}
      //viewports
      static inline void newView() {rawOut.newView();}
      //device coordinates ---------
      static inline idx_t posX() {return rawOut.posX();}
      static inline idx_t posY() {return rawOut.posY();}
      // get free space ----
      static inline idx_t freeX() {return rawOut.freeX();}
      static inline idx_t freeY() {return rawOut.freeY();}
      static inline idx_t free() {return rawOut.free();}
      // use space ----
      static inline void useX(idx_t ux=1) {rawOut.useX(ux);}
      static inline void useY(idx_t uy=1) {rawOut.useY(uy);}
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
        rawOut.newView();
        fmtMenu<Menu,true>();
        fmtTitle<Menu,true>();
        out(menu);
        fmtTitle<Menu,false>();
        fmtMenuBody<Menu,true>();
        if (rawOut.isRange()) {
          //ensure that selection option is withing range
          while(rawOut.top()+posY()>nav.pos())
            rawOut.setTop(rawOut.top()-1);
          while(nav.pos()>=rawOut.top()+nav.freeY())
            rawOut.setTop(rawOut.top()+1);
        }
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
      static inline bool selected() {return at==idx;}
      template<typename Nav>
      static inline bool _up() {
        if (at<Nav::size()-1) {at++;return true;}
        return O::template _up<Nav>();
      }
      template<typename Nav>
      static inline bool _down() {
        if (at>0) {at--;return true;}
        return O::template _down<Nav>();
      }
      static inline size_t pos() {return at;}
    protected:
      static size_t at;
  };
};

// auto nl=out.endl;

template<typename Menu, typename Out,typename O,typename T>
AM5::Nav<Menu,Out,O>& operator<<(AM5::Nav<Menu,Out,O>& o,T x) {
  o.raw(x);
  return o;
}

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
    template<typename Nav,size_t idx,typename T> static inline void printMenu(T&) {}
  };

  template<typename O>
  struct PanelPrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenu(T& menu) {
      Nav::newView();
      Nav::template fmtPanel<T,true>();

      using MenuPrinter=typename Nav::Printers::template Menu<O>;
      MenuPrinter::template printMenu<Nav,0>(menu);

      Nav::template fmtPanel<T,false>();
    }
  };

  template<typename O>
  struct MenuPrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenu(T& menu) {
      Nav::template fmtMenu<T,true>();
      //plugin a printer
      using TitlePrinter=typename Nav::Printers::template Title<O>;
      TitlePrinter::template printMenu<Nav,0>(menu);

      using BodyPrinter=typename Nav::Printers::template Body<O>;
      BodyPrinter::template printMenu<Nav,0>(menu);

      Nav::template fmtMenu<T,false>();
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenu(T& menu) {
      Nav::template fmtTitle<T,true,idx>();
      Nav::out(menu);
      Nav::template fmtTitle<T,false>();
    }
  };

  template<typename O>
  struct BodyPrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenu(T& menu) {
      Nav::template fmtMenuBody<T,true>();
      if (Nav::isRange()) {
        //ensure that selection option is withing range
        while(Nav::top()+Nav::posY()>Nav::pos())
          Nav::setTop(Nav::top()-1);
        while(Nav::pos()>=Nav::top()+Nav::freeY())
          Nav::setTop(Nav::top()+1);
      }
      menu.template printItems<Nav,T>();
      Nav::template fmtMenuBody<T,false>();
    }
  };

  template<
    template<typename> class P=PanelPrinter,
    template<typename> class M=MenuPrinter,
    template<typename> class T=TitlePrinter,
    template<typename> class B=BodyPrinter,
    template<typename> class I=Id
  > struct PrintersCfg {
    template<typename O> using Panel=P<O>;
    template<typename O> using Menu=M<O>;
    template<typename O> using Title=T<O>;
    template<typename O> using Body=B<O>;
    template<typename O> using Item=I<O>;
  };

  using DefaultPrinters=PrintersCfg<>;

  template<typename Menu, typename Out,typename O,typename P=DefaultPrinters>
  class Nav:public O {
    public:
      using Printers=P;
      using This=Nav<Menu,Out,O,Printers>;
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
      constexpr static inline bool isRange() {return rawOut.isRange();}
      constexpr static inline bool isViewport() {return rawOut.isViewport();}
      template<typename T>
      static inline void raw(T o) {rawOut.template raw<This>(o);}
      static inline void raw(This&(*f)(This&)) {(*f)(nav);}
      static inline This& endl(This& o) {nl();return o;}
      template<typename I>
      static inline void out(I& i) {i.template out<This>();}
      static inline void nl() {rawOut.nl();}
      static inline size_t top() {return rawOut.top();}
      static inline void setTop(size_t n) {rawOut.setTop(n);}
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
      static inline void fmtPanel() {rawOut.template fmtPanel<This,I,io,idx>();}
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
        using PanelPrinter=typename Printers::template Panel<O>;
        PanelPrinter::template printMenu<This,0>(menu);
      }
      // items ---------------------------
      constexpr static inline size_t size() {return menu.size();}
    protected:
      static Menu menu;
      static Out rawOut;
      static Nav<Menu,Out,O,Printers> nav;
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

template<typename Menu, typename Out,typename O, typename P,typename T>
AM5::Nav<Menu,Out,O,P>& operator<<(AM5::Nav<Menu,Out,O,P>& o,T x) {
  o.raw(x);
  return o;
}

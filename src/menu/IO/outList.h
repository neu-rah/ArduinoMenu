#pragma once

namespace Menu {
  //a list of output devices
  template<typename O,typename... OO>
  struct OutList:StaticMenuBody<O,OO...> {
    // using _tail=OutList<OO...>;
    using Base=StaticMenuBody<O,OO...>;
    using Base::head;
    inline void begin() {
      Base::head().begin();
      Base::tail().begin();
    }
    inline void nl() {
      Base::head().nl();
      Base::tail().nl();
    }
    inline void newView() {
      Base::head().newView();
      Base::tail().newView();
    }
    template<typename Nav,typename Menu>
    void printMenu(Nav& nav,Menu& menu,Idx level,Idx selIdx) {
      Base::head().template printMenu<Nav,Menu>(nav,menu,selIdx);
      Base::tail().template printMenu<Nav,Menu>(nav,menu,selIdx);
    }
  };

  template<typename O>
  struct OutList<O>:O {};

}//namespace Menu
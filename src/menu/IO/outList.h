/* -*- C++ -*- */
#pragma once
/**
* @file outList.h
* @author Rui Azevedo
* @brief ArduinoMenu multiple outputs chain
*/

template<typename O,typename... OO>
class OutList:public OutList<O> {
  public:
    using This=OutList<O,OO...>;
    using Next=OutList<OO...>;
    using O::O;
    template<typename Nav,typename Out,typename I>
    inline void printMenu(Nav& nav,Out& out,I& i) {
      trace(MDO<<"OutList::printMenu<O,OO...> "<<endl);
      OutList<O>::template printMenu<Nav,Out,I>(nav,out,i);
      next.newView();
      next.template printMenu<Nav,Next,I>(nav,next,i);
    }
  protected:
    Next next;
};

template<typename O>
class OutList<O>:public O {
  public:
    using O::O;
    template<typename Nav,typename Out,typename I>
    inline void printMenu(Nav& nav,Out& out,I& i) {
      trace(MDO<<"OutList::printMenu<O> "<<endl);
      O::template printMenu<Nav,Out,I>(nav,out,i);
    }
};

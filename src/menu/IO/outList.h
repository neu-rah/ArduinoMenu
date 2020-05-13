/* -*- C++ -*- */
#pragma once
/**
* @file outList.h
* @author Rui Azevedo
* @brief ArduinoMenu multiple outputs chain
*/

namespace Menu {
  template<typename O,typename... OO>
  class OutList:public OutList<O> {
    public:
      using This=OutList<O,OO...>;
      using Next=OutList<OO...>;
      using O::O;
      template<typename It,typename Nav,Op op=Op::Printing>
      void printMenu(It& it,Nav& nav,Idx level,bool fullPrint) {
        trace(MDO<<"OutList::printMenu<O,OO...> "<<(int)&out<<endl);
        OutList<O>::template printMenu<It,Nav,op>(it,nav,level,fullPrint);
        next.newView();
        next.template printMenu<It,Nav,op>(it,nav,level,fullPrint);
      }
    protected:
      Next next;
  };

  template<typename O>
  class OutList<O>:public O {
    public:
      using O::O;
      template<typename It,typename Nav,Op op=Op::Printing>
      void printMenu(It& it,Nav& nav,Idx level,bool fullPrint) {
        trace(MDO<<"OutList::printMenu<O> "<<(int)&out<<endl);
        O::template printMenu<It,Nav,op>(it,nav,level,fullPrint);
      }
  };
};

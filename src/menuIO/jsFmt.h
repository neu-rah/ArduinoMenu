/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_JSFMT
  #define RSITE_ARDUINO_MENU_JSFMT
  #include "../menuDefs.h"

  //NOT A VALID FILE.. work in progress... maybe

  namespace Menu {

    template<class T>
    class jsFmt:public T {
      public:
        using T::T;
        result fmt(bool start,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) {
          assert(idx>=0&&idx<nav.sz());
          //prompt* n=&nav[idx];
          switch(part) {
            case menuOut::fmtSelect:
            case menuOut::fmtChoose:
              if(!start) break;
              *this<<"menuReflex(\"";
              nav.root->printPath(*this);
              *this
                <<"/"<<idx<<"\","
                <<((menuVariant<int>*)&nav[idx])->reflex
                <<");";
              return quit;
              break;
            default:break;
          }
          return proceed;
        }
        result fmtStart(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(true,part,nav,idx);}
        result fmtEnd(prompt& target,menuOut::fmtParts part,navNode &nav,idx_t idx=-1) override {return fmt(false,part,nav,idx);}
    };
  }//namespace
#endif

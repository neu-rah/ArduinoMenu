/* -*- C++ -*- */
#pragma once

//command input parsers

#include "kernel/api.h"

namespace Menu {

  #ifdef ARDUINO
    inline bool isdigit(uint8_t k) {return k>='0'&&k<='9';}
  #endif

  //navigate by numeric ID
  //ID's are ment for external call
  //however we have this parsaer just in case (and for testing)
  struct Ids {
    template<typename In>
    struct Part:In {
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        static bool onId=false;
        static Idx id=0;
        if(isdigit(k)) {
          onId=true;
          id*=10;
          id+=k-'0';
          return false;
        } else {
          bool res=false;
          if(onId) {//dispatch command
            trace(MDO<<"seeking id:"<<id<<endl);
            res=APICall::Activate<Nav>(nav).walkId(nav.root,id);
            id=0;
            onId=0;
          }
          return res||In::parseCmd(nav,k);//chain
        }
      }
    };
  };

  //classical AM4 navigation keys
  template<char up='+',char down='-',char enter='*',char esc='/'>
  struct AMNavKeys {
    template<typename In>
    struct Part:In {
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        trace(MDO<<"AMNavKeys::parseCmd"<<endl);
        switch(k) {
          case up: return nav.template cmd<Cmd::Up>();
          case down: return nav.template cmd<Cmd::Down>();
          case enter: return nav.template cmd<Cmd::Enter>();
          case esc: return nav.template cmd<Cmd::Esc>();
          default:return In::parseCmd(nav,k);
        }
      }
    };
  };

  //clasical AM4 index
  struct IndexAccel {
    template<typename In>
    struct Part:In {
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        trace(MDO<<"IndexAccel::parseCmd "<<(char)k<<endl);
        return isdigit(k)?
           nav.template cmd<Cmd::Index>(k-'0'):
           In::parseCmd(nav,k,e);
      }
    };
  };

  //must be the last one in chain because it will consume all keys
  struct Accel {
    template<typename In>
    struct Part:In {
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        trace(MDO<<"Accel::parseCmd"<<endl);
        return nav.template cmd<Cmd::Accel>(k);
      }
    };
  };
};

/* -*- C++ -*- */
#pragma once

//command input parsers

namespace Menu {

  #ifdef ARDUINO
    inline bool isdigit(uint8_t k) {return k>='0'&&k<='9';}
  #endif

  struct Ids {
    template<typename In=None>
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
            _trace(MDO<<"seeking id:"<<id<<endl);
            auto a=APICall::Activate();
            res=nav.template walkId<decltype(a)>(a,id);
            id=0;
            onId=0;
          }
          return res||In::parseCmd(nav,k);//chain
        }
      }
    };
  };

  template<char up='+',char down='-',char enter='*',char esc='/'>
  struct AMNavKeys {
    template<typename In=None>
    struct Part:In {
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        _trace(MDO<<"AMNavKeys::parseCmd"<<endl);
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
};

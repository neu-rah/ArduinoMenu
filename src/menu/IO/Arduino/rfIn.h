#pragma once

#include "menu/in.h"
#include <debug.h>
using namespace Parts;

#include <tinyTimeUtils.h>
using namespace TinyTimeUtils;

namespace Menu {

  //RF input device, reads command codes and eventually call parser
  template<int relaxTimeout=100>
  struct RFIn {
    template<typename In=Quiet<>>
    struct Part:In {
      static constexpr bool isReader=true;
      static volatile uint32_t rfCmd;
      static uint32_t lastCmd;
      static Tick<relaxTimeout> radioRelax;
      static bool rcvCmd(unsigned long cmd) {
        rfCmd=cmd;
        radioRelax.reset();
        // _trace(clog<<"RFIn::rcvCmd: 0x"<<hex<<cmd<<endl);
        return true;
      }
      void relax() {
        // _trace(clog<<"RFIn::relax"<<endl);
        //allow commands to repeat
        if(radioRelax) {
          lastCmd=0;
          rfCmd=0;
        }
        In::relax();//pass on
      }
      template<typename Nav>
      inline bool parseKey(Nav& nav) {
        if(lastCmd!=rfCmd) {
          // _trace(clog<<"RFin::parseKey: 0x"<<hex<<rfCmd<<endl);
          lastCmd=rfCmd;
          return In::parseCmd(nav,lastCmd);
        }
        return In::parseKey(nav);
      }
    };
  };

};


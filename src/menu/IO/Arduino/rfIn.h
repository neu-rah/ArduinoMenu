#pragma once

#include "menu/in.h"
#include <debug.h>
using namespace Parts;

namespace Menu {

  //RF input device, reads command codes and eventually call parser
  struct RFIn {
    template<typename In=Quiet<>>
    struct Part:In {
      static constexpr bool isReader=true;
      static volatile uint32_t rfCmd;
      static uint32_t lastCmd;
      static bool rcvCmd(unsigned long cmd) {
        rfCmd=cmd;
        _trace(clog<<"cmd: 0x"<<hex<<cmd<<endl);
        return true;//In::recvCmd(cmd);
      }
      void relax() {
        //allow commands to repeat
        lastCmd=0;
        rfCmd=0;
        In::relax();//pass on
      }
      template<typename Nav>
      inline bool parseKey(Nav& nav) {
        if(lastCmd!=rfCmd) {
          // Serial.print("parseKey:");
          // Serial.print(lastCmd);
          // Serial.print("==");
          // Serial.println(rfCmd);
          lastCmd=rfCmd;
          return In::parseCmd(nav,lastCmd);
        }
        return false;
      }
    };
  };

};


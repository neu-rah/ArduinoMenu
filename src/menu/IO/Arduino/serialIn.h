/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  #include "../../sys/base.h"

  namespace Menu {

    //Arduino serial input reader
    template<typename Dev,Dev& dev>
    struct SerialIn {
      template<typename In>
      struct Part:In {
        static constexpr bool isReader=true;
        template<typename Nav>
        inline bool parseKey(Nav& nav) {
          return dev.available()&&In::parseCmd(nav,dev.read());
        }
      };
    };

    //defaults to Arduino `Serial` as input device
    template<decltype(Serial)& dev=Serial>
    using ArduinoSerialIn=SerialIn<decltype(Serial),dev>;


  };
#endif
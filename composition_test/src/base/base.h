/* -*- C++ -*- */
#pragma once

namespace ArduinoMenu {
  template<typename Raw,typename O>
  struct OutDef:public O {
    using This=OutDef<Raw,O>;
    using RawOut=Raw;
  };
}

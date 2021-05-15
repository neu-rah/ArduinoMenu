#include "rfIn.h"

namespace Menu {
  template<int relaxTimeout>
  template<typename In>
  volatile uint32_t RFIn<relaxTimeout>::Part<In>::rfCmd;

  template<int relaxTimeout>
  template<typename In>
  uint32_t RFIn<relaxTimeout>::Part<In>::lastCmd;

  template<int relaxTimeout>
  template<typename In>
  Tick<relaxTimeout> RFIn<relaxTimeout>::Part<In>::radioRelax;

};


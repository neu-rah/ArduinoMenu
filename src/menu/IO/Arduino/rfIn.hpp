#include "rfIn.h"

namespace Menu {
  template<typename In> volatile uint32_t RFIn::Part<In>::rfCmd;
  template<typename In> uint32_t RFIn::Part<In>::lastCmd;
};


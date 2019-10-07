/* -*- C++ -*- */
#pragma once

#include "../out.h"
using Console=StreamOut<decltype(std::cout),std::cout>;

#ifdef MENU_DEBUG
  template<typename O>
  using Debug=DebugOut<Console,O>;
#endif

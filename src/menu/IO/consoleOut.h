/* -*- C++ -*- */
#pragma once

#include <iostream>
#include "../staticIO.h"

namespace Menu {
  template<typename O>
  using Console=StreamOut<decltype(std::cout),std::cout>::Part<O>;
};

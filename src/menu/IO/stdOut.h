/* -*- C++ -*- */
#pragma once

#include <iostream>
#include "streamOut.h"

namespace Menu {
  struct StdOut:StreamOut<decltype(cout),cout> {};
};
/* -*- C++ -*- */
#pragma once

#include <iostream>
#include "../menuIO.h"

template<typename O>
using Console=StreamOut<decltype(std::cout),std::cout>::Part<O>;

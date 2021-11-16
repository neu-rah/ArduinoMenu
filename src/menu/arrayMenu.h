#pragma once
/**
* @author Rui Azevedo
* @brief C array menu
*/

#include <menu/item.h>

namespace Menu {
  template<typename I>
  using ArrayItem=Prompt::template Part<I>*;
};
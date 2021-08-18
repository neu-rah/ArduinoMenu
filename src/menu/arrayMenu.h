#pragma once
/**
* @author Rui Azevedo
* @brief C array menu
*/

#include <menu/item.h>

namespace Menu {
  template<typename I>
  using ArrayMenu=Prompt::template Part<I>*[];
};
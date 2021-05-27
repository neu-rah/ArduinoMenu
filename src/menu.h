/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu main include file

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "menu/sys/staticMenu.h"
#include "menu/sys/components.h"
#include "menu/sys/fields.h"
#include "menu/fmt/xml.h"
#include "menu/fmt/text.h"
#include "menu/nav.h"
#include "menu/item.h"
#include "menu/IO/Arduino/rfIn.hpp"

namespace Menu {
  template<typename Cfg>
  struct ConfigMenuLib {
    template<typename O>
    using CP=typename Cfg::template Part<O>;
    template<template<typename> class... I>
    using ItemDef=ItemDefBase<CP,I...>;
    template<template<typename> class... I>
    using InDef=InDefBase<CP,I...>;\
    template<template<typename> class... I>
    using OutDef=OutDefBase<CP,I...>;
    template<template<typename> class... I>
    using NavDef=NavDefBase<CP,I...>;
  };
};

#define CONFIG_MENU_LIB(Cfg)\
  using MenuLib=Menu::ConfigMenuLib<Cfg>;\
  template<template<typename> class... I>\
  using ItemDef=MenuLib::ItemDef<I...>;\
  template<template<typename> class... I>\
  using InDef=MenuLib::InDef<I...>;\
  template<template<typename> class... I>\
  using OutDef=MenuLib::OutDef<I...>;\
  template<template<typename> class... I>\
  using NavDef=MenuLib::NavDef<I...>;


#if defined(RS_DEBUG)&&!defined(RELEASE)
// namespace Menu {
//   MDO mdo;
// };
#endif

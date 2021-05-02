/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu main include file

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

// // _trace(constexpr static const char*named=#name;);
// // _trace(MDO& operator<<(MDO& o) const {return o<<named;})
// #define APIDEF(memberFunc,name)\
// struct name {\
//   template<typename T,typename... Args>\
//   inline auto operator()(T& o,Args... args)\
//     ->decltype(o.memberFunc(args...))\
//     {return o.memberFunc(args...);}\
// };
// // _trace(MDO& operator<<(MDO& o,const name& api) {return api.operator<<(o);})


#include "menu/sys/staticMenu.h"
#include "menu/sys/components.h"
#include "menu/sys/fields.h"
#include "menu/fmt/xml.h"
#include "menu/fmt/text.h"

#if defined(RS_DEBUG)&&!defined(RELEASE)
// namespace Menu {
//   MDO mdo;
// };
#endif

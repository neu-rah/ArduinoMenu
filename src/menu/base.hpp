#include "base.h"

// inline void MenuOut::fmt(Roles,bool,NavNode&) {
//   switch(role) {
//     case Panel:fmt
//     case Menu:
//     case Title:
//     case Body:
//     case Item:
//     case Index:
//     case Cursor:
//     case Name:
//     case Mode:
//     case Value:
//     case Unit:
//   }
// }

// template<Roles role,typename O,void (MenuOut::*f)(bool)>
// void Role<role,O,f>::out(MenuOut&o) const {
//   fmt(o,true);
//   O::out(o);
//   fmt(o,false);
// }
//
// template<Roles role,typename O,void (MenuOut::*f)(bool)>
// inline void Role<role,O,f>::fmt(MenuOut& o,bool io) const {
//   (static_cast<MenuOut&>(o).*f)(io);
// }

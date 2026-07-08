/* -*- C++ -*- */
/********************
Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

AM5 — ArduinoMenu's AM4-syntax rebuilt on OneMenu/HAPI (zero-overhead
compile-time composition, see github.com/InternetOfPins/HAPI). This branch's
src/ is a thin forward onto OneMenu's compat layer, not a reimplementation —
the actual macros/components live in OneMenu, pulled in as a dependency
(see library.json). MENU/FIELD/OP/EXIT/SUBMENU/TOGGLE/SELECT/CHOOSE/VALUE/
MENU_INPUTS/MENU_OUTPUTS/NAVROOT all resolve through this include.

www.r-site.net
***/
#pragma once

#include <oneMenu/compat/am4.h>

/* -*- C++ -*- */
// AM5 stub: real AM4 sketches include this alongside <menu.h> for
// Menu::chainStream<N> (multi-input fan-in). On AM5, MENU_INPUTS() (defined
// in <menu.h>, OneMenu's oneMenu/compat/am4.h) builds an oneMenu::InGroup
// directly and never needs Menu::chainStream. This header exists only so
// unmodified AM4 sketches keep compiling without editing their #include
// block. Not yet needed by any example verified against this branch
// (Blink/Button use a single input device) — if a real chainStream<N> use
// site shows up, it needs Menu::chainStream ported for real, not stubbed.
#pragma once

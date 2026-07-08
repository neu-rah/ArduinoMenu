/* -*- C++ -*- */
// AM5 stub: real AM4 sketches include this alongside <menu.h> to get
// Menu::serialOut + the SERIAL_OUT() macro. On AM5, <menu.h> (OneMenu's
// oneMenu/compat/am4.h) already defines SERIAL_OUT() directly — it never
// needs a Menu::serialOut class. This header exists only so unmodified AM4
// sketches keep compiling without editing their #include block.
#pragma once

/* -*- C++ -*- */
#pragma once

#include <streamFlow.h>//https://github.com/neu-rah/streamFlow
#include <menu.h>
#include <menu/IO/serialOut.h>

using namespace Menu;

//describing an output -----------------------------------------
//MenuOutCap - top level adapter for menu output, wraps a type-level composition
//WrapTitle - type level block will format all titles with surrounding []
//SerialOutDev - an output device bound to a serial port (arduino)
using SerialOut=MenuOutCap<WrapTitle<SerialOutDev<Serial>>>;

//describing an option ------------------------------------
// Prompt - top level adapter for menu items, wraps a type-level composition
// asTitle - role description, its meaning is interpreted by
//           an inner output device/format/filter (output composition chain)
using Op=Prompt<asTitle<Text<Empty>>>;//option will be formatted as title

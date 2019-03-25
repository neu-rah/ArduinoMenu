/* -*- C++ -*- */
#pragma once

#include <iostream>
using namespace std;

#include "../../menu.h"
#include "../IO/consoleOut.h"

using namespace Menu;

//describing an output -----------------------------------------
using ConsoleOut=MenuOutCap<WrapTitle<ConsoleOutDef>>;

//describing an option ------------------------------------
using Op=Prompt<asTitle<Text<Empty>>>;//option will be formatted as title

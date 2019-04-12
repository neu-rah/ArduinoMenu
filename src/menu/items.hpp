#include "items.h"

//default activation returning "not changed" (false)
EmptyCmds<false> Empty::cmds;

//activation result returning "changed" (true)
//for object that handle activation (by default we want it)
//this objects can also return "not changed" by using the default
template<typename O,ActionHandler act>
EmptyCmds<true> Action<O,act>::cmds;

//handler for objects that wish to receive navigation commands
template<typename O>
ItemCmd<NavHandler<O>> NavHandler<O>::cmds;

#include "items.h"

EmptyCmds<false> Empty::cmds;

template<typename O,ActionHandler act>
EmptyCmds<true> Action<O,act>::cmds;

template<typename O>
ItemCmd<NavHandler<O>> NavHandler<O>::cmds;

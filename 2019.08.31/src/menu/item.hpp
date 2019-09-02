#include "item.h"
/**
* @file item.hpp
* @author Rui Azevedo
* @brief menu item members that have to be implemented outside the class body
*/

template<typename I>
NavAgent Empty<I>::activate() {return {NULL,&cmds}; }
template<typename I>
NavAgent Empty<I>::activateItem(idx_t) {return Empty<I>::activate();}

template<typename I>
EmptyCmds<false> Empty<I>::cmds;

template<typename I,ActionHandler act>
EmptyCmds<true> Action<I,act>::cmds;

//handler for objects that wish to receive navigation commands
template<typename O>
ItemCmd<NavHandler<O>> NavHandler<O>::cmds;

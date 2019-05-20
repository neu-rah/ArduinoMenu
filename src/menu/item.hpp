#include "item.h"

template<typename I>
NavAgent Empty<I>::activate() {return {NULL,&cmds}; }
template<typename I>
NavAgent Empty<I>::activateItem(idx_t) {return Empty<I>::activate();}

template<typename I>
EmptyCmds<false> Empty<I>::cmds;

template<typename I,ActionHandler act>
EmptyCmds<true> Action<I,act>::cmds;

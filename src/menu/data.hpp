#include "data.h"

using namespace AM5;

template<typename O,typename... OO>
StaticMenu<OO...> StaticMenu<O,OO...>::next;

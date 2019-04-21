// #include "nav.h"
using namespace AM5;

template<typename Menu, typename Out,typename O>
Menu Nav<Menu,Out,O>::menu;

template<typename Menu, typename Out,typename O>
Out Nav<Menu,Out,O>::rawOut;

// template<typename Menu, typename Out,typename O>
// using NavType=typename Nav<Menu,Out,O>::NavBase;

template<typename Menu, typename Out,typename O>
Nav<Menu,Out,O> Nav<Menu,Out,O>::nav;

template<typename O>
size_t PosDef<O>::at=0;

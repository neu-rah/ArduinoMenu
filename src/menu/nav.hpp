// #include "nav.h"
using namespace AM5;

template<typename Menu, typename Out,typename O,typename Printers>
Menu Nav<Menu,Out,O,Printers>::menu;

template<typename Menu, typename Out,typename O,typename Printers>
Out Nav<Menu,Out,O,Printers>::rawOut;

template<typename Menu, typename Out,typename O,typename Printers>
Nav<Menu,Out,O,Printers> Nav<Menu,Out,O,Printers>::nav;

template<typename O>
size_t PosDef<O>::at=0;

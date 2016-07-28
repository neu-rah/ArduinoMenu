#include "menu.h"

class menuNav {
public:
  menuNode& activeNode;
  menuOut& scr;//some form of menu output usually a screen
  Stream& dev;//input devices as a stream
  menuNav(menuNode &node,menuOut & scr,Stream & input):activeNode(node),scr(scr),dev(input) {}
};

class navNode {
public:
  menuNode focus;//the component receiving input
  navNode *prev;//the previous nav node
  bool canExit;//nav can exit (only effective on menus)
  int sel;//selected item (menus,choose,toggle)
  bool tunning;//fine tune field value
  //char ch;
  //T tmp;<-- template non-constant value, to use this here this class must be a template
  // and creation should be done by focus class
};

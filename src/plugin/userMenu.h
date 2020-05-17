/* -*- C++ -*- */
/********************
May 2020 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

www.r-site.net

user print menu with optional submenu
***/

#include "../menuDefs.h"

namespace Menu {

  // menu with no defined data source, user has to provide a printItem method based on index
  // menu has static size but user can oiverride sz() function, some info might be placed on flash
  class UserMenu:public menuNode {
  public:
    menu* subMenu=NULL;
    UserMenu(constMEM menuNodeShadow& shadow):menuNode(shadow) {}
    UserMenu(constMEM menuNodeShadow& shadow,menu& subMenu):menuNode(shadow),subMenu(&subMenu) {}
    UserMenu(constText* text,idx_t size,menu& subMenu,action a=noAction,eventMask e=noEvent,styles style=wrapStyle)
      :menuNode(text,size,NULL,a,e,style,(systemStyles)(_menuData|_canNav)),subMenu(&subMenu) {}

    prompt& operator[](idx_t i) const override {return *(prompt*)this;}
    virtual Used printItem(menuOut& out, int idx,int len)=0;
    Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t p) override {
      if(root.navFocus!=this) return menuNode::printTo(root,sel,out,idx,len,p);
      return idx<0?
        menuNode::printTo(root,sel,out,idx,len,p)://print the title
        printItem(out,out.tops[root.level]+idx,len);
    }
    void doNav(navNode& nav,navCmd cmd) {
      if(nav.root->navFocus==this&&cmd.cmd==enterCmd) {
        _trace(MENU_DEBUG_OUT<<"no sub levels! submenu:"<<(subMenu?"yes":"no")<<" level:"<<nav.root->level<<"\n");
        //just send the event, but do not open (as this menu is representing the item)
        // nav.root->path[nav.root->level-1].event(enterEvent);
        // nav.event(enterEvent);
        // if(enterEvent&&) operator()(enterEvent, nav, *this);
        nav.event(enterEvent);
        if(subMenu) {
          assert(nav.root->level<nav.root->maxDepth);
          nav.root->active().dirty=true;
          nav.root->level++;
          nav.root->navFocus=nav.root->node().target=subMenu;
          nav.root->node().sel=0;
        }
      } else menuNode::doNav(nav,cmd);
    }
  };

  //user menu to use std containers or other structure that provide a size() function
  //not tested yes, please provide feedback
  template<typename T>
  class StdMenu:UserMenu {
    T& src;
    StdMenu(constText* text,T& src,action a=noAction,eventMask e=noEvent,styles style=wrapStyle)
      :menuNode(text,0,NULL,a,e),src(src) {}
    StdMenu(constText* text,T& src,menu& subMenu,action a=noAction,eventMask e=noEvent,styles style=wrapStyle)
      :menuNode(text,0,NULL,subMenu,a,e),src(src) {}
    idx_t sz() const override {return src.size();}
  };
};

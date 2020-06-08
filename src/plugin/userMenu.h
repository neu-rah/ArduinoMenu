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
    const char*backTitle=NULL;
    UserMenu(constMEM menuNodeShadow& shadow,const char*backTitle=NULL)
      :menuNode(shadow),backTitle(backTitle) {}
    UserMenu(constMEM menuNodeShadow& shadow,menu& subMenu,const char*backTitle=NULL)
      :menuNode(shadow),subMenu(&subMenu),backTitle(backTitle) {}
    UserMenu(const char* text,idx_t size,menu& subMenu,action a=noAction,eventMask e=noEvent,styles style=wrapStyle)
      :menuNode(text,size,NULL,a,e,style,(systemStyles)(_menuData|_canNav)),subMenu(&subMenu) {}
    UserMenu(const char* text,idx_t size,const char*backTitle,menu& subMenu,action a=noAction,eventMask e=noEvent,styles style=wrapStyle)
      :menuNode(text,size,NULL,a,e,style,(systemStyles)(_menuData|_canNav)),subMenu(&subMenu),backTitle(backTitle) {}

      inline idx_t sz() const override {return menuNode::sz()+(backTitle?1:0);}

    prompt& operator[](idx_t i) const override {return *(prompt*)this;}
    virtual Used printItem(menuOut& out, int idx,int len)=0;
    Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t p) override {
      if(root.navFocus!=this) return menuNode::printTo(root,sel,out,idx,len,p);
      if(idx<0) return menuNode::printTo(root,sel,out,idx,len,p);//print the title
      if(backTitle&&idx==sz()-1) return out.printText(backTitle,len);
      trace(MENU_DEBUG_OUT<<"{"<<idx<<"}");
      return printItem(out,out.tops[root.level]+idx/*-(backTitle?1:0)*/,len);
    }
    void doNav(navNode& nav,navCmd cmd) {
      if(nav.root->navFocus==this) {
        trace(MENU_DEBUG_OUT<<" submenu:"<<(subMenu?"yes":"no")<<" level:"<<nav.root->level<<"\n");
        switch (cmd.cmd) {
          case selCmd:
          case idxCmd:
            trace(MENU_DEBUG_OUT<<"idxCmd "<<cmd.param<<endl);
            nav.sel=cmd.param;
          case enterCmd:
            _trace(MENU_DEBUG_OUT<<"enterCmd");
            if(backTitle&&nav.sel==sz()-1) nav.root->exit();
            else {
              nav.event(enterEvent);
              if(subMenu) {
                _trace(MENU_DEBUG_OUT<<"edit item");
                assert(nav.root->level<nav.root->maxDepth);
                nav.root->active().dirty=true;
                nav.root->level++;
                nav.root->navFocus=nav.root->node().target=subMenu;
                nav.root->node().sel=0;
              }
            }
            return;
          default:break;
        }
      }
      menuNode::doNav(nav,cmd);
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

#include "menuDefs.h"
using namespace Menu;

void menuIn::setFieldMode(bool) {}
bool menuIn::fieldMode() const {return false;}

size_t noInput::write(uint8_t) {return 0;}
int noInput::available() {return 0;}
int noInput::read() {return -1;}
int noInput::peek() {return -1;}

#ifdef MENU_ASYNC
  int StringStream::available() {return 0!=*src;}
  int StringStream::read() {return *src++;}
  int StringStream::peek() {return *src?*src:-1;}
  void StringStream::flush() {while(*src) src++;}
  size_t StringStream::write(uint8_t) {return 0;}
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// menuOut - base menu output device
//
////////////////////////////////////////////////////////////////////////////////
#if defined(MENU_DEBUG) || defined(MENU_ASYNC)
  #ifdef ESP8266
    template<typename T>
    menuOut& menuOut::operator<<(T o) {(*(Stream*)this)<<(o);return *this;}
  #endif
#endif

#ifdef MENU_ASYNC
  bool menuOut::isAsync() {return false;}
#endif

menuOut& menuOut::fill(
  int x1, int y1, int x2, int y2,char ch,
  colorDefs color,
  bool selected,
  status stat,
  bool edit
) {return *this;}

idx_t menuOut::printRaw(const char* at,idx_t len) {
  trace(MENU_DEBUG_OUT<<"menuOut::printRaw"<<endl);
  const char* p=at;
  uint8_t ch;
  for(int n=0;(ch=memByte(at++))&&(len==0||n<len);n++) {
    write(ch);
  }
  return at-p-1;
}

idx_t menuOut::printText(const char* at,idx_t len) {
  for(int n=0;at[n]&&(len==0||n<len);n++)
    write(at[n]);
  return len;
}

void menuOut::doNav(navCmd cmd,navNode &nav) {
  trace(MENU_DEBUG_OUT<<"menuOut::doNav"<<endl);
  panel p=panels[nav.root->level];
  idx_t t=top(nav)-1;
  idx_t sz=nav.target->sz();
  switch(cmd.cmd) {
    case scrlUpCmd:
      if (t) {
        t--;
        if (nav.sel>t+sz) nav.sel--;
      }
      break;
    case scrlDownCmd:
      if (t+p.h<sz) {
        t++;
        if (nav.sel<t) nav.sel++;
      }
      break;
    default: assert(false);
  }
}

void menuOut::setColor(colorDefs c,bool selected,status s,bool edit) {}
void menuOut::drawCursor(idx_t ln,bool selected,status stat,bool edit,idx_t panelNr) {
  setColor(cursorColor, selected, stat,edit);
  write(selected?(stat==disabledStatus? options->disabledCursor : options->selectedCursor):' ');
}
idx_t menuOut::startCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) {write(charEdit?">":"[");return 1;}
idx_t menuOut::endCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) {write(charEdit?"<":"]");return 1;}
idx_t menuOut::editCursor(navRoot& root,idx_t x,idx_t y,bool editing,bool charEdit,idx_t panelNr) {return 0;}
void menuOut::rect(idx_t panelNr,idx_t x,idx_t y,idx_t w,idx_t h,colorDefs c,bool selected,status stat,bool edit) {}
void menuOut::box(idx_t panelNr,idx_t x,idx_t y,idx_t w,idx_t h,colorDefs c,bool selected,status stat,bool edit) {}
#ifdef MENU_FMT_WRAPS
  result menuOut::fmtStart(prompt& target,fmtParts part,navNode &nav,idx_t idx) {return proceed;}
  result menuOut::fmtEnd(prompt& target,fmtParts part,navNode &nav,idx_t idx) {return proceed;}
#endif

#if defined(MENU_DEBUG) || defined(MENU_ASYNC)
menuOut& menuOut::operator<<(const prompt& p) {
  print_P(*this,p.getText());
  return *this;
}
#endif

void cursorOut::clearLine(idx_t ln,idx_t panelNr,colorDefs color,bool selected,status stat,bool edit) {
  setCursor(0,ln,panelNr);
  for(int n=0;n<maxX();n++) print(' ');
  setCursor(0,ln,panelNr);
}

menuOut& cursorOut::fill(
  int x1, int y1, int x2, int y2,char ch,
  colorDefs color,
  bool selected,
  status stat,
  bool edit
) {
  for(int r=y1;r<=y2;r++) {
    setCursor(x1,r);
    for(int c=x1;c<=x2;c++)
      write(ch);
  }
  return *this;
}

idx_t gfxOut::startCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) {
  if (charEdit) {
    rect(panelNr,  x-1,  y, 1, 1, bgColor, false, enabledStatus, false);
    setColor(fgColor,false,enabledStatus,false);
  }/* else
    box(panelNr,  x,  y, 1, 1, bgColor, false, enabledStatus, false);*/
  return 0;
}

idx_t gfxOut::endCursor(navRoot& root,idx_t x,idx_t y,bool charEdit,idx_t panelNr) {
  setColor(fgColor,true,enabledStatus,true);return 0;
}

idx_t gfxOut::editCursor(navRoot& root,idx_t x,idx_t y,bool editing,bool charEdit,idx_t panelNr) {
  //TODO: next version, this furntiosn should return an USED value (and avoid nasty x-1)
  if (editing) box(panelNr,x-1,y);
  return 0;
}

Used outputsList::printMenu(navNode& nav) const {
  trace(MENU_DEBUG_OUT<<"outputsList::printMenu");
  for(int n=0;n<cnt;n++) {
    trace(MENU_DEBUG_OUT.write('.'));
    menuOut& o=*((menuOut*)memPtr(outs[n]));
    if ((o.style&(menuOut::rasterDraw))||(o.style&(menuOut::redraw))||nav.changed(o))
      o.printMenu(nav);
  }
  trace(MENU_DEBUG_OUT<<endl);
  clearChanged(nav);
  trace(MENU_DEBUG_OUT<<"outputsList::printMenu ended!"<<endl);
  return 0;
}

result outputsList::idle(idleFunc f,idleEvent e,bool idleChanged) {
  #ifdef MENU_DEBUG
    if (!f) MENU_DEBUG_OUT<<"idleFunc is NULL!!!"<<endl;
  #endif
  if (!f) return proceed;
  trace(MENU_DEBUG_OUT<<"idling on output list cnt:"<<cnt<<endl);
  for(int n=0;n<cnt;n++) {
    menuOut& o=*((menuOut*)memPtr(outs[n]));
    switch(e) {
      case idleStart:
        trace(MENU_DEBUG_OUT<<"idleStart"<<endl);
        if ((*f)(o,e)==proceed) {
          if (!(o.style&menuOut::redraw)) {
            result r=(*f)(o,idling);
            if (r==quit) return r;
          }
        } else return quit;
        break;
      case idling:
      trace(MENU_DEBUG_OUT<<"idling"<<endl);
        if (o.style&menuOut::redraw||(idleChanged&&(o.style&menuOut::minimalRedraw)))
          return (*f)(o,e);
        break;
      case idleEnd:
        trace(MENU_DEBUG_OUT<<"idle end"<<endl);
        result r=(*f)(o,e);
        if (r==quit) return r;
        break;
    }
  }
  return proceed;
}

void outputsList::refresh() {
  for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->drawn=NULL;
}
void outputsList::clearChanged(navNode& nav) const {
  for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->clearChanged(nav);
}
void outputsList::clear() {
  for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->clear();
}
// void outputsList::reset() {
//   for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->reset();
// }
void outputsList::doNav(navCmd cmd,class navNode &nav) {
  for(int n=0;n<cnt;n++) ((menuOut*)memPtr(outs[n]))->doNav(cmd,nav);
}

// draw a menu preview on a panel
void menuOut::previewMenu(navRoot& root,menuNode& menu,idx_t panelNr) {
  trace(MENU_DEBUG_OUT<<"menuOut::previewMenu"<<endl);
  // #ifdef MENU_FMT_WRAPS
  //   fmtStart(fmtBody,root.node());
  // #endif
  setColor(fgColor,false);
  if (menu.has(_asPad)) {
    for(int i=0;i<menu.sz();i++) {
      prompt& p=menu[i];
      #ifdef MENU_FMT_WRAPS
        fmtStart(p,fmtOp,root.node(),i);
      #endif
      #ifdef MENU_FMT_WRAPS
        fmtStart(p,fmtCursor,root.node(),i);
      #endif
      setColor(fgColor,false,p.enabled);
      drawCursor(i,false,p.enabled,false,panelNr);
      #ifdef MENU_FMT_WRAPS
        fmtEnd(p,fmtCursor,root.node(),i);
      #endif
      setColor(fgColor,false,p.enabled,false);
      #ifdef MENU_FMT_WRAPS
        fmtStart(p,fmtOpBody,root.node(),i);
      #endif
      p.printTo(root,false,*this,i,panels[panelNr].w,panelNr);
      #ifdef MENU_FMT_WRAPS
        fmtEnd(p,fmtOpBody,root.node(),i);
      #endif
      #ifdef MENU_FMT_WRAPS
        fmtEnd(p,fmtOp,root.node(),i);
      #endif
    }
  } else {
    clear(panelNr);
    setCursor(0,0,panelNr);
    for(idx_t i=0;i<maxY(panelNr);i++) {
      if (i>=menu.sz()) break;
      prompt& p=menu[i];
      clearLine(i,panelNr,bgColor,false,p.enabled);
      setCursor(0,i,panelNr);
      setColor(fgColor,false,p.enabled);
      drawCursor(i,false,p.enabled,false,panelNr);
      setColor(fgColor,false,p.enabled,false);
      p.printTo(root,false,*this,i,panels[panelNr].w,panelNr);
    }
  }
  // #ifdef MENU_FMT_WRAPS
  //   fmtEnd(fmtBody,root.node());
  // #endif
}

//determin panel number here and distribute menu and previews among the panels
Used menuOut::printMenu(navNode &nav) {
  trace(MENU_DEBUG_OUT<<"menuOut::printMenu(navNode &nav)"<<endl);
  menuNode& focus=nav.root->active();
  int lvl=nav.root->level;
  if (focus.parentDraw()) lvl--;
  navNode& nn=nav.root->path[lvl];
  int k=(lvl<panels.sz-1)?lvl:panels.sz-1;
  if ((style&usePreview)&&k) k--;
  for(int i=0;i<k;i++) {
    navNode &n=nav.root->path[lvl-k+i];
    if (!(/*n.has(_asPad)&&*/(style&minimalRedraw)&&panels.nodes[i]==&n)) {
      previewMenu(*nav.root,*n.target,i);
      panels.nodes[i]=&n;
    }
  }
  panels.cur=k;
  printMenu(nn,k);
  panels.nodes[k]=&nav;//for cleaning purposes
  for(int i=k+2;i<panels.sz;i++) if (panels.nodes[i]) {
    clear(i);
    panels.nodes[i]=NULL;
  }
  return 0;
}

// generic (menuOut) print menu on a panel
// this function emits format messages
// to be handler by format wrappers
Used menuOut::printMenu(navNode &nav,idx_t panelNr) {
  trace(MENU_DEBUG_OUT<<"menuOut::printMenu(navNode &nav,idx_t panelNr)"<<endl);
  if (!(nav.root->navFocus->has((systemStyles)(_parentDraw|_menuData)))) {
    //on this case we have a navTarget object that draws himself
    if (nav.root->navFocus->changed(nav,*this,false))
      nav.root->navFocus->printTo(*nav.root,true,*this,nav.sel,maxX(panelNr),panelNr);
  } else {
    idx_t topi=nav.root->level;
    if(topi&&nav.root->active().parentDraw()) topi--;
    idx_t ot=tops[topi];
    bool asPad=nav.target->has(_asPad);
    idx_t st
      =((nav.target->style()&showTitle)||(nav.root->showTitle&&(!(nav.target->style()&noTitle))))
      &&!(asPad||(maxY(panelNr)<2));//do not use titles on single line devices!
    if (!nav.target->parentDraw()) {
      if (pageScroll)
        tops[topi]=nav.sel-(nav.sel%(pageScroll?panels[panelNr].maxY()-st:1));
      else {
        while(nav.sel+st>=(tops[topi]+maxY(panelNr))) tops[topi]++;
        while(nav.sel<tops[topi]||(tops[topi]&&((nav.sz()-tops[topi])<maxY(panelNr)-st)))
          tops[topi]--;
      }
    }
    bool all=(style&redraw)
      ||(tops[topi]!=ot)
      ||(drawn!=nav.target)
      ||(panels.nodes[panelNr]!=&nav);
    if (!(all||(style&minimalRedraw))) {//non minimal draw will redraw all if any change
      trace(MENU_DEBUG_OUT<<"request full draw "<<style<<endl);
      all|=nav.target->changed(nav,*this);
    }
    all|=nav.target->dirty;
    if (!(all||(style&minimalRedraw))) return 0;
        panel pan=panels[panelNr];

    //-----> panel start
    trace(MENU_DEBUG_OUT<<"panel start"<<endl);
    bool titleChanged=st||nav.target->changed(nav,*this,false);
    #ifdef MENU_FMT_WRAPS
      fmtStart(*nav.target,fmtPanel,nav);
    #endif
    if (all||titleChanged) {
      trace(MENU_DEBUG_OUT<<"clear title "<<titleChanged<<endl);
      trace(MENU_DEBUG_OUT<<"all:"<<all<<" panelNr:"<<panelNr<<endl);
      trace(MENU_DEBUG_OUT<<"{x:"<<pan.x<<" y:"<<pan.y<<" w:"<<pan.w<<" h:"<<pan.h<<"}"<<endl);
      if (all&&!asPad) clear(panelNr);
      if (st||asPad) {
        ///------> titleStart
        trace(MENU_DEBUG_OUT<<"title start"<<endl);
        #ifdef MENU_FMT_WRAPS
          fmtStart(*nav.target,fmtTitle,nav,-1);
        #endif
        if (!asPad) {
          setColor(titleColor,false);
          clearLine(0,panelNr,titleColor);
          setColor(titleColor,true);
          setCursor(0,0,panelNr);
          //print('[');
          nav.target->printTo(*nav.root,true,*this,-1,pan.w-(asPad?1:2),panelNr);
        }
        ///<----- titleEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(*nav.target,fmtTitle,nav,-1);
        #endif
        if (asPad) {
          #ifdef MENU_FMT_WRAPS
            fmtStart(*nav.target,fmtCursor,nav,-1);
          #endif
          print(":");
          #ifdef MENU_FMT_WRAPS
            fmtEnd(*nav.target,fmtCursor,nav,-1);
          #endif
        }
      }
    }
    //------> bodyStart
    trace(MENU_DEBUG_OUT<<"body start all:"<<all<<endl);
    #ifdef MENU_FMT_WRAPS
      fmtStart(*nav.target,fmtBody,nav);
    #endif
    idx_t top=asPad?0:tops[topi];
    // idx_t idxCnt=top;
    // idx_t exitCnt=0;
    for(idx_t i=0;asPad||(i<maxY(panelNr)-st);i++) {
      int ist=i+st;
      if (i+top>=nav.sz()) break;
      prompt& p=nav[i+top];
      idx_t len=pan.w;
      if (all||p.changed(nav,*this,false)) {
        //-------> opStart
        #ifdef MENU_FMT_WRAPS
          fmtStart(p,fmtOp,nav,i);
        #endif
        bool selected=nav.sel==i+top;
        bool ed=nav.target==&p;
        //-----> idxStart
        #ifdef MENU_FMT_WRAPS
          fmtStart(p,fmtIdx,nav,i);
        #endif
        if (!asPad) {//pad MUST be able of clearing part of a line!!!!!
          clearLine(ist,panelNr,bgColor,selected,p.enabled);//<-- THIS IS DEPENDENT OF A DROP MENU!!!!
          setCursor(0,ist,panelNr);//<-- THIS IS MAKING A DROP MENU!!!!
        }
        setColor(fgColor,selected,p.enabled,ed);
        //should we use accels on pads?
        if (nav.root->useAccel&&(!asPad)&&(drawNumIndex&style)) {//<-- NO INDEX FOR PADS
          char a=top+i+'1';
          print('[');
          // if (p.is(_Exit)) {//this would require an enourmous confusiuon on keyboard accels
          //   print(exitCnt?'-':'0');
          //   exitCnt++;
          // } else {
            print(a<='9'?a:'-');
          //   idxCnt++;
          // }
          print(']');
          len-=3;
        }
        //<------idxEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(p,fmtIdx,nav,i);
        #endif
        //------> cursorStart
        #ifdef MENU_FMT_WRAPS
          fmtStart(p,fmtCursorOpen,nav,i);
          // fmtStart(p,fmtCursor,nav,i);
        #endif
        if (asPad&&selected) print("[");
        else drawCursor(ist,selected,p.enabled,ed,panelNr);//assuming only one character
        //<------ cursorEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(p,fmtCursorOpen,nav,i);
          // fmtEnd(p,fmtCursor,nav,i);
        #endif
        len--;
        //---->opBodyStart
        #ifdef MENU_FMT_WRAPS
          fmtStart(p,fmtOpBody,nav,i);
        #endif
        setColor(fgColor,selected,p.enabled,ed);
        if (len>0) len=p.printTo(*nav.root,selected,*this,i,len,panelNr);
        if (len>0) {
          if (asPad) {
            #ifdef MENU_FMT_WRAPS
              // fmtStart(p,fmtCursor,nav,i);
              fmtStart(p,fmtCursorClose,nav,i);
            #endif
            print(selected?"]":"");
            #ifdef MENU_FMT_WRAPS
              fmtEnd(p,fmtCursorClose,nav,i);
              // fmtEnd(p,fmtCursor,nav,i);
            #endif
            len--;
          }
        }
        //<---opBodyEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(p,fmtOpBody,nav,i);
        #endif
        if (selected&&panels.sz>panelNr+1) {
          if(p.isMenu()&&!p.asPad()) {
            //----->  previewStart
            previewMenu(*nav.root,*(menuNode*)&p,panelNr+1);
            panels.nodes[panelNr+1]=&nav;
            //<----  previewEnd
          } else if (panels.nodes[panelNr+1]) clear(panelNr+1);
        }
        //opEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(p,fmtOp,nav,i);
        #endif
      }
    }
  }
  //<-----bodyEnd
  #ifdef MENU_FMT_WRAPS
    fmtEnd(*nav.target,fmtBody,nav,-1);
  #endif
  drawn=nav.target;
  //lastSel=nav.sel;
  //<---- panel end
  #ifdef MENU_FMT_WRAPS
    fmtEnd(*nav.target,fmtPanel,nav,-1);
  #endif
  trace(MENU_DEBUG_OUT<<"ENDING menuOut::printMenu(navNode &nav,idx_t panelNr)"<<endl);
  return 0;
}

void menuOut::clearChanged(navNode &nav) {
  nav.target->clearChanged(nav,*this,true);
}

idx_t& menuOut::top(navNode& nav) const {return tops[nav.root->level];}

void cursorOut::clear(idx_t panelNr) {
  const panel p=panels[panelNr];
  fill(p.x,p.y,p.x+p.w-1,p.y+p.h-1);
  setCursor(0,0,panelNr);
  panels.nodes[panelNr]=NULL;
}

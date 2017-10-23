#include "menu.h"
using namespace Menu;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// menuOut - base menu output device
//
////////////////////////////////////////////////////////////////////////////////
idx_t menuOut::printRaw(const char* at,idx_t len) {
  trace(Serial<<"menuOut::printRaw"<<endl);
  const char* p=at;
  uint8_t ch;
  for(int n=0;(ch=*(at++))&&(len==0||n<len);n++) {
    write(ch);
  }
  return at-p;
}

void menuOut::doNav(navCmd cmd,navNode &nav) {
  trace(Serial<<"menuOut::doNav"<<endl);
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

#ifdef DEBUG
menuOut& menuOut::operator<<(const prompt& p) {
  print_P(*this,p.getText());
  return *this;
}
#endif

Used outputsList::printMenu(navNode& nav) const {
  trace(Serial<<"outputsList::printMenu"<<endl);
  for(int n=0;n<cnt;n++) {
    menuOut& o=*((menuOut*)memPtr(outs[n]));
    if (nav.changed(o)||(o.style&(menuOut::rasterDraw)))
      o.printMenu(nav);
  }
  clearChanged(nav);
  trace(Serial<<"outputsList::printMenu ended!"<<endl);
  return 0;
}

// draw a menu preview on a panel
void menuOut::previewMenu(navRoot& root,menuNode& menu,idx_t panelNr) {
  trace(Serial<<"menuOut::previewMenu"<<endl);
  setColor(fgColor,false);
  if (menu.has(_asPad)) {
    for(int i=0;i<menu.sz();i++) {
      prompt& p=menu[i];
      setColor(fgColor,false,p.enabled);
      drawCursor(i,false,p.enabled,false,panelNr);
      setColor(fgColor,false,p.enabled,false);
      p.printTo(root,false,*this,i,panels[panelNr].w,panelNr);
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
}

//determin panel number here and distribute menu and previews among the panels
Used menuOut::printMenu(navNode &nav) {
  trace(Serial<<"menuOut::printMenu(navNode &nav)"<<endl);
  menuNode& focus=nav.root->active();
  int lvl=nav.root->level;
  if (focus.parentDraw()) lvl--;
  navNode& nn=nav.root->path[lvl];
  int k=min(lvl,panels.sz-1);
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
  trace(Serial<<"menuOut::printMenu(navNode &nav,idx_t panelNr)"<<endl);
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
      while(nav.sel+st>=(tops[topi]+maxY(panelNr))) tops[topi]++;
      while(nav.sel<tops[topi]||(tops[topi]&&((nav.sz()-tops[topi])<maxY(panelNr)-st)))
        tops[topi]--;
    }
    bool all=(style&redraw)
      ||(tops[topi]!=ot)
      ||(drawn!=nav.target)
      ||(panels.nodes[panelNr]!=&nav);
    if (!(all||(style&minimalRedraw))) //non minimal draw will redraw all if any change
      all|=nav.target->changed(nav,*this);
    all|=nav.target->dirty;
    if (!(all||(style&minimalRedraw))) return 0;
    panel pan=panels[panelNr];

    //-----> panel start
    trace(Serial<<"panel start"<<endl);
    bool titleChanged=st||nav.target->changed(nav,*this,false);
    #ifdef MENU_FMT_WRAPS
      fmtStart(fmtPanel,nav);
    #endif
    if (all||titleChanged) {
      trace(Serial<<"all:"<<all<<" panelNr:"<<panelNr<<endl);
      trace(Serial<<"{x:"<<pan.x<<" y:"<<pan.y<<" w:"<<pan.w<<" h:"<<pan.h<<"}"<<endl);
      if (all&&!asPad) clear(panelNr);
      if (st||asPad) {
        ///------> titleStart
        trace(Serial<<"title start"<<endl);
        #ifdef MENU_FMT_WRAPS
          fmtStart(fmtTitle,nav,-1);
        #endif
        if (!asPad) {
          setColor(titleColor,false);
          clearLine(0,panelNr);
          setColor(titleColor,true);
          setCursor(0,0,panelNr);
          //print('[');
          nav.target->printTo(*nav.root,true,*this,-1,pan.w-(asPad?1:2),panelNr);
        }
        if (asPad) print(":");
        //else print(']');
        ///<----- titleEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(fmtTitle,nav,-1);
        #endif
      }
    }
    //------> bodyStart
    trace(Serial<<"body start"<<endl);
    #ifdef MENU_FMT_WRAPS
      fmtStart(fmtBody,nav);
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
          fmtStart(fmtOp,nav,i);
        #endif
        bool selected=nav.sel==i+top;
        bool ed=nav.target==&p;
        //-----> idxStart
        #ifdef MENU_FMT_WRAPS
          fmtStart(fmtIdx,nav,i);
        #endif
        if (!asPad) {//pad MUST be able of clearing part of a line!!!!!
          clearLine(ist,panelNr,bgColor,selected,p.enabled);//<-- THIS IS DEPENDENT OF A DROP MENU!!!!
          setCursor(0,ist,panelNr);//<-- THIS IS MAKING A DROP MENU!!!!
        }
        setColor(fgColor,selected,p.enabled,ed);
        //should we use accels on pads?
        if ((!asPad)&&(drawNumIndex&style)) {//<-- NO INDEX FOR PADS
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
          fmtEnd(fmtIdx,nav,i);
        #endif
        //------> cursorStart
        #ifdef MENU_FMT_WRAPS
          fmtStart(fmtCursor,nav,i);
        #endif
        if (asPad&&selected) print("[");
        else drawCursor(ist,selected,p.enabled,ed,panelNr);//assuming only one character
        //<------ cursorEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(fmtCursor,nav,i);
        #endif
        len--;
        //---->opBodyStart
        #ifdef MENU_FMT_WRAPS
          fmtStart(fmtOpBody,nav,i);
        #endif
        setColor(fgColor,selected,p.enabled,ed);
        if (len>0) len=p.printTo(*nav.root,selected,*this,i,len,panelNr);
        if (len>0) {
          if (asPad) {
            print(selected?"]":"");
            len--;
          }
        }
        //<---opBodyEnd
        #ifdef MENU_FMT_WRAPS
          fmtEnd(fmtOpBody,nav,i);
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
          fmtEnd(fmtOp,nav,i);
        #endif
      }
    }
  }
  //<-----bodyEnd
  #ifdef MENU_FMT_WRAPS
    fmtEnd(fmtBody,nav,-1);
  #endif
  drawn=nav.target;
  //lastSel=nav.sel;
  //<---- panel end
  #ifdef MENU_FMT_WRAPS
    fmtEnd(fmtPanel,nav,-1);
  #endif
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

#include "menu.h"
#ifdef DEBUG
  #include <AnsiStream.h>
#endif
using namespace Menu;

result Menu::doNothing() {return proceed;}
result Menu::doExit() {return quit;}
action Menu::noAction(doNothing);

//this is for idle (menu suspended)
result Menu::inaction(menuOut& o,idleEvent) {return proceed;}

void prompt::printTo(navRoot &root,bool sel,menuOut& out) {out<<*this;}

menuOut& menuOut::operator<<(const prompt& p) {
  print_P(*this,(const char *)memPtr(p.shadow->text));
  return *this;
}

void menuOut::clearChanged(navNode &nav) {
  nav.target->dirty=false;
  for(idx_t i=0;i<maxY();i++) {
    if (i+top>=nav.sz()) break;
    nav[i+top].dirty=false;
  }
}

// draw a menu preview on a panel
void menuOut::previewMenu(navRoot& root,menuNode& menu,idx_t panelNr) {
  clear(panelNr);
  setColor(fgColor,false);
  setCursor(0,0,panelNr);
  for(idx_t i=0;i<maxY(panelNr);i++) {
    if (i>=menu.sz()) break;
    prompt& p=menu[i];
    clearLine(i,panelNr,bgColor,false,p.enabled);
    setCursor(0,i,panelNr);
    setColor(fgColor,false,p.enabled);
    drawCursor(i,false,p.enabled,false,panelNr);
    setColor(fgColor,false,p.enabled,false);
    p.printTo(root,false,*this);
  }
}

//determin panel number here and distribute menu and previews among the panels
void menuOut::printMenu(navNode &nav) {
  menuNode& focus=nav.root->active();
  int lvl=nav.root->level;
  if (focus.sysStyles()&_parentDraw) lvl--;
  int k=min(lvl,panels.sz-1);
  if (usePreview&&k) k--;
  for(int i=0;i<k;i++) {
    navNode &n=nav.root->path[lvl-k+i];
    if (!(minimalRedraw&&panels.nodes[i]==n.target)) {
      previewMenu(*nav.root,*n.target,i);
      panels.nodes[i]=n.target;
    }
  }
  printMenu(nav,k);
  panels.nodes[k]=nav.target;//for cleaning purposes
  for(int i=k+2;i<panels.sz;i++) if (panels.nodes[i]) {
    clear(i);
    panels.nodes[i]=NULL;
  }
}

// generic (menuOut) print menu on a panel
void menuOut::printMenu(navNode &nav,idx_t panelNr) {
  idx_t ot=top;
  idx_t st=(nav.root->showTitle&&(maxY(panelNr)>1))?1:0;//do not use titles on single line devices!
  while(nav.sel+st>=(top+maxY(panelNr))) {top++;}
  while(nav.sel<top||(top&&((nav.sz()-top)<maxY(panelNr)-st))) {top--;}
  bool all=redraw||(top!=ot)||drawn!=nav.target;
  if (!(all||minimalRedraw))
    for(idx_t i=0;i<maxY(panelNr)-st;i++) {
      if (all||i+top>=nav.sz()) break;
      else all|=nav[i+top].changed(nav,*this);
    }
  all|=nav.target->dirty;
  if (all) {
    clear(panelNr);
    if (st) {
      setColor(titleColor,false);
      clearLine(0,panelNr);
      setColor(titleColor,true);
      setCursor(0,0,panelNr);
      *this<<'['<<*(prompt*)nav.target<<']';
    }
  }
  //bool any=all;
  for(idx_t i=0;i<maxY(panelNr)-st;i++) {
    int ist=i+st;
    if (i+top>=nav.sz()) break;
    prompt& p=nav[i+top];
    if (all||p.changed(nav,*this)) {
      //any=true;
      bool selected=nav.sel==i+top;
      bool ed=nav.target==&p;
      clearLine(ist,panelNr,bgColor,selected,p.enabled);
      setCursor(0,ist,panelNr);
      setColor(fgColor,selected,p.enabled,ed);
      if (drawNumIndex) {
        char a=top+i+'1';
        *this<<'['<<(a<='9'?a:'-')<<']';
      }
      drawCursor(ist,selected,p.enabled,ed,panelNr);
      setColor(fgColor,selected,p.enabled,ed);
      p.printTo(*nav.root,selected,*this);
      if (selected&&panels.sz>panelNr+1) {
        if(p.isMenu()) {
          previewMenu(*nav.root,*(menuNode*)&p,panelNr+1);
          panels.nodes[panelNr+1]=(menuNode*)&p;
        } else if (panels.nodes[panelNr+1]) clear(panelNr+1);
      }
    }
  }
  drawn=nav.target;
}

navRoot* navNode::root=NULL;

bool menuNode::changed(const navNode &nav,const menuOut& out) {
  if (nav.target!=this) return dirty;
  if (dirty) return true;
  for(int i=0;i<out.maxY();i++) {
    if (i+out.top>=nav.sz()) break;
    if (operator[](i).changed(nav,out)) return true;
  }
  return false;
}

//aux function, turn input character into navigation command
navCmds navNode::navKeys(char ch) {
  for(uint8_t i=0;i<sizeof(options->navCodes)/sizeof(navCode);i++)
    if (options->navCodes[i].ch==ch) return options->navCodes[i].cmd;
  return noCmd;
}

void navTarget::navigate(navNode& nav,Stream& in) {
  nav.doNavigation(in.read());
}

//generic navigation (aux function)
void navNode::doNavigation(char ch) {
  idx_t osel=sel;
  navCmds cmd=navKeys(ch);
  switch(cmd) {
    case downCmd:
      sel--;
      if (sel<0) {if(wrap()) sel=sz()-1; else sel=0;}
      break;
    case upCmd:
      sel++;
      if (sel>=sz()) {if(wrap()) sel=0; else sel=sz()-1;}
      break;
    case escCmd:
      assert(root);
      root->exit();
      break;
    case noCmd:
    default: break;
  }
  if (strchr(numericChars,ch)) {
    char at=ch-'1';
    if (at>=0&&at<sz()) {
      sel=at;
      cmd=enterCmd;
    }
  }
  if(osel!=sel) {
    if (target->sysStyles()&(_parentDraw|_isVariant))
      target->dirty=true;
    else {
      operator[](osel).dirty=true;
      operator[](sel).dirty=true;
    }
    //send focus In/Out events
    event(blurEvent,osel);
    event(focusEvent,sel);
  }
  if(cmd==enterCmd) {
    assert(root);
    root->enter();
  }
}

result navNode::event(eventMask e,idx_t i) {
  prompt& p=operator[](i);
  eventMask m=(eventMask)memByte(&p.shadow->promptShadow::events);
  eventMask me=(eventMask)(e&m);
  if (me) {
    return p(e,*this,p);
  }
  return proceed;
}

result navNode::sysEvent(eventMask e,idx_t i) {
  prompt& p=operator[](i);
  return p(e,*this,p);
}

void navRoot::doInput() {
  while ((!sleepTask)&&in.available())
    navFocus->navigate(node(),in);
}

void navRoot::doOutput() {
  if (sleepTask) {
    if (options->getCmdChar(enterCmd)==in.read()) idleOff();
    else out.idle(sleepTask,idling);
  } else if (!(sleepTask)) printMenu();
}

void navRoot::poll() {
  doInput();
  doOutput();
}

result maxDepthError(menuOut& o,idleEvent e) {
  o<<F("Error: maxDepth reached!\n\rincrease maxDepth on your scketch.");
  return proceed;
}

void navRoot::enter() {
  if (
    selected().enabled
    &&selected().sysHandler(activateEvent,node(),selected())==proceed
  ) {
    prompt& sel=selected();
    bool canNav=sel.canNav();
    bool isMenu=sel.isMenu();
    result go=node().event(enterEvent);//item event sent here
    if (go==proceed&&isMenu&&canNav) {
      if (level<maxDepth) {
        active().dirty=true;
        menuNode* dest=(menuNode*)&selected();
        level++;
        node().target=dest;
        node().sel=0;
        active().dirty=true;
        sel.sysHandler(enterEvent,node(),selected());
      } else idleOn(maxDepthError);
    } else if (go==quit&&!selected().isMenu()) exit();
    if (canNav) {
      navFocus=(navTarget*)&sel;
      navFocus->dirty=true;
    }
  }
}

void navRoot::exit() {
  node().event(exitEvent,node().sel);
  navFocus->dirty=true;
  if (navFocus->isMenu()) {
    if (level) level--;
    else {
      idleOn(idleTask);
    }
  }
  active().dirty=true;
  navFocus=&active();
}

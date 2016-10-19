#include "menu.h"
#ifdef DEBUG
  #include <AnsiStream.h>
#endif
using namespace Menu;

config Menu::options;

result Menu::doNothing() {return proceed;}
result Menu::doExit() {return quit;}
action Menu::noAction(doNothing);

//this is for idle (menu suspended)
result Menu::inaction(menuOut& o,idleEvent) {return proceed;}

void prompt::printTo(idx_t i,navNode &nav,menuOut& out) {out<<*this;}

menuOut& menuOut::operator<<(const prompt& p) {
  print_P(*this,(const char *)memPtr(p.shadow->text));
  return *this;
}

void menuOut::clearChanged(navNode &nav) {
  nav.target->dirty=false;
  for(idx_t i=0;i<maxY;i++) {
    if (i+top>=nav.sz()) break;
    nav[i+top].dirty=false;
  }
}

void menuOut::printMenu(navNode &nav,idx_t panelNr=0) {
  idx_t ot=top;
  idx_t st=(nav.root->showTitle&&maxY>1)?1:0;//do not use titles on single line devices!
  while(nav.sel+st>=(top+maxY)) top++;
  while(nav.sel<top||(top&&nav.sel+top<maxY-st)) top--;
  bool all=redraw;
  if (!(all||minimalRedraw))
    for(idx_t i=0;i<maxY-st;i++)
      if (all) break; else all|=nav[i+top].changed(nav,*this);
  all|=(top!=ot)||nav.target->dirty||drawn!=nav.target;
  if (all) {
    clear();
    if (st) {
      setColor(titleColor,false);
      clearLine(0);
      setColor(titleColor,true);
      setCursor(0,0);
      *this<<"["<<*(prompt*)nav.target<<"]";
    }
  }
  for(idx_t i=0;i<maxY-st;i++) {
    int ist=i+st;
    if (i+top>=nav.sz()) break;
    prompt& p=nav[i+top];
    if (all||p.changed(nav,*this)) {
      bool selected=nav.sel==i+top;
      bool ed=nav.target==&p;
      clearLine(ist,bgColor,selected,p.enabled,ed);
      setCursor(0,ist);
      drawCursor(ist,selected,p.enabled,ed);
      setColor(fgColor,selected,p.enabled,ed);
      p.printTo(i+top,nav,*this);
    }
  }
  drawn=nav.target;
}

navRoot* navNode::root=NULL;

bool menuNode::changed(const navNode &nav,const menuOut& out) {
  if (nav.target!=this) return dirty;
  if (dirty) return true;
  for(int i=0;i<out.maxY;i++) {
    if (i+out.top>=nav.sz()) break;
    if (operator[](i).changed(nav,out)) return true;
  }
  return false;
}

//aux function, turn input character into navigation command
navCmds navNode::navKeys(char ch) {
  for(uint8_t i=0;i<sizeof(options.navCodes)/sizeof(config::navCode);i++)
    if (options.navCodes[i].ch==ch) return options.navCodes[i].cmd;
  return noCmd;
}

void navTarget::navigate(navNode& nav,char ch,Stream& in) {
  nav.doNavigation(ch,in);
}

//generic navigation (aux function)
void navNode::doNavigation(char ch,Stream& in) {
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
  if ((!sleepTask)&&in.available())
    navFocus->navigate(node(),in.read(),in);
}

void navRoot::doOutput() {
  if (sleepTask) {
    if (options.getCmdChar(enterCmd)==in.read()) idleOff();
    else out.idle(sleepTask,idling);
  } else if (!(sleepTask)) printMenu();
}

void navRoot::poll() {
  doInput();
  doOutput();
  /*if (sleepTask) {
    if (options.getCmdChar(enterCmd)==in.read()) idleOff();
    else out.idle(sleepTask,idling);
  } else {
    if (in.available()) navFocus->navigate(node(),in.read(),in);
    if (!(sleepTask)) printMenu();
  }*/
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
      }
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
      idleOn(options.idleTask);
    }
  }
  active().dirty=true;
  navFocus=&active();
}

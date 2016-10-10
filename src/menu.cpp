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
result Menu::inaction(idleEvent) {return proceed;}

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

void gfxOut::printMenu(navNode &nav) {
  idx_t ot=top;
  while(nav.sel>=top+maxY) top++;
  while(nav.sel<top) top--;
  if (top!=lastTop||lastSel!=nav.sel||top!=ot||drawn!=nav.target||nav.target->changed(nav,*this)) {
    for(idx_t i=0;i<maxY;i++) {
      if (i+top>=nav.sz()) break;
      prompt& p=nav[i+top];
      bool selected=nav.sel==i+top;
      clearLine(i,bgColor,selected,p.enabled);
      drawCursor(i,selected,p.enabled);
      setColor(fgColor,selected,p.enabled);
      setCursor(posX,posY+i);
      p.printTo(i,nav,*this);
    }
    lastTop=top;
    lastSel=nav.sel;
  }
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
  for(uint8_t i=0;i<sizeof(options.navCodes)/sizeof(config::navCode);i++) {
    if (options.navCodes[i].ch==ch) return options.navCodes[i].cmd;
  }
  return noCmd;
}

void navTarget::navigate(navNode& nav,char ch,Stream& in) {
  nav.doNavigation(ch,in);
}

//generic navigation (aux function)
void navNode::doNavigation(char ch,Stream& in) {
  idx_t osel=sel;
  //idx_t otop=out.top;
  navCmds cmd=navKeys(ch);
  //Serial<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  //Serial<<ANSI::xy(0,20)<<"doNavigation "<<cmd<<" sel:"<<sel;
  switch(cmd) {
    case downCmd:
      sel--;
      if (sel<0) {if(wrap()) sel=sz()-1; else sel=0;}
      //if (sel<top) top=sel;
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
  //Serial<<ANSI::xy(0,21)<<"processed keys, sel:"<<sel;
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
      //Serial<<" setting dirty on "<<osel<<" and "<<sel;
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
  /*if(otop!=top) {
    target->prompt::dirty=true;
  }*/
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

void navRoot::poll() {
  if (suspended) {
    Serial<<"suspended"<<endl;
    if (in.available()&&in.read()==options.getCmdChar(enterCmd)) {
      options.idleTask(idleEnd);
      suspended=false;
    } else options.idleTask(idling);
  } else if (in.available()) {
    navFocus->navigate(node(),in.read(),in);
  }
  if (!suspended) printMenu();//previous actions can suspend the  menu
}

void navRoot::enter() {
  if (
    selected().enabled
    &&selected().sysHandler(activateEvent,node(),selected())==proceed
  ) {
    prompt& sel=selected();
    bool canNav=sel.canNav();
    bool isMenu=sel.isMenu();
    result go=node().event(enterEvent);
    if (go==proceed&&isMenu&&canNav) {
      if (level<maxDepth) {
        active().dirty=true;
        menuNode* dest=(menuNode*)&selected();
        level++;
        node().target=dest;
        //out.top=0;
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
  if (selected().shadow->events&&exitEvent)
    (*selected().shadow)(exitEvent,node(),selected());
  navFocus->dirty=true;
  if (navFocus->isMenu()) {
    if (level) level--;
    else {
      suspended=true;
      options.idleTask(idleStart);
    }
  }
  active().dirty=true;
  navFocus=&active();
}

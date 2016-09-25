#include "menu.h"
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

navRoot* navNode::root=NULL;

bool menuNode::changed(const navNode &nav,const menuOut& out) {
  Serial<<"menuNode "<<*(prompt*)this<<" changed?"<<endl;
  if (nav.target!=this) return dirty;
  if (dirty) return true;
  for(int i=0;i<out.maxY;i++) {
    if (i+nav.top>=nav.sz()) break;
    if (operator[](i).changed(nav,out)) return true;
  }
  Serial<<"NOT CHANGED!"<<endl;
  return false;
}

void menuOut::printMenu(navNode &nav) {
  if (nav.target->changed(nav,*this)) {
    *this<<"["<<*(prompt*)nav.target<<"]"<<endl;
    for(idx_t i=0;i<maxY;i++) {
      if (i+nav.top>=nav.sz()) break;
      *this<<"["<<i+1<<"]";
      prompt& p=nav[i+nav.top];
      write(i+nav.top==nav.sel?options.selectedCursor:' ');
      p.printTo(i,nav,*this);
      *this<<endl;
      p.dirty=false;
    }
    nav.target->prompt::dirty=false;
    for(int n=memStrLen((char*)memPtr(nav.target->shadow->text))+2;n;n--) *this<<"-";
    *this<<endl;
  }
}

//aux function, turn input character into navigation command
navCmds navNode::navKeys(char ch) {
  for(uint8_t i=0;i<sizeof(options.navCodes)/sizeof(config::navCode);i++) {
    if (options.navCodes[i].ch==ch) return options.navCodes[i].cmd;
  }
  return noCmd;
}

void navTarget::navigate(navNode& nav,char ch,Stream& in,menuOut& out) {
  nav.doNavigation(ch,in,out);
}

//generic navigation (aux function)
void navNode::doNavigation(char ch,Stream& in,menuOut& out) {
  idx_t osel=sel;
  idx_t otop=top;
  navCmds cmd=navKeys(ch);
  switch(cmd) {
    case downCmd:
      sel--;
      if (sel<0) {if(wrap()) sel=sz()-1; else sel=0;}
      //if (sel<top) top=sel;
      Serial<<"downCmd "<<sel<<" sz:"<<sz()<<endl;
      break;
    case upCmd:
      sel++;
      if (sel>=sz()) {if(wrap()) sel=0; else sel=sz()-1;}
      Serial<<"upCmd "<<sel<<" sz:"<<sz()<<endl;
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
    Serial<<"sel changed to "<<sel<<endl;
    if (target->sysStyles()&(_parentDraw|_isVariant))
      target->dirty=true;
    else {
      Serial<<"siblings dirty"<<endl;
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
  if(otop!=top) {
    target->prompt::dirty=true;
  }
}

result navNode::event(eventMask e,idx_t i) {
  prompt& p=operator[](i);
  eventMask m=(eventMask)memByte(&p.shadow->promptShadow::events);
  eventMask me=(eventMask)(e&m);
  if (me) {
    return p(e,*this,p,root->in,root->out);
  }
  return proceed;
}

result navNode::sysEvent(eventMask e,idx_t i) {
  prompt& p=operator[](i);
  return p(e,*this,p,root->in,root->out);
}

void navRoot::poll() {
  if (suspended) {
    Serial<<"suspended"<<endl;
    if (in.available()&&in.read()==options.getCmdChar(enterCmd)) {
      options.idleTask(idleEnd);
      suspended=false;
    } else options.idleTask(idling);
  } else if (in.available()) {
    navFocus->navigate(node(),in.read(),in,out);
  }
  if (!suspended) printMenu();//previous actions can suspend the  menu
}

void navRoot::enter() {
  if (
    selected().enabled
    &&selected().sysHandler(activateEvent,node(),selected(),in,out)==proceed
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
        node().top=0;
        node().sel=0;
        active().dirty=true;
        sel.sysHandler(enterEvent,node(),selected(),in,out);
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
    (*selected().shadow)(exitEvent,node(),selected(),in,out);
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

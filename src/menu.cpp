#include "menu.h"
using namespace Menu;

result Menu::doNothing() {return proceed;}
result Menu::doExit() {return quit;}
action Menu::noAction(doNothing);

//this is for idle (menu suspended)
result Menu::inaction(menuOut& o,idleEvent) {return proceed;}

idx_t prompt::printRaw(menuOut& out,idx_t len) const {
  return print_P(out,(const char *)memPtr(shadow->text),len);
}

idx_t prompt::printTo(navRoot &root,bool sel,menuOut& out,idx_t len) {return printRaw(out,len);}

idx_t menuOut::printRaw(const char* at,idx_t len) {
  const char* p=at;
  uint8_t ch;
  for(int n=0;(ch=*(at++))&&(len==0||n<len);n++) {
    write(ch);
  }
  return at-p;
}

void menuOut::doNav(navCmd cmd,navNode &nav) {
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

menuOut& menuOut::operator<<(const prompt& p) {
  print_P(*this,(const char *)memPtr(p.shadow->text));
  return *this;
}

void menuOut::clearChanged(navNode &nav) {
  //if (nav.target->dirty) Serial<<"clear dirty "<<*(prompt*)nav.target<<" sz:"<<nav.sz()<<endl;
  nav.target->dirty=false;
  for(idx_t i=0;i<maxY();i++) {
    if (i+tops[nav.root->level]>=nav.sz()) break;
    //if (nav[i+top].dirty) Serial<<"clear sub dirty "<<nav[i+top]<<endl;
    nav[i+tops[nav.root->level]].dirty=false;
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
    p.printTo(root,false,*this,panels[panelNr].w);
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
    if (!(minimalRedraw&&panels.nodes[i]==&n)) {
      previewMenu(*nav.root,*n.target,i);
      panels.nodes[i]=&n;
    }
  }
  panels.cur=k;
  printMenu(nav,k);
  panels.nodes[k]=&nav;//for cleaning purposes
  for(int i=k+2;i<panels.sz;i++) if (panels.nodes[i]) {
    clear(i);
    panels.nodes[i]=NULL;
  }
}

//bool menuOut::changed

// generic (menuOut) print menu on a panel
void menuOut::printMenu(navNode &nav,idx_t panelNr) {
  idx_t ot=tops[nav.root->level];
  idx_t st=(nav.root->showTitle&&(maxY(panelNr)>1))?1:0;//do not use titles on single line devices!
  while(nav.sel+st>=(tops[nav.root->level]+maxY(panelNr))) tops[nav.root->level]++;
  while(nav.sel<tops[nav.root->level]||(tops[nav.root->level]&&((nav.sz()-tops[nav.root->level])<maxY(panelNr)-st))) tops[nav.root->level]--;
  bool all=redraw
    ||(tops[nav.root->level]!=ot)
    ||(drawn!=nav.target)
    ||(panels.nodes[panelNr]!=&nav);
  //if (all) Serial<<"all 1"<<endl;
  if (!(all||minimalRedraw)) //non minimal draw will redraw all if any change
    all|=nav.target->changed(nav,*this);
  //if (all) Serial<<"all 2"<<endl;
  all|=nav.target->dirty;
  //if (all) Serial<<"all 3 "<<*(prompt*)nav.target<<endl;
  if (!(all||minimalRedraw)) return;
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
  panel pan=panels[panelNr];
  //if (all) Serial<<"all 4"<<endl;
  //bool any=all;
  for(idx_t i=0;i<maxY(panelNr)-st;i++) {
    int ist=i+st;
    if (i+tops[nav.root->level]>=nav.sz()) break;
    prompt& p=nav[i+tops[nav.root->level]];
    idx_t len=pan.w+1;
    if (all||p.changed(nav,*this,false)) {
      //any=true;
      bool selected=nav.sel==i+tops[nav.root->level];
      bool ed=nav.target==&p;
      clearLine(ist,panelNr,bgColor,selected,p.enabled);
      setCursor(0,ist,panelNr);
      setColor(fgColor,selected,p.enabled,ed);
      if (drawNumIndex) {
        char a=tops[nav.root->level]+i+'1';
        *this<<'['<<(a<='9'?a:'-')<<']';
        len-=3;
      }
      drawCursor(ist,selected,p.enabled,ed,panelNr);
      len--;
      setColor(fgColor,selected,p.enabled,ed);
      if (len>0) p.printTo(*nav.root,selected,*this,len);
      if (selected&&panels.sz>panelNr+1) {
        if(p.isMenu()) {
          //Serial<<"preview of:"<<p<<endl;
          previewMenu(*nav.root,*(menuNode*)&p,panelNr+1);
          //TODO: do we need preview pointers? panels.nodes[panelNr+1]=(menuNode*)&p;
        } else if (panels.nodes[panelNr+1]) clear(panelNr+1);
      }
    }
  }
  //if (any) Serial<<"printMenu "<<*(prompt*)nav.target<<endl;
  drawn=nav.target;
  //lastSel=nav.sel;
  //lastTop=top;
}

navRoot* navNode::root=NULL;

bool menuNode::changed(const navNode &nav,const menuOut& out,bool sub) {
  if (nav.target!=this) return dirty;
  if (dirty) return true;
  if (sub) for(int i=0;i<out.maxY();i++) {
    if (i+out.tops[nav.root->level]>=nav.sz()) break;
    if (operator[](i+out.tops[nav.root->level]).changed(nav,out,false)) return true;
  }
  return false;
}

//aux function, turn input character into navigation command
navCmd navNode::navKeys(char ch) {
  if (strchr(numericChars,ch))
    return navCmd(idxCmd,ch);
  for(uint8_t i=0;i<sizeof(options->navCodes)/sizeof(navCode);i++)
    if (options->navCodes[i].ch==ch) return options->navCodes[i].cmd;
  return noCmd;
}

void navTarget::doNav(navNode& nav,navCmd cmd) {nav.doNavigation(cmd);}

void navTarget::parseInput(navNode& nav,Stream& in) {doNav(nav,nav.navKeys(in.read()));}

//generic navigation (aux function)
navCmd navNode::doNavigation(navCmd cmd) {
  idx_t osel=sel;
  navCmd rCmd=cmd;
  switch(cmd.cmd) {
    /*case scrlUpCmd:
      if (!target->isVariant())
        root->out.doNav(cmd,*this);*/
    case downCmd:
      sel--;
      if (sel<0) {if(wrap()) sel=sz()-1; else sel=0;}
      break;
    /*case scrlDownCmd:
      if (!target->isVariant())
        root->out.doNav(cmd,*this);*/
    case upCmd:
      sel++;
      if (sel>=sz()) {if(wrap()) sel=0; else sel=sz()-1;}
      break;
    case escCmd:
      assert(root);
      rCmd=root->exit();
      break;
    case enterCmd:
      assert(root);
      rCmd=root->enter();
      break;
    case idxCmd: {
        char at=cmd.param-'1';
        if (at>=0&&at<sz()) {
          sel=at;
          Serial<<"idxCmd:"<<sel<<endl;
          assert(root);
          rCmd=root->enter();
        }
      }
      break;
    case noCmd:
    default: break;
  }
  if(osel!=sel) {
    if (target->sysStyles()&(_parentDraw|_isVariant)) {
      target->dirty=true;
    } else {
      operator[](osel).dirty=true;
      operator[](sel).dirty=true;
    }
    //send focus In/Out events
    event(blurEvent,osel);
    event(focusEvent,sel);
  }
  return rCmd;
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
  if (sleepTask&&options->getCmdChar(enterCmd)==in.read()) idleOff();
  else if (!sleepTask&&in.available())//while ((!sleepTask)&&in.available())//if not doing something else and there is input
    navFocus->parseInput(node(),in);//deliver navigation input task to target...
}

void navRoot::doNav(navCmd cmd) {
  if (sleepTask&&cmd.cmd==enterCmd) idleOff();
  else if (!sleepTask) switch (cmd.cmd) {
    case scrlUpCmd:
    case scrlDownCmd:
      out.doNav(cmd,node());
      break;
    default:
      navFocus->doNav(node(),cmd);
  }
}


result maxDepthError(menuOut& o,idleEvent e) {
  o<<F("Error: maxDepth reached!\n\rincrease maxDepth on your scketch.");
  return proceed;
}

navCmd navRoot::enter() {
  //Serial<<"navRoot::enter "<<selected()<<endl;
  if (
    selected().enabled
    &&selected().sysHandler(activateEvent,node(),selected())==proceed
  ) {
    //Serial<<"sysHandler allowed activate"<<endl;
    prompt& sel=selected();
    bool canNav=sel.canNav();
    bool isMenu=sel.isMenu();
    result go=node().event(enterEvent);//item event sent here
    //Serial<<"enterEvent returned "<<go<<" isMenu:"<<isMenu<<" canNav:"<<canNav<<endl;
    navCmd rCmd=enterCmd;
    if (go==proceed&&isMenu&&canNav) {
      //Serial<<"proceeding..."<<endl;
      if (level<maxDepth) {
        active().dirty=true;
        menuNode* dest=(menuNode*)&selected();
        level++;
        node().target=dest;
        node().sel=0;
        active().dirty=true;
        sel.sysHandler(enterEvent,node(),selected());
        rCmd=enterCmd;
      } else {
        idleOn(maxDepthError);
        rCmd=noCmd;
      }
    } else if (go==quit&&!selected().isMenu()) exit();
    if (canNav) {
      navFocus=(navTarget*)&sel;
      navFocus->dirty=true;
    }
    return rCmd;
  }
  return noCmd;
}

navCmd navRoot::exit() {
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
  return escCmd;
}

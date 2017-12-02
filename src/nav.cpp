#include "menu.h"
using namespace Menu;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// navNode - Navigation layer controller
//
////////////////////////////////////////////////////////////////////////////////

navCmd navNode::navKeys(char ch) {
  trace(Serial<<"navNode::navKeys"<<endl);
  if (strchr(numericChars,ch)) {
    return navCmd(idxCmd,ch-'1');
  }
  for(uint8_t i=0;i<sizeof(options->navCodes)/sizeof(navCode);i++)
    if (options->navCodes[i].ch==ch) return options->navCodes[i].cmd;
  return noCmd;
}

// generic navigation (aux function)
navCmd navNode::doNavigation(navCmd cmd) {
  trace(Serial<<"navNode::doNavigation"<<endl);
  idx_t osel=sel;
  idx_t nsel=sel;
  navCmd rCmd=cmd;
  bool changed=false;
  // trace(if(cmd.cmd!=noCmd) Serial<<"navigate "<<*target<<" with command:"<<cmd.cmd<<" index:"<<nsel<<endl);
  switch(cmd.cmd) {
    /*case scrlDownCmd:
      if (!target->isVariant())
        root->out.doNav(cmd,*this);*/
    case upCmd:
      // trace(Serial<<"up"<<endl;);
      nsel++;
      if (nsel>=sz()) {if(wrap()) nsel=0; else nsel=sz()-1;}
      // trace(Serial<<"new sel:"<<nsel<<endl);
      break;
      /*case scrlUpCmd:
        if (!target->isVariant())
          root->out.doNav(cmd,*this);*/
    case downCmd:
      // trace(Serial<<"down"<<endl);
      if (nsel||!target->is(_asPad)) {
        nsel--;
        if (nsel<0) {if(wrap()) nsel=sz()-1; else nsel=0;}
        break;
      }
    case escCmd:
      assert(root);
      rCmd=root->exit();
      break;
    case enterCmd:
      assert(root);
      rCmd=root->enter();
      break;
    case selCmd:
    case idxCmd: {
        idx_t at=(idx_t)cmd.param;//-'1';send us numeric index pls!
        if (at>=0&&at<sz()) {
          changed=true;
          nsel=at;
        }
      }
      break;
    case noCmd:
    default: break;
  }
  // trace(Serial<<"changed:"<<changed<<" sels?"<<(osel!=nsel)<<endl);
  if((osel!=nsel)||changed) {//selection changed, must have been and idx/sel or an up/down movement
    // trace(Serial<<"changed"<<endl);
    if (target->sysStyles()&(_parentDraw|_isVariant)) {
      trace(Serial<<"setting dirty"<<endl);
      target->dirty=true;
      //this works but might be too much, we dont want to invaludate all the menu!
      // if (_parentDraw/*&&root->level*/) root->path[root->level-1].target->dirty=true;
    } else {
      operator[](osel).dirty=true;
      operator[](nsel).dirty=true;
    }
    //send focus In/Out events
    if (selBlurEvent&target->events()) target->operator()(selBlurEvent,*this,*target);
    event(blurEvent,osel);
    sel=nsel;
    if (cmd.cmd==selCmd||cmd.cmd==idxCmd) {//do accelerator and enter the option
      assert(root);
      rCmd=root->enter();
    }//other commands up/down just receive focus events
    event(focusEvent,nsel);
    if (selFocusEvent&target->events()) target->operator()(selFocusEvent,*this,*target);
  } //else its an enter/esc or a non-changing index!
  //Serial<<"doNavigation returning "<<rCmd<<endl;
  return rCmd;
}

result navNode::event(eventMask e,idx_t i) {
  trace(Serial<<"navNode::event"<<endl);
  prompt& p=operator[](i);
  eventMask m=p.events();
  eventMask me=(eventMask)(e&m);
  if (me) return p.eventHandler(e,*this,i);//p(e,p);
  return proceed;
}

result navNode::sysEvent(eventMask e,idx_t i) {
  trace(Serial<<"navNode::sysEvent"<<endl);
  prompt& p=operator[](i);
  return p(e,*this,p);
}

void navRoot::doInput(menuIn& in) {
  trace(Serial<<"navRoot::doInput"<<endl);
  if (sleepTask) {
    if (options->getCmdChar(enterCmd)==in.read()) idleOff();
  } else {
    idx_t inputBurstCnt=inputBurst+1;
    //if (in.available())
    while ((!sleepTask)&&in.available()&&(--inputBurstCnt)) {//if not doing something else and there is input
      //Serial.print(".");
      navFocus->parseInput(node(),in);//deliver navigation input task to target...
    }
  }
  trace(Serial<<"navRoot::doInput ended!"<<endl);
}

void navRoot::doNav(navCmd cmd) {
  trace(Serial<<"navRoot::doNav"<<endl);
  if (sleepTask&&cmd.cmd==enterCmd) idleOff();
  else if (!sleepTask) switch (cmd.cmd) {
    case scrlUpCmd:
    case scrlDownCmd:
      out.doNav(cmd,node());//scroll is perceived better at output device
      break;
    default:
      navFocus->doNav(node(),cmd);
  }
}

navCmd navRoot::enter() {
  trace(Serial<<"navRoot::enter"<<endl);
  if (
    selected().enabled
    &&selected().sysHandler(activateEvent,node(),selected())==proceed
  ) {
    trace(Serial<<"enabled by syshandler"<<endl);
    prompt& sel=selected();
    bool canNav=sel.canNav();
    bool isMenu=sel.isMenu();
    result go=node().event(enterEvent);//item event sent here
    navCmd rCmd=enterCmd;
    if (go==proceed&&isMenu&&canNav) {
      trace(Serial<<"go for canNav && isMenu"<<endl);
      if (level<maxDepth) {
        trace(Serial<<"level<maxDepth"<<endl);
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
      trace(Serial<<"canNav"<<endl);
      navFocus=(navTarget*)&sel;
      navFocus->dirty=true;
      if (!isMenu) in.fieldOn();
    }
    return rCmd;
  }
  return noCmd;
}

navCmd navRoot::exit() {
  trace(Serial<<"navRoot::exit"<<endl);
  navFocus->dirty=true;
  if (navFocus->isMenu()) {
    if (level) {
      level--;
      node().event(exitEvent,node().sel);
    } else if (canExit) {
      node().event(exitEvent,node().sel);
      idleOn(idleTask);
    }
  } else {
    node().event(exitEvent,node().sel);
    in.fieldOff();
  }
  active().dirty=true;
  navFocus=&active();
  return escCmd;
}

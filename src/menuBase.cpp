
#include "menuDefs.h"

using namespace Menu;

#ifdef MENU_DEBUG
  bool debugFlag=false;
  #if defined(USING_PGM)
    const char* libMemMode="PGM";
  #elif defined(USING_RAM)
    const char* libMemMode="RAM";
  #else
    const char* libMemMode="ERROR!";
  #endif
#endif

template<void (*A)(eventMask event, navNode& nav, prompt &item, menuIn &in)> result Menu::callCaster(eventMask event, navNode& nav, prompt &item, menuIn &in) {A(event,nav,item,in);return proceed;}
template<void (*A)(eventMask event, navNode& nav, prompt &item)> result Menu::callCaster(eventMask event, navNode& nav, prompt &item) {A(event,nav,item);return proceed;}
template<void (*A)(eventMask event, navNode& nav)> result Menu::callCaster(eventMask event, navNode& nav) {A(event,nav);return proceed;}
template<void (*A)(eventMask event)> result Menu::callCaster(eventMask event) {A(event);return proceed;}
template<void (*A)()> result Menu::callCaster() {A();return proceed;}

//MEMMODE AUX PRINT
#if defined(MENU_DEBUG) || defined(MENU_ASYNC)
idx_t Menu::print_P(Print& s,const char* at,idx_t len) {
  const char* p=at;
  uint8_t ch;
  for(int n=0;(ch=memByte(at++))&&(len==0||n<len);n++)
    s.write(ch);
  return at-p-1;
}
#endif

idx_t Menu::print_P(menuOut& s,const char* at,idx_t len) {
  return s.printRaw(at,len);
}

const navCodesDef Menu::defaultNavCodes={
  {noCmd,(uint8_t)0xff},
  {escCmd,'/'},
  {enterCmd,'*'},
  {upCmd,'+'},
  {downCmd,'-'},
  {leftCmd,'-'},
  {rightCmd,'+'},
  {idxCmd,'?'},
  {scrlUpCmd,0x35},
  {scrlUpCmd,0x36}
};

config Menu::defaultOptions={'>','-',Menu::defaultNavCodes,false};
config* Menu::options=&defaultOptions;

#ifdef MENU_DEBUG

  Print& Menu::operator<<(Print& o,bool b) {
    return o<<(b?"true":"false");
  }

  Print& Menu::operator<<(Print& o,result r) {
    switch(r) {
      case proceed: o<<"proceed";break;
      case quit: o<<"quit";break;
    }
    return o;
  }

  const char* eventName(eventMask e) {
    switch(e) {
      case noEvent:return "noEvent";
      case activateEvent:return "activateEvent";
      case enterEvent:return "enterEvent";
      case exitEvent:return "exitEvent";
      case returnEvent:return "returnEvent";
      case focusEvent:return "focusEvent";
      case blurEvent:return "blurEvent";
      case selFocusEvent:return "selFocusEvent";
      case selBlurEvent:return "selBlurEvent";
      case anyEvent:return "anyEvent";
      default: return "need to implement this print";
    }
  }

  Print& Menu::operator<<(Print& o,eventMask e) {
    if (e==noEvent||e==anyEvent) {o<<eventName(e); return o;}
    bool first=true;
    for(int n=0;n<=6;n++) {
      eventMask t=(eventMask)(1<<n);
      if (t&e) {
        o<<(first?"":"|")<<eventName(t);
        first=false;
      }
    }
    return o;
  }

  Print& Menu::operator<<(Print& o,navCmds cmd) {
    switch(cmd) {
      case noCmd:o<<"noCmd";break;
      case escCmd:o<<"escCmd";break;
      case enterCmd:o<<"enterCmd";break;
      case upCmd:o<<"upCmd";break;
      case downCmd:o<<"downCmd";break;
      case leftCmd:o<<"leftCmd";break;
      case rightCmd:o<<"rightCmd";break;
      case idxCmd:o<<"idxCmd";break;
      case selCmd:o<<"selCmd";break;
      default:o<<"? "<<cmd;break;
    }
    return o;
  }

  Print& Menu::operator<<(Print& o,navCmd cmd) {
    return o<<"{"<<cmd.cmd<<","<<cmd.param<<"}";
  }

  Print& Menu::operator<<(Print& o,colorDefs c) {
    switch(c) {
      case bgColor:o<<"bgColor";break;
      case fgColor:o<<"fgColor";break;
      case valColor:o<<"valColor";break;
      case unitColor:o<<"unitColor";break;
      case cursorColor:o<<"cursorColor";break;
      case titleColor:o<<"titleColor";break;
      default:o<<"color?";break;
    }
    return o;
  };

  Print& Menu::operator<<(Print& o,idleEvent e) {
    switch(e) {
      case idleStart:o<<"idleStart";break;
      case idling:o<<"idling";break;
      case idleEnd:o<<"idleEnd";break;
      default: o<<"idelEvent?"<<(int)e;break;
    }
    return o;
  }

  Print& Menu::operator<<(Print& o,systemStyles s) {
    if (s==_noStyle) o<<"_noStyle";
    else {
      bool first=true;
      if (s&_menuData) {o<<"_menuData";first=false;}
      if (s&_canNav) {o<<(first?"":"|")<<"_canNav";first=false;}
      if (s&_parentDraw) {o<<(first?"":"|")<<"_parentDraw";first=false;}
      if (s&_isVariant) {o<<(first?"":"|")<<"_isVariant";first=false;}
    }
    return o;
  }

  // Print& Menu::operator<<(Print& o,prompt &p) {
  //   print_P(o,p.getText(),-1);
  //   return o;
  // }
#endif

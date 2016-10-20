#include "menu.h"

using namespace Menu;

#ifdef DEBUG
  bool debugFlag=false;
#endif

template<void (*A)(eventMask event, navNode& nav, prompt &item, Stream &in)> result Menu::callCaster(eventMask event, navNode& nav, prompt &item, Stream &in) {A(event,nav,item,in);return proceed;}
template<void (*A)(eventMask event, navNode& nav, prompt &item)> result Menu::callCaster(eventMask event, navNode& nav, prompt &item) {A(event,nav,item);return proceed;}
template<void (*A)(eventMask event, navNode& nav)> result Menu::callCaster(eventMask event, navNode& nav) {A(event,nav);return proceed;}
template<void (*A)(eventMask event)> result Menu::callCaster(eventMask event) {A(event);return proceed;}
template<void (*A)()> result Menu::callCaster() {A();return proceed;}

//MEMMODE AUX PRINT
int Menu::print_P(Print& s,const char* at) {
  const char* p=at;
  while(uint8_t ch=memByte(at++)) s.write(ch);
  return p-at;
}

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

const char* showEvent(eventMask e) {
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
  if (e==noEvent||e==anyEvent) {o<<showEvent(e); return o;}
  bool first=true;
  for(int n=0;n<=6;n++) {
    eventMask t=(eventMask)(1<<n);
    if (t&e) {
      o<<(first?"":"|")<<showEvent(t);
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
  }
  return o;
}

Print& Menu::operator<<(Print& o,prompt const &p) {
  print_P(o,(const char*)memPtr(p.shadow->text));
  return o;
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

#ifndef Arduino_h
Print Serial;
#endif

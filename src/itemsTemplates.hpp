#pragma once

#include "menuDefs.h"

////////////////////////////////////////////////////////////////////////
// template implementation
#include "menuIo.h"
#include "nav.h"
namespace Menu {

  #ifdef MENU_ASYNC
    template<typename T>
    bool menuValue<T>::async(const char*uri,navRoot& root,idx_t lvl) {
      trace(MENU_DEBUG_OUT<<(*(prompt*)this)<<" menuValue::async! lvl:"<<lvl<<" navRoot.level:"<<root.level<<" navFocus:"<<(*(prompt*)root.navFocus)<<endl);
      return prompt::async(uri,root,lvl);
    }
    template<typename T>
    const char* menuValue<T>::typeName() const {return "menuValue";}
  #endif

  template<typename T>
  bool menuField<T>::canTune() {return !!tune();}
  template<typename T>
  void menuField<T>::constrainField() {target() = constrain(target(), low(), high());}
  template<typename T>
  void menuField<T>::clearChanged(const navNode &nav,const menuOut& out,bool sub) {
    fieldBase::clearChanged(nav,out,sub);
    reflex=target();
  }
  template<typename T>
  bool menuField<T>::changed(const navNode &nav,const menuOut& out,bool sub,bool test) {
    trace(if (test&&dirty) MENU_DEBUG_OUT<<"field dirty"<<endl);
    trace(if (test&&(reflex!=target())) MENU_DEBUG_OUT<<"reflex!=target reflex:"<<reflex<<" target:"<<target()<<endl);
    return dirty||(reflex!=target());
  }

  template<typename T>
  idx_t menuField<T>::printReflex(menuOut& o) const {return o.print(reflex);}

  template<typename T>
  Used menuField<T>::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
    trace(print_P(out,getText());MENU_DEBUG_OUT<<" menuField<T>::printTo "<<reflex<<endl);
    reflex=target();
    return fieldBase::printTo(root,sel,out,idx,len,panelNr);
  }

  template<typename T>
  void menuField<T>::parseInput(navNode& nav,menuIn& in) {
    //TODO: on a cmd based nav (not streams) this mess will be pushed to stream input only
    //can not enter negative number literals by serial, using steps or web is ok
    bool neg=false;
    char nc=in.peek();
    if (nc=='-') {
      in.read();
      if (!strchr(numericChars,in.peek())) {
        doNav(nav,downCmd);
        return;
      }
      // Serial.println("NEGATIVE NUMBER PARSE THEN!");
      neg=true;
    }
    if (neg||strchr(numericChars,nc)) {//a numeric value was entered
      if (in.numValueInput) {
        target()=(T)((neg?-1:1)*in.parseFloat());//TODO: use template specialization and proper convertion
        tunning=true;
        doNav(nav,enterCmd);
      } else doNav(nav,idxCmd);
    } else doNav(nav,nav.navKeys(in.read()));
  }

  #ifdef MENU_ASYNC
    template<typename T>
    void menuField<T>::printValue(menuOut& o) const {o.print(reflex);}
    template<typename T>
    void menuField<T>::printHigh(menuOut& o) const {o.print(high());}
    template<typename T>
    void menuField<T>::printLow(menuOut& o) const {o.print(low());}
    template<typename T>
    void menuField<T>::printStep(menuOut& o) const {o.print(step());}
    template<typename T>
    void menuField<T>::printTune(menuOut& o) const {o.print(tune());}
    template<typename T>
    const char* menuField<T>::typeName() const {return typeStr<T>();};
  #endif

  template<typename T>
  void menuField<T>::stepit(int dir) {
    dir*=options->invertFieldKeys?-1:1;
    T thisstep = tunning?tune():step();
    dirty=true;
    //by default they are inverted.. now buttons and joystick have to flip them
    if (dir > 0) {
      if ((high()-target()) < thisstep)
        target() = canWrap()?low():high();
      else
        target() += thisstep;
    } else {
      if ((target()-low()) < thisstep)
        target() = canWrap()?high():low();
      else
        target() -= thisstep;
    }
  }

  template<uint8_t dps>
  template<typename T>
  idx_t decPlaces<dps>::menuField<T>::printReflex(menuOut& o) const {return o.print(Menu::menuField<T>::reflex,dps);}

  template<typename T>
  Used choose<T>::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
    bool ed=this==root.navFocus;
    return ed?
      prompt::printTo(root,sel,out,idx,len,panelNr)
      :menuVariantBase::printTo(root,sel,out,idx,len,panelNr);
  }

  template<typename T>
  Used toggle<T>::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
    return menuVariantBase::togglePrintTo(root,sel,out,idx,len,panelNr);
  }

  #ifdef MENU_ASYNC
    template<typename T>
    const char* toggle<T>::typeName() const {return "toggle";}
    template<typename T>
    bool toggle<T>::async(const char*uri,navRoot& root,idx_t lvl) {
      _trace(MENU_DEBUG_OUT<<(*(prompt*)this)<<" toggle::async! uri:"<<uri<<endl);
      if(uri[0]) {
        _trace(Serial<<"selecting value by index!"<<endl);
        idx_t n=menuNode::parseUriNode(uri);
        _trace(MENU_DEBUG_OUT<<"n:"<<n<<" sel:"<<root.path[lvl].sel<<endl);
        menuVariant<T>::sync(n);//sync to index n!
        // root.node().event(root.useUpdateEvent?updateEvent:enterEvent);
        return true;
      }
      //if not by index then do the toggle
      _trace(Serial<<"toggle proceed..."<<endl);
      // root.doNav(navCmd(enterCmd));
      // return true;
      // bool r=prompt::async(uri,root,lvl);
      // root.node().event(root.useUpdateEvent?updateEvent:enterEvent);
      // return r;
      return prompt::async(uri,root,lvl);
    }
  #endif

  template<typename T>
  result toggle<T>::sysHandler(SYS_FUNC_PARAMS) {
    _trace(Serial<<"toggle sysHandler!"<<endl;);
    switch(event) {
        case activateEvent: {
        idx_t at=menuVariant<T>::sync();
        assert(at!=-1);
        at++;
        if (at>=menuNode::sz()) at=0;
        menuVariant<T>::sync(at);
        prompt::dirty=true;
        (menuNode::operator[](at))(FUNC_VALUES);
      }
      default:
        return proceed;
    }
  }

  #ifdef MENU_FMT_WRAPS
    template<typename T>
    classes toggle<T>::type() const {return toggleClass;}
  #endif

  template<typename T>
  idx_t menuVariant<T>::sync() {
    for(idx_t i=0;i<sz();i++)
      if (((menuValue<T>*)&operator[](i))->target()==target()) return i;
    #ifdef MENU_DEBUG
      MENU_DEBUG_OUT.print("value out of range ");
      MENU_DEBUG_OUT.println(target());MENU_DEBUG_OUT.flush();
      assert(false);
    #endif
    return -1;
  }
  template<typename T>
  idx_t menuVariant<T>::sync(idx_t i) {
    #ifdef MENU_DEBUG
      if (!(i>=0&&i<sz())){
        print_P(MENU_DEBUG_OUT,getText());
        MENU_DEBUG_OUT.print(" : value out of range ");
        MENU_DEBUG_OUT.println(i);
      }
      assert(i>=0&&i<sz());
    #endif
    if (i!=reflex) dirty=true;
    reflex=i;
    target()=((menuValue<T>*)&operator[](i))->target();
    return i;
  }

  template<typename T>
  bool menuVariant<T>::changed(const navNode &nav,const menuOut& out,bool sub,bool test) {
    return dirty||((menuValue<T>*)&operator[](reflex))->target()!=target();
  }

  #ifdef MENU_ASYNC
    template<typename T>
    idx_t menuVariant<T>::selected() const {return reflex;}
    template<typename T>
    const char* menuVariant<T>::typeName() const {return "menuVariant";}
  #endif

  template<typename T>
  result choose<T>::sysHandler(SYS_FUNC_PARAMS) {
    switch(event) {
      case updateEvent:
      case enterEvent:
        nav.sel=menuVariant<T>::sync();
      default:
        return proceed;
    }
  }

  template<typename T>
  bool choose<T>::changed(const navNode &nav,const menuOut& out,bool sub,bool test) {
    return menuVariant<T>::changed(nav,out)||menuNode::changed(nav,out);
  }

  #ifdef MENU_FMT_WRAPS
    template<typename T>
    classes choose<T>::type() const {return chooseClass;}
  #endif

  #ifdef MENU_ASYNC
    template<typename T>
    const char* choose<T>::typeName() const {return "choose";}
  #endif

}//namespace Menu

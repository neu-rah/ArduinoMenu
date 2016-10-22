// menu aux functions and vars ------------------------------------------------
result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial
    <<ANSI::xy(0,13)
    <<ANSI::setForegroundColor(WHITE)
    <<ANSI::setBackgroundColor(BLACK)
    <<ANSI::eraseLine()
    <<e<<" on "<<item;
  return proceed;
}

int test=55;

result action1(eventMask e,navNode& nav, prompt &item) {
  Serial
    <<ANSI::xy(0,13)
    <<ANSI::setForegroundColor(WHITE)
    <<ANSI::setBackgroundColor(BLACK)
    <<e<<" event on "<<item<<", proceed menu";
  Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<ANSI::xy(24,nav.sel+nav.root->showTitle)
    <<item<<" "<<e<<" event on "<<item<<", quiting menu.";
  Serial.flush();
  return quit;
}

int ledCtrl=LOW;

result ledOn() {
  ledCtrl=HIGH;
  return proceed;
}
result ledOff() {
  ledCtrl=LOW;
  return proceed;
}

////////////////////////////////////////////////////////////////////////////////
// menu structure
TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
  altPrompt(const promptShadow& p):prompt(p) {}
  void printTo(navRoot &root,bool sel,menuOut& out) override {
    out<<"special prompt!";
  }
};

MENU(subSubMenu,"Sub-Sub-Menu",doNothing,noEvent,noStyle
  ,OP("SSub1",doNothing,noEvent)
  ,OP("SSub2",doNothing,noEvent)
  ,EXIT("<Back")
);//just to test depth limit

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"custom",showEvent,anyEvent)
  ,SUBMENU(subSubMenu)
  ,EXIT("<Back")
);

extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);

result alert(menuOut& o,idleEvent e) {
  if (e==idling)
    o
      <<ANSI::xy(0,0)
      <<ANSI::setBackgroundColor(BLACK)
      <<ANSI::setForegroundColor(WHITE)
      <<"alert test"
      <<endl
      <<"press [select] to continue..."
      <<endl;
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  //,SUBMENU(togOp)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Show colors",showOutColors,enterEvent)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

#include <Arduino.h>

/********************
Arduino generic menu system
Arduino menu on I2C LCD example
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Sep.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

LCD library:
https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
http://playground.arduino.cc/Code/LCD3wires
*/
#ifndef ARDUINO_SAM_DUE

  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>//F. Malpartida LCD's driver
  #include <menu.h>//menu macros and objects
  #include <menuIO/lcdOut.h>//malpartidas lcd menu output
  #include <menuIO/serialIn.h>//Serial input
  #include <menuIO/encoderIn.h>//quadrature encoder driver and fake stream
  #include <menuIO/keyIn.h>//keyboard driver and fake stream (for the encoder button)
  #include <menuIO/chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)

  using namespace Menu;

  //LiquidCrystal_I2C lcd(0x27);//, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
  LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address and pinout

  // Encoder /////////////////////////////////////
  #define encA 2
  #define encB 3
  //this encoder has a button here
  #define encBtn 4

  encoderIn<encA,encB> encoder;//simple quad encoder driver
  #define ENC_SENSIVITY 4
  encoderInStream<encA,encB> encStream(encoder,ENC_SENSIVITY);// simple quad encoder fake Stream

  //a keyboard with only one key as the encoder button
  keyMap encBtn_map[]={{-encBtn,enterCmd}};//negative pin numbers use internal pull-up, this is on when low
  keyIn<1> encButton(encBtn_map);//1 is the number of keys

  serialIn serial(Serial);

  //input from the encoder + encoder button + serial
  menuIn* inputsList[]={&encStream,&encButton,&serial};
  chainStream<3> in(inputsList);//3 is the number of inputs

  #define LEDPIN A3

  result doAlert(eventMask e, prompt &item);

  result showEvent(eventMask e,navNode& nav,prompt& item) {
    Serial.print("event: ");
    Serial.println(e);
    return proceed;
  }

  int test=55;

  result action1(eventMask e,navNode& nav, prompt &item) {
    Serial.print("action1 event: ");
    Serial.print(e);
    Serial.println(", proceed menu");
    Serial.flush();
    return proceed;
  }

  result action2(eventMask e,navNode& nav, prompt &item) {
    Serial.print("action2 event: ");
    Serial.print(e);
    Serial.println(", quiting menu.");
    Serial.flush();
    return quit;
  }

  int ledCtrl=LOW;

  result myLedOn() {
    ledCtrl=HIGH;
    return proceed;
  }
  result myLedOff() {
    ledCtrl=LOW;
    return proceed;
  }

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
    altPrompt(constMEM promptShadow& p):prompt(p) {}
    Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
      return out.printRaw("special prompt!",len);;
    }
  };

  MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
    ,OP("Sub1",showEvent,anyEvent)
    ,OP("Sub2",showEvent,anyEvent)
    ,OP("Sub3",showEvent,anyEvent)
    ,altOP(altPrompt,"",showEvent,anyEvent)
    ,EXIT("<Back")
  );

  /*extern menu mainMenu;
  TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
    ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
    ,VALUE("disabled",disabledStatus,doNothing,noEvent)
  );*/

  char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
  char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
  char buf1[]="0x11";

  MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
    ,OP("Op1",action1,anyEvent)
    ,OP("Op2",action2,enterEvent)
    //,SUBMENU(togOp)
    ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
    ,SUBMENU(subMenu)
    ,SUBMENU(setLed)
    ,OP("LED On",myLedOn,enterEvent)
    ,OP("LED Off",myLedOff,enterEvent)
    ,SUBMENU(selMenu)
    ,SUBMENU(chooseMenu)
    ,OP("Alert test",doAlert,enterEvent)
    ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
    ,EXIT("<Back")
  );

  #define MAX_DEPTH 2

  /*const panel panels[] MEMMODE={{0,0,16,2}};
  navNode* nodes[sizeof(panels)/sizeof(panel)];
  panelsList pList(panels,nodes,1);
  idx_t tops[MAX_DEPTH];
  lcdOut outLCD(&lcd,tops,pList);//output device for LCD
  menuOut* outputs[]={&outLCD};//list of output devices
  outputsList out(outputs,1);//outputs list with 2 outputs
  */

  MENU_OUTPUTS(out,MAX_DEPTH
    ,LCD_OUT(lcd,{0,0,16,2})
    ,NONE
  );
  NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);//the navigation root object

  result alert(menuOut& o,idleEvent e) {
    if (e==idling) {
      o.setCursor(0,0);
      o.print("alert test");
      o.setCursor(0,1);
      o.print("[select] to continue...");
    }
    return proceed;
  }

  result doAlert(eventMask e, prompt &item) {
    nav.idleOn(alert);
    return proceed;
  }

  result idle(menuOut& o,idleEvent e) {
    switch(e) {
      case idleStart:o.print("suspending menu!");break;
      case idling:o.print("suspended...");break;
      case idleEnd:o.print("resuming menu.");break;
    }
    return proceed;
  }

  void setup() {
    pinMode(encBtn,INPUT_PULLUP);
    pinMode(LEDPIN,OUTPUT);
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Arduino Menu Library");Serial.flush();
    encoder.begin();
    lcd.begin(16,2);
    nav.idleTask=idle;//point a function to be used when menu is suspended
    mainMenu[1].enabled=disabledStatus;
    nav.showTitle=false;
    lcd.setCursor(0, 0);
    lcd.print("Menu 4.x LCD");
    lcd.setCursor(0, 1);
    lcd.print("r-site.net");
    delay(2000);
  }

  void loop() {
    nav.poll();
    digitalWrite(LEDPIN, ledCtrl);
    delay(100);//simulate a delay as if other tasks are running
  }

#endif

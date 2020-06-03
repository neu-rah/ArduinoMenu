/********************
Arduino generic menu system

Sept.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

On this case: set of alarms
- Reusing a menu to setup multiple alatms
- Using a very specialized prompt with alarm data
- Using custom menu print to customize title text

the alarm data is stored on the menu prompts, we use an extra alarm prompt (tmp)
to be the editing target of the alarm setup menu.

We are using Serial, however serial does a lot of redraw because of custom looks
ideally this should run on a device that can position the cursor (not simple serial)
you can try the ANSI serial or any LCD/graphics output

Also time is tracked from millis() for simplicity, it should be tracked
from a real time device instead. However the purpose of this demo is to
show reutilization of a menu and customization of both menus and prompts
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
//#include <menuIO/ansiSerialOut.h>

using namespace Menu;

// define menu colors for ANSI terminal
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
/*const colorDef<uint8_t> colors[6] MEMMODE={
  {{BLUE,WHITE}  ,{BLUE,WHITE,WHITE}},//bgColor
  {{BLACK,BLACK} ,{WHITE,BLUE,BLUE}},//fgColor
  {{BLACK,BLACK} ,{YELLOW,YELLOW,RED}},//valColor
  {{BLACK,BLACK} ,{WHITE,BLUE,YELLOW}},//unitColor
  {{BLACK,BLACK} ,{BLACK,BLUE,RED}},//cursorColor
  {{BLACK,BLACK}  ,{BLUE,RED,BLUE}},//titleColor
};*/

//this is the tmp shadow data (stored on flash)
constMEM char promptShadowTitle[] MEMMODE="";
constMEM promptShadowRaw alarmPromptShadowRaw MEMMODE={
  (Menu::callback)doNothing,
  _noStyle,
  promptShadowTitle,
  noEvent,
  noStyle
};

//using a special prompt to print the alarm info
class AlarmPrompt:public prompt {
  public:
    bool use=false;
    enum {alarm,countDown} type=alarm;
    int h=0,m=0,s=0;
    long msTime;
    long lastTime;
    static AlarmPrompt* selectedAlarm;
    static idx_t selectedAlarmIdx;

    AlarmPrompt(constMEM promptShadow& p):prompt(p) {}

    AlarmPrompt& operator=(AlarmPrompt& o) {
      //Serial<<"operator="<<endl;
      use=o.use;
      type=o.type;
      h=o.h;
      m=o.m;
      s=o.s;
      msTime=o.msTime;
    }
    //initialize the alarm
    void setup() {
      //Serial<<"alarm setup"<<endl;
      msTime=((long)h*60*60+(long)m*60+(long)s)*1000;
      lastTime=millis();
    }
    //alarm tick update
    void update();
    Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
      if (use) {
        switch(type) {
          case alarm:
            len-=out.printRaw(F("Alarm "),len);
            len-=out.print(h);
            len-=out.printRaw(F(":"),len);
            len-=out.print(m);
            len-=out.printRaw(F(":"),len);
            len-=out.print(s);
            break;
          case countDown:
            len-=out.printRaw(F("Countdown "),len);
            long ss=msTime/1000;
            long hh=ss/(60*60);
            ss-=hh*60*60;
            long mm=ss/60;
            ss-=mm*60;
            if (hh<10) len-=out.print("0");
            len-=out.print(hh);
            len-=out.printRaw(F(":"),len);
            if (mm<10) len-=out.print("0");
            len-=out.print(mm);
            len-=out.printRaw(F(":"),len);
            if (ss<10) len-=out.print("0");
            len-=out.print(ss);
            break;
        }
      } else {
        len-=out.printRaw(F("Alarm"),len);
        len-=out.print(idx+1);
        len-=out.printRaw(F(" Off"),len);
      }
      return len;
    }
    result eventHandler(eventMask e,navNode& nav,idx_t i) override;
    static result quitAlarmSetup();
    static void setupAlarm();
} tmp(*(promptShadow*)&alarmPromptShadowRaw);//define a tmp object to be edited

//custome menu look, prints time at title
class AlarmMenu:public menu {
  public:
    AlarmMenu(constMEM menuNodeShadow& shadow):menu(shadow) {}
    Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr=0) override {
      long ss=millis()/1000;
      long hh=ss/(60*60);
      ss-=hh*60*60;
      long mm=ss/60;
      ss-=mm*60;
      len-=print_P(out,getText(),len);
      len-=out.printRaw(F(" "),len);
      if (hh<10) len-=out.print("0");
      len-=out.print(hh);
      len-=out.printRaw(F(":"),len);
      if (mm<10) len-=out.print("0");
      len-=out.print(mm);
      len-=out.printRaw(F(":"),len);
      if (ss<10) len-=out.print("0");
      len-=out.print(ss);
      //dirty=true;useless here because dirty flag is cleaned after all devices print
    }
    bool changed(const navNode &nav,const menuOut& out,bool sub=true,bool test=false) override {
      //this does not set the dirty flag, it  would redraw all menu causing flick
      //however printMenu checks this as a title change signal
      return true;//always changed!
      //return menu::changed(nav,out,sub);//disable for debug of other parts!
    };
};
//the static members (this are common to all AlarmPrompt objects)
AlarmPrompt* AlarmPrompt::selectedAlarm=NULL;
idx_t AlarmPrompt::selectedAlarmIdx=-1;

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION

// menu for setup an alarm, we will reuse this for all alarms
TOGGLE(tmp.use,setUse,"Use: ",doNothing,noEvent,noStyle
  ,VALUE("Yes",true,doNothing,noEvent)
  ,VALUE("No",false,doNothing,noEvent)
);

TOGGLE(tmp.type,setType,"Type: ",doNothing,noEvent,noStyle
  ,VALUE("Alarm",AlarmPrompt::alarm,doNothing,enterEvent)
  ,VALUE("Countdown",AlarmPrompt::countDown,doNothing,enterEvent)
);

//we will reuse this menu to setup all alarms
altMENU(AlarmMenu,alarmDef,"Alarm def",doNothing,noEvent,wrapStyle,(Menu::_menuData|Menu::_canNav)
  ,SUBMENU(setUse)
  ,SUBMENU(setType)
  ,FIELD(tmp.h,"Hour","",0,23,1,0,doNothing,noEvent,wrapStyle)
  ,FIELD(tmp.m,"Minutes","",0,59,5,1,doNothing,noEvent,wrapStyle)
  ,FIELD(tmp.s,"Seconds","",0,59,5,1,doNothing,noEvent,wrapStyle)
  ,OP("Ok",AlarmPrompt::setupAlarm,enterEvent)
  ,OP("Cancel",AlarmPrompt::quitAlarmSetup,enterEvent)
);

void alarm1() {Serial.println("\r\nAlarm 1 reached!");}
void alarm2() {Serial.println("\r\nAlarm 2 reached!");}
void alarm3() {Serial.println("\r\nAlarm 3 reached!");}

//menu with list of alarms
altMENU(AlarmMenu,mainMenu,"Alarms",doNothing,noEvent,wrapStyle,(Menu::_menuData|Menu::_canNav)
  ,altOP(AlarmPrompt,"Alarm1",alarm1,enterEvent)
  ,altOP(AlarmPrompt,"Alarm2",alarm2,enterEvent)
  ,altOP(AlarmPrompt,"Alarm3",alarm3,enterEvent)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  //,ANSISERIAL_OUT(Serial,colors,{1,1,20,10})//,{18,1,16,10},{36,1,16,10})
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

idleFunc backupIdleTask=nav.idleTask;

result AlarmPrompt::eventHandler(eventMask e,navNode& nav,idx_t i) {
  selectedAlarm=this;
  selectedAlarmIdx=i;
  tmp=*this;
  nav.root->useMenu(alarmDef);
  backupIdleTask=nav.root->idleTask;
  nav.root->idleTask=(idleFunc)quitAlarmSetup;
  //return prompt::eventHandler(e,i);//this is reserved for alarm reach
  return proceed;
}

//when an alarm is selected
void AlarmPrompt::setupAlarm() {
  *selectedAlarm=tmp;
  selectedAlarm->setup();
  quitAlarmSetup();
}

result AlarmPrompt::quitAlarmSetup() {
  nav.sleepTask=NULL;//do not call idleOff! it will exaust the stack
  nav.idleTask=backupIdleTask;
  nav.useMenu(mainMenu);
  nav.node().sel=selectedAlarmIdx;
  return proceed;
}

void AlarmPrompt::update() {
  if (use) {
    switch(type) {
      case countDown: {
          long ms=millis();
          long delta=ms-lastTime;
          lastTime=ms;
          msTime-=delta;
          if (msTime<=0) {
            use=false;
            operator()(enterEvent,nav.node(),*this);
          }
        }
        dirty=true;
        break;
      case alarm:
        if (millis()>=msTime) {
          use=false;
          operator()(enterEvent,nav.node(),*this);
          dirty=true;
        }
        break;
    }
  }
}
void setup() {
  Serial.begin(115200);
  while(!Serial);
  nav.showTitle=true;
  Serial.println("Menu 4.x reuse test");
  delay(1000);
  out.clear();
}

void loop() {
  for(int n=0;n<3;n++)//update the alarms idx: 0,1,2
    ((AlarmPrompt*)&mainMenu[n])->update();
  nav.poll();
  delay(300);
}

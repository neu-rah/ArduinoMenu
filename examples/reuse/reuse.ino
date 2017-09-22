/********************
Arduino generic menu system
Serial menu example
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Sept.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes
*/
#include <menu.h>//menu macros and objects
#include <menuFields.h>
#include <menuPrint.h>
//#include <streamFlow.h>

struct Timer {
  bool use=false;
  enum {alarm,countDown} type=alarm;
  int h=0,m=0,s=0;
};

Timer tmp;
Timer alarms[3];

promptFeedback cancel() {return true;}
promptFeedback setCurAlarm();
promptFeedback setupAlarm();

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION

// menu for setup an alarm, we will reuse this for all alarms
TOGGLE(tmp.use,setUse,"Use",VALUE("Yes",true),VALUE("No",false));
SELECT(tmp.type,setType,"Type"
  ,VALUE("Alarm",Timer::alarm)
  ,VALUE("Countdown",Timer::countDown)
);
MENU(alarmDef,"Alarm def"
  ,SUBMENU(setUse)
  ,SUBMENU(setType)
  ,FIELD(tmp.h,"Hour","",0,24,1,0)
  ,FIELD(tmp.m,"Minuts","",0,60,5,1)
  ,FIELD(tmp.s,"Seconds","",0,60,5,1)
  ,OP("Ok",setupAlarm)
);

//menu with list of alarms
MENU(mainMenu,"Alarms"
  ,OP("Alarm 1",setCurAlarm)
  ,OP("Alarm 2",setCurAlarm)
  ,OP("Alarm 3",setCurAlarm)
);

//when click ok on alarm
promptFeedback setCurAlarm() {
  tmp=alarms[mainMenu.sel];
  menuNode::activeNode=&alarmDef;
  return false;
}

//when an alarm is selected
promptFeedback setupAlarm() {
  Serial.println("Ok");
  alarms[mainMenu.sel]=tmp;
  return cancel();
}

menuPrint menu_out(Serial);//describe output device

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu system test");
  delay(1000);
}

void loop() {
  mainMenu.poll(menu_out,Serial);
  delay(100);
}

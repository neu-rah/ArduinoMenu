/********************
Oct. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu to ANSI serial terminal
output: ANSI Serial terminal
input: Serial

draw color menu on ansi serial device
to see the result on linux machines use:

screen /dev/ttyUSBn 115200

replace the port (/dev/ttyUSBn) with your appropriate port
screen utility exits with [Ctrl+A \ y]

www.r-site.net
***/

#include <menu.h>

using namespace Menu;

#define LEDPIN A4

// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
const colorDef<uint8_t> colors[] MEMMODE={
  {{BLUE,WHITE}  ,{BLUE,WHITE,WHITE}},//bgColor
  {{BLACK,BLACK} ,{WHITE,BLUE,BLUE}},//fgColor
  {{BLACK,BLACK} ,{YELLOW,YELLOW,RED}},//valColor
  {{BLACK,BLACK} ,{WHITE,BLUE,YELLOW}},//unitColor
  {{BLACK,BLACK} ,{BLACK,BLUE,RED}},//cursorColor
  {{BLACK,BLACK}  ,{BLUE,RED,BLUE}},//titleColor
};

//define menu outputs ------------------------------------------------
const panel panels[] MEMMODE={{1,1,16,10},{18,1,16,10},{36,1,16,10}};
menuNode* nodes[sizeof(panels)/sizeof(panel)];
panelsList pList(panels,nodes,sizeof(panels)/sizeof(panel));
ansiSerialOut ansi(Serial,colors,pList);//the output device, ansi-terminal Cols x Rows
menuOut* outputs[]={&ansi};
outputsList out(outputs,1);

void putColor(
  menuOut& out,
  colorDefs def,
  bool selected,
  status stat,
  bool edit
  ,int x,int y
) {
    out.fill(x,y,x+8,y,' ',bgColor,selected,stat,edit);
    out.setColor(def,selected,stat,edit);
    out.setCursor(x,y-1);
    out<<"XX";
}

void showColorDef(menuOut& out,colorDefs def,int x,int y) {
  out<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  out<<ANSI::xy(x+1,y+1)<<def<<ANSI::xy(x+16,y+1)<<"Disabled | Enabled | Editing  "<<endl;
  out<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  out<<ANSI::xy(x+1,y+2)<<"normal";
  putColor(out, def, false, disabledStatus, false,x+15,y+1);
  putColor(out, def, false, enabledStatus, false,x+25,y+1);
  out<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  out<<ANSI::xy(x+1,y+3)<<"selected";
  putColor(out, def, true, disabledStatus, false,x+15,y+2);
  putColor(out, def, true, enabledStatus, false,x+25,y+2);
  putColor(out, def, true, enabledStatus, true,x+35,y+2);
}

result showColors(menuOut& o,idleEvent e) {
  switch(e) {
    case idling: for(int c=0;c<nColors;c++) showColorDef(o,c,0,c<<2); break;
    default:
      o<<ANSI::xy(80,25)<<ANSI::eraseScreen()<<ANSI::xy(1,1);
      break;
  }
  return proceed;
}

result showChars(menuOut& o,idleEvent e) {
  char box0[]="╭─┬╮";
  char box1[]="│ ││";
  char box2[]="├─┼┤";
  char box3[]="╰─┴╯";
  switch(e) {
    case idling:
      //o<<"len:"<<strlen(box_top);
      o<<box0<<endl;
      o<<box1<<endl;
      o<<box2<<endl;
      o<<box3<<endl;
      break;
    default:
      o<<ANSI::setForegroundColor(WHITE)
        <<ANSI::setBackgroundColor(BLACK)
        <<ANSI::xy(80,25)
        <<ANSI::eraseScreen()
        <<ANSI::xy(1,1);
      break;
  }
  return proceed;
}

void showCharsMap(eventMask event, navNode& nav) {nav.root->idleOn(showChars);}

void showOutColors(eventMask event, navNode& nav) {nav.root->idleOn(showColors);}

//menu definition ------------------------------------
#include "menuDef.hpp"

////////////////////////////////////////////////////////////////////////
// menu navigation root object
NAVROOT(nav,mainMenu,2,Serial,out);

//when menu is suspended -----------------------------------------------
result idle(menuOut& o,idleEvent e) {
  switch(e) {
    //case idleStart:o<<"suspending menu!"<<endl;break;
    case idling:
      o
      <<ANSI::xy(0,0)
      <<ANSI::setBackgroundColor(BLACK)
      <<ANSI::setForegroundColor(WHITE)
      <<"suspended..."<<endl;
      break;
    //case idleEnd:o<<"resuming menu."<<endl;break;
  }
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  /*while(!Serial.available()) {Serial<<".";delay(300);}
  while(Serial.available()) Serial.read();*/
  Serial<<"menu 3.0 test"<<endl;Serial.flush();
  options.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].enabled=disabledStatus;
  nav.showTitle=true;
  //nav.printMenu(1);
  //ansi.fill(1, 1, 2, 2, 'X');
  //Serial<<"pList[0]:{"<<pList[0].x<<","<<pList[0].y<<","<<pList[0].w<<","<<pList[0].h<<"}"<<endl;
  delay(1000);
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}

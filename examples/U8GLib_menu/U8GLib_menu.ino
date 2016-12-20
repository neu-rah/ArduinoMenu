/********************
Arduino generic menu system
U8GLib menu example
U8Glib: https://github.com/olikraus/U8glib_Arduino
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Jul.2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes
*/

#include <menu.h>//menu macros and objects
#include <menuFields.h>
#include <menuU8G.h>

#define U8_DC 9
#define U8_CS 8
#define U8_RST 7

U8GLIB_PCD8544 u8g(U8_CS, U8_DC, U8_RST) ;

menuU8G gfx(u8g,1,0,1,1,1,0,7,9);

bool runMenu=true;

promptFeedback pauseMenu(prompt &p, menuOut &o, Stream &i) {
  runMenu=false;
  o.clear();
  return false;
}

bool test=false;
int param;

TOGGLE(test,onoff_tog,"led: ",
  VALUE("On",HIGH),
  VALUE("Off",LOW)
);

promptFeedback quit() {
  Serial.println("Quiting after action call");
  return true;
}

int selTest=0;
SELECT(selTest,selMenu,"Select",
  VALUE("Zero",0),
  VALUE("One",1),
  VALUE("Two",2)
);

MENU(subMenu,"SubMenu"
  ,OP("TESTING LONG STRINGS",quit)
  ,FIELD(param,"Name","%",0,100,10,1)
  ,OP("B",quit)
);

int adc_prescale=2;
promptFeedback setADCClk() {
  return false;
}

//field value, click to browse, click to choose
CHOOSE(adc_prescale,sample_clock,"Clock"
    ,VALUE("/128",128,setADCClk)
    ,VALUE("/64",64,setADCClk)
    ,VALUE("/32",32,setADCClk)
    ,VALUE("/16",16,setADCClk)
    ,VALUE("/8",8,setADCClk)
    ,VALUE("/4",4,setADCClk)
    ,VALUE("/2",2,setADCClk)
);

MENU(mainMenu,"Main menu"
  ,SUBMENU(selMenu)
  ,SUBMENU(subMenu)
  ,FIELD(param,"Name","%",0,100,10,1)
  ,SUBMENU(sample_clock)
  //,SUBMENU(onoff_tog)
  //,OP("Nothing",prompt::nothing)
  ,OP("Exit",pauseMenu)
);

void setup()   {
  Serial.begin(115200);
  u8g.setFont(u8g_font_unifont);
  menu::wrapMenus=true;
}

void draw(void) {
  if (runMenu) mainMenu.poll(gfx,Serial);
  else {
    u8g.setColorIndex(1);
    u8g.drawStr(0,15,"r-site.net");
    if (Serial.read()==menu::enterCode) runMenu=true;
  }
}

void loop()
{
  u8g.firstPage();
  do {
    draw();
  } while( u8g.nextPage() );
}

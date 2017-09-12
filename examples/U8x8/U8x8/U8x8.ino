#include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
#include <menu.h>
// #include <menuIO/serialOut.h>
//#include <menuIO/U8x8Out.h>
//#include <U8x8lib.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

// #define U8_DC 9
// #define U8_CS 8
// #define U8_RST 7
// #define U8_Width 84
// #define U8_Height 48

//U8X8_PCD8544_84X48_4W_HW_SPI u8x8(U8_CS, U8_DC , U8_RST);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,EXIT("<Back")
);

#define MAX_DEPTH 1

/*MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);*/

/*class SO:public menuOut {
  public:
    Print& device;
    idx_t lastLine=-1;
    inline SO(Print& o,idx_t* t,panelsList &p=default_serial_panel_list,menuOut::styles st=none)
      :menuOut(t,p,st),device(o) {}
    void clear() override {
      println();
      panels.reset();
    }
    void clear(idx_t panelNr) override {
      println();
      panels.nodes[panelNr]=NULL;
    }
    void clearLine(
      idx_t ln,
      idx_t panelNr=0,
      colorDefs color=bgColor,
      bool selected=false,
      status stat=enabledStatus,
      bool edit=false
    ) override {
      lastLine=-1;
      device.println("");
    }
    size_t write(uint8_t ch) override {return device.write(ch);}
    void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
      if (lastLine>=0&&lastLine!=y) println();
      lastLine=y;
    };
};*/

const panel default_serial_panels[] MEMMODE={{0,0,40,10}};
navNode* default_serial_nodes[sizeof(default_serial_panels)/sizeof(panel)];
panelsList default_serial_panel_list(
  default_serial_panels,
  default_serial_nodes,
  sizeof(default_serial_panels)/sizeof(panel)
);

class serialOut:public menuOut {
  public:
    Print& device;
    idx_t lastLine=-1;
    inline serialOut(Print& o,idx_t* t,panelsList &p=default_serial_panel_list,menuOut::styles st=none)
      :menuOut(t,p,(menuOut::styles)(st|menuOut::drawNumIndex)),device(o) {}
    void clear() override {
      println();
      panels.reset();
    }
    void clear(idx_t panelNr) override {
      println();
      panels.nodes[panelNr]=NULL;
    }
    void clearLine(
      idx_t ln,
      idx_t panelNr=0,
      colorDefs color=bgColor,
      bool selected=false,
      status stat=enabledStatus,
      bool edit=false
    ) override {
      lastLine=-1;
      device.println("");
    }
    size_t write(uint8_t ch) override {return device.write(ch);}
    void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {
      if (lastLine>=0&&lastLine!=y) println();
      lastLine=y;
    };
};

//define output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(*(Print*)&Serial,serialTops);

//define outputs controller
//menuOut* const outputs[] MEMMODE={&outSerial};//list of output devices
//outputsList out(outputs,1);//outputs list controller

//define navigation root and aux objects
//navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
//navRoot nav(mainMenu, nav_cursors, MAX_DEPTH-1, Serial, out);

void blink(uint8_t on,uint8_t off=0) {
  digitalWrite(LEDPIN, 1);
  delay(on);
  digitalWrite(LEDPIN, 0);
  delay(off?off:on);
}

void blinks(uint8_t n,uint8_t on=100,uint8_t off=0) {while(n--) blink(on,off?off:on);}

void setup(void)
{
  pinMode(LEDPIN,OUTPUT);
  blinks(1,1500);
  Serial.begin(9600);
  blinks(2,1000);
  while(!Serial);
  blinks(3,500);
  Serial.println("Menu 3.x test");
  blinks(4,100);
  /*u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,0,"Menu 3.x");
  delay(1000);
  char s[2]={0,0};
  int c=32;
  for(int y=0;y<6;y++) {
    for(int x=0;x<10;x++,c++) {
      s[0]=c;
      u8x8.drawString(x,y,s);
    }
  }
  delay(1000);*/
}

void loop(void)
{
  // nav.poll();
  delay(100);
}

/********************
March 2020 M. Smit - github@gangkast.nl

Generic Rotary/Button input
output: SSD1306 OLED
input: clickable rotary encoder

purpose:

  Having a generic rotary event-based implementation,
  leaving rotary and button libraries up to the user.
  Example uses QDEC and AceButton, but could be anything
  that suits your particular hardware and/or needs.

  TODO: userland rotary/button event mapping to menu actions,
  as doubleclick/longpress are now hardcoded to back.
  
***/


#include <menu.h>
#include <menuIO/u8g2Out.h>
#include <menuIO/chainStream.h>
#include <menuIO/rotaryEventIn.h>

// some example libraries to handle the rotation and clicky part
// of the encoder. These will generate our events.
#include <qdec.h> //https://github.com/SimpleHacks/QDEC
#include <AceButton.h> // https://github.com/bxparks/AceButton


// Encoder
const int ROTARY_PIN_A    = 13; // the first pin connected to the rotary encoder
const int ROTARY_PIN_B    = 15; // the second pin connected to the rotary encoder
const int ROTARY_PIN_BUT  = 25;

using namespace ::ace_button;
using namespace ::SimpleHacks;
QDecoder qdec(ROTARY_PIN_A, ROTARY_PIN_B, true); // rotary part
AceButton button(ROTARY_PIN_BUT); // button part
//--//


// Display
// LOLIN32 I2C SSD1306 128x64 display
// https://github.com/olikraus/u8g2
#define SDA 5
#define SCL 4

#include <Wire.h>
#define fontName u8g2_font_7x13_mf
#define fontX 7
#define fontY 16
#define offsetX 0
#define offsetY 3
#define U8_Width 128
#define U8_Height 64
#define USE_HWI2C
#define fontMarginX 2
#define fontMarginY 2
U8G2_SSD1306_128X64_VCOMH0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);//allow contrast change

const colorDef<uint8_t> colors[6] MEMMODE={
  {{0,0},{0,1,1}},//bgColor
  {{1,1},{1,0,0}},//fgColor
  {{1,1},{1,0,0}},//valColor
  {{1,1},{1,0,0}},//unitColor
  {{0,1},{0,0,1}},//cursorColor
  {{1,1},{1,0,0}},//titleColor
};
//--//


// AndroidMenu 
// https://github.com/neu-rah/ArduinoMenu
#define MAX_DEPTH 1

unsigned int timeOn=10;
unsigned int timeOff=90;

using namespace Menu;
MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,1000,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,10000,10,1,Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

RotaryEventIn reIn(
  RotaryEventIn::EventType::BUTTON_CLICKED | // select
  RotaryEventIn::EventType::BUTTON_DOUBLE_CLICKED | // back
  RotaryEventIn::EventType::BUTTON_LONG_PRESSED | // also back
  RotaryEventIn::EventType::ROTARY_CCW | // up
  RotaryEventIn::EventType::ROTARY_CW // down
); // register capabilities, see AndroidMenu MenuIO/RotaryEventIn.h file
MENU_INPUTS(in,&reIn);

MENU_OUTPUTS(out,MAX_DEPTH
  ,U8G2_OUT(u8g2,colors,fontX,fontY,offsetX,offsetY,{0,0,U8_Width/fontX,U8_Height/fontY})
  ,NONE
);
NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);
//--//


// This is the ISR (interrupt service routine) for rotary events
// We will convert/relay events to the RotaryEventIn object
// Callback config in setup()
void IsrForQDEC(void) { 
  QDECODER_EVENT event = qdec.update();
  if (event & QDECODER_EVENT_CW) { reIn.registerEvent(RotaryEventIn::EventType::ROTARY_CW); }
  else if (event & QDECODER_EVENT_CCW) { reIn.registerEvent(RotaryEventIn::EventType::ROTARY_CCW); }

}

// This is the handler/callback for button events
// We will convert/relay events to the RotaryEventIn object
// Callback config in setup()
void handleButtonEvent(AceButton* /* button */, uint8_t eventType, uint8_t buttonState) {

  switch (eventType) {
    case AceButton::kEventClicked:
      reIn.registerEvent(RotaryEventIn::EventType::BUTTON_CLICKED);
      break;
    case AceButton::kEventDoubleClicked:
      reIn.registerEvent(RotaryEventIn::EventType::BUTTON_DOUBLE_CLICKED);
      break;
    case AceButton::kEventLongPressed:
      reIn.registerEvent(RotaryEventIn::EventType::BUTTON_LONG_PRESSED);
      break;
  }
}


void setup() {
  Serial.begin(115200);
  while(!Serial);

  // setup rotary encoder
  qdec.begin();
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), IsrForQDEC, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), IsrForQDEC, CHANGE);

  // setup rotary button
  pinMode(ROTARY_PIN_BUT, INPUT);
  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleButtonEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);

  // setup OLED disaply
  Wire.begin(SDA,SCL);
  u8g2.begin();
  u8g2.setFont(fontName);

  do {
    u8g2.drawStr(0,fontY,"RotaryEventIn demo");
  } while(u8g2.nextPage());
  // appear
  for(int c=255;c>0;c--) {
    u8g2.setContrast(255-255.0*log(c)/log(255));
    delay(12);
  }
    
}

void loop() {
  // put your main code here, to run repeatedly:
  button.check(); // acebutton check, rotary is on ISR
  nav.doInput(); // menu check

  if (nav.changed(0)) {//only draw if menu changed for gfx device
    u8g2.firstPage();
    do nav.doOutput(); while(u8g2.nextPage());
  }

}
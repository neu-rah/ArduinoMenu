/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu output to standard arduino LCD
output: LCD
input: encoder and Serial
www.r-site.net
***/

#include <menu.h>
#include <dev/encoderIn.h>//for PCINT encoder
#include <dev/keyIn.h>//for encoder button
#include <dev/chainStream.h>//for mixing input stream (encoder+button)
#include <dev/lcdOut.h>//for lcd output

using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 8
#define RW 3
#define EN 9
LiquidCrystal lcd(RS, RW, EN, 4, 5, 6, 7);

// Encoder /////////////////////////////////////
#define encA A2
#define encB 2
//this encoder has a button here
#define encBtn A3

encoderIn encoder(encA,encB);//simple quad encoder driver
encoderInStream encStream(encoder,4);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-encBtn,options.getCmdChar(enterCmd)}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

//input from the encoder + encoder button + serial
Stream* inputsList[]={&encStream,&encButton,&Serial};
chainStream<3> in(inputsList);//3 is the number of inputs

void action1() {Serial<<"action1!"<<endl;}

// menu structure definition //////////////////////////////////////////////
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,enterEvent)
  ,OP("Op2",doNothing,noEvent)
  ,OP("Op3",doNothing,noEvent)
  ,EXIT("<Back")
);

lcdOut outLCD(lcd,16,2);//output device for LCD
menuOut* outputs[]={&outLCD};//list of output devices
outputsList out(outputs,1);//outputs list with 2 outputs
NAVROOT(nav,mainMenu,2,in,out);//the navigation root object

//#define debugPin A5

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"Arduino Menu Library"<<endl;Serial.flush();
  encoder.begin();
  pinMode(encBtn,INPUT_PULLUP);
  lcd.begin(16,2);
  int c=colors[optionColorHi][enabled][foreground];
  Serial<<"color:"<<c<<endl;
}

void loop() {
  nav.poll();
  //digitalWrite(LEDPIN, ledCtrl);
  //digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  delay(100);//simulate a delay as if other tasks are running
}

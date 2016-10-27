# ArduinoMenu 3

**AVR generic menu/interactivity system**

[![License: CC BY-NC-SA 4.0](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by-nc-sa/4.0/)
[![Build Status](https://travis-ci.org/neu-rah/ArduinoMenu.svg?branch=dev)](https://travis-ci.org/neu-rah/ArduinoMenu)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/ff2a9215b7644adda60453d850a36aa3)](https://www.codacy.com/app/ruihfazevedo/ArduinoMenu?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=neu-rah/ArduinoMenu&amp;utm_campaign=Badge_Grade)

## Purpose
Full automated navigation system.
With this system you can define menus, submenus, input fields and other iteration objects that deal with all input/output and can call user defined handler as a result of user iteration.
The user function can be operated as a single action called on click/enter or as a event driven function responding to focus In/Out or Enter/Esc events.
The system is designed as a non blocking polling system, allowing parallel task to run.

## Simple Example
```c++
#include <Arduino.h>
#include <menu.h>

#define LEDPIN 13

int timeOn=500;
int timeOff=500;

MENU(mainMenu, "Blink menu", doNothing, noEvent, wrapStyle
  ,FIELD(timeOn,"On","ms",0,1000,100,10,doNothing,noEvent)
  ,FIELD(timeOff,"Off","ms",0,1000,100,10,doNothing,noEvent)
  ,EXIT("<Back")
);

MENU_INPUTS(in,&Serial);//Serial input

serialOut outSerial(Serial);//Serial output

MENU_OUTPUTS(out,&outSerial);//Outputs list
NAVROOT(nav,mainMenu,1,in,out);//navigation root object (nav)

void setup() {
  Serial.begin(115200);
  while(!Serial);
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  nav.poll();//run menu in non-blocking mode
  digitalWrite(LEDPIN, HIGH);
  delay(timeOn);
  digitalWrite(LEDPIN, LOW);
  delay(timeOff);
}
```

## Features
- Small footprint on RAM using PROGMEM on system where it is available.
- Wide variety of input/output devices supported.
- Easy to define menus (macros).
- Minimalistic user code base.
- Fields edit values hooked to existing program variables (references).
- Fields can edit variables of any type (templates).
- Reflexive fields, showing variable changes done outside the menu system.
- Numerical field edit and range validation.
- Enumerated fields for numeric and non-numeric types
- Customizable (colors and cursors).
- Able to work over Serial stream IO as a base level.
- Supports multiple inputs and outputs in parallel.
- static allocation of RAM, avoiding heap fragmentation, all RAM needed to define menu structure is allocated at program statup.
- events available for menus and prompts
- simply returns when no input available and no draw needed.
- lazy drawing, only draws when changed, avoiding time consumption and flicking.

## Menu structure elements (macros)

**MENU** - define a menu

**OP** - define a prompt with associated action function

**FIELD** - numeric field for editing and range validate a numeric variable (int, float, double, unsigned int, etc...)

**SELECT** - define varible value by selecting from an enumerated list of possible values. Click to start, select, click to end.

**TOGGLE** - define varible value by toggling from an enumerated list of possible values. Change value every click.

**CHOOSE** -  define varible value by chosing from an enumerated list of possible values. Enumerated list of values shown as a submenu.

**SUBMENU** - include a menu as submenu or a SELECT|TOGGLE|CHOOSE field.

**EXIT** - quick define special option to leave current menu.

## Dependencies
This library depends on the following libraries:

- Streaming https://github.com/scottdky/Streaming
- Assert4a https://github.com/nettigo/Assert4a

So, be sure to install them too.

Depending on the type of input or output other libraries might be needed. Essentially any library needed for your devices.

## Limits

- when using macros the menu is limited to 16 options (current macro limnit).
- menus **must** have at least 2 options.
- maximum 256 options.
- fast access (numeric keys) only supports 9 options (1 to 9)

## Base

- Character based information display.
- Line based menu organization.
- Stream IO + specializations.

[![IMAGE ALT TEXT](https://img.youtube.com/vi/wHv5sU-HXVI/2.jpg)](https://youtu.be/wHv5sU-HXVI "Arduino menu 2.0 fields video") [![IMAGE ALT TEXT](https://img.youtube.com/vi/W-TRCziF67g/2.jpg)](https://youtu.be/W-TRCziF67g "Arduino menu basic features video")

## IO devices
### Output devices

Serial https://www.arduino.cc/en/Reference/Serial

Standard arduino LCD library  https://www.arduino.cc/en/Reference/LiquidCrystal

F Malpartida's LCDs (ex: i2c LCD) (not yet implemented on v3)
https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

Adafruit's GFX devices
https://github.com/adafruit/Adafruit-GFX-Library

UTFT devices (not yet implemented on v3)
http://www.rinkydinkelectronics.com/library.php?id=51

U8glib devices
https://github.com/olikraus/U8glib_Arduino

Serial ANSI terminal
https://github.com/neu-rah/AnsiStream

### Input devices

Serial https://www.arduino.cc/en/Reference/Serial

quadEncoder - Generic encoder using PCINT (builtin)

Buttons - simple digital keyboard (builtin v2.x)

Generic keyboard (no PCINT) - configurable for digital or analog keyboards (v2.x)

ClickEncoder https://github.com/0xPIT/encoder (not yest implemented on v3)

## Menu definition example
example of menu definition (c++ macros)

```c++
//a submenu
TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;//SELECT will link to this variable and change it as needed
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;//CHOOSE will link to this variable and change it as needed
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

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

```
## History

### 3.0
	- complete revision of menu control system
	- menu structure separated in ram objects and flash objects
	- using separate navigation control objects **navNode**
	- central navigation control object **navRoot**
	- using event for all menu objects (focus, blur, enter, exit).
	- multiple panels
	- color enumeration at base level
	- multiple output devices in parallel
	- define exit as regular option

### 2.4
- support for teensy (tested on 3.2)
- new field type SELECT
- reflexivity, fields reflect external changes to values
- store field strings to progmem
- automatic use of RAM on system without PROGMEM

### 2.3

- action functions now need to return bool (only affects menus)

>**false** = continue menu
>
>**true** = exit menu

- Support for U8GLib screens
- alternative use ClickEncoder
- using flash memory to store menu strings and lists (PROGMEM)

### 2.0

- non-blocking menu main cycle
- Menufields as menu prompts with associated value
  values can be:
    numeric withing range
    list of values toggled on click (for small lists)
    list of values selected as submenu (for longer lists)
- PCINT now supports Mega/2560 and possibly others

### 1.x
- basic menu functionality

## notes

encoder now needs begin() to be called on setup

input is read from generic streams, included simple streams for encoders and keyboards
	- provided encoder driver uses internal pull-ups and reverse logic

multiple stream packing for input to mix encoder stream with encoder keyboard (usually 1 or 2 keys)

## info
more info at

[wiki](https://github.com/neu-rah/ArduinoMenu/wiki) pages, [issues](https://github.com/neu-rah/ArduinoMenu/issues?utf8=%E2%9C%93&q=) or [r-site.net](http://www.r-site.net/?lang=en&at=//op%5B@id=%273090%27%5D)

## Repots
please report errors, problems or enhancement ideas, I apreciate the feedback. Thanks.

On issues report please specify the input and output drivers or devices.

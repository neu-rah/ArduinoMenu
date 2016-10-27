**AVR generic menu/interactivity system**

[![License: CC BY-NC-SA 4.0](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by-nc-sa/4.0/)
[![Build Status](https://travis-ci.org/neu-rah/ArduinoMenu.svg?branch=dev)](https://travis-ci.org/neu-rah/ArduinoMenu)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/ff2a9215b7644adda60453d850a36aa3)](https://www.codacy.com/app/ruihfazevedo/ArduinoMenu?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=neu-rah/ArduinoMenu&amp;utm_campaign=Badge_Grade)

## Purpose
Full automated navigation system calling user funtions.
@@ -12,6 +12,44 @@ With this system you can define menus, submenus, input fields and other iteratio
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

MENU_INPUTS(in,&Serial);

serialOut outSerial(Serial);//,serial_panels);//the output device (just the serial port)

MENU_OUTPUTS(out,&outSerial);
NAVROOT(nav,mainMenu,1,in,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, HIGH);
  delay(timeOn);
  digitalWrite(LEDPIN, LOW);
  delay(timeOff);
}
```

## Features
- Small footprint on RAM using PROGMEM on system where it is available.
- Wide variety of input/output devices supported.
@@ -21,13 +59,33 @@ The system is designed as a non blocking polling system, allowing parallel task
- Fields can edit variables of any type (templates).
- Reflexive fields, showing variable changes done outside the menu system.
- Numerical field edit and range validation.
- Enumerated fields for numeric and non-numeric types
- Customizable (colors and cursors).
- Able to work over Serial stream IO as a base level.
- Supports multiple inputs and outputs in parallel.
- static allocation of RAM, avoiding heap fragmentation, all RAM needed to define menu structure is allocated at program statup.
- events available for menus and prompts
- simply returns when no input available and no draw needed
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

@@ -36,12 +94,14 @@ This library depends on the following libraries:

So, be sure to install them too.

Depending on the type of input or output other libraries might be needed. Essentially any library needed for your devices.

## Limits

- when using macros the menu is limited to 16 options (current macro limnit).
- menus **must** have at least 2 options.
- maximum 256 options
- fast access only supports 9 options (1 to 9)
- maximum 256 options.
- fast access (numeric keys) only supports 9 options (1 to 9)

## Base

@@ -58,276 +118,102 @@ Serial https://www.arduino.cc/en/Reference/Serial

Standard arduino LCD library  https://www.arduino.cc/en/Reference/LiquidCrystal

F Malpartida's LCDs (ex: i2c LCD)
F Malpartida's LCDs (ex: i2c LCD) (not yet implemented on v3)
https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

Adafruit's GFX devices
https://github.com/adafruit/Adafruit-GFX-Library

UTFT devices
UTFT devices (not yet implemented on v3)
http://www.rinkydinkelectronics.com/library.php?id=51

U8glib devices
https://github.com/olikraus/U8glib_Arduino

Serial ANSI terminal
https://github.com/neu-rah/AnsiStream

### Input devices

Serial https://www.arduino.cc/en/Reference/Serial

quadEncoder - Generic encoder using PCINT
quadEncoder - Generic encoder using PCINT (builtin)

Buttons - simple digital keyboard
Buttons - simple digital keyboard (builtin v2.x)

Generic keyboard (no PCINT) - configurable for digital or analog keyboards
Generic keyboard (no PCINT) - configurable for digital or analog keyboards (v2.x)

ClickEncoder https://github.com/0xPIT/encoder
ClickEncoder https://github.com/0xPIT/encoder (not yest implemented on v3)

## Menu definition example
example of menu definition (c++ macros)
partial alternative input system

this must be rethinked in depth, for now we will stick to input streams
Rui Azevedo
authored10/27/2016 @ 3:58 AM
commit:8e0f9d
parent:f3cf0d
8 modified
Name
Full Path
README.md
library.json
platformio.ini
src/dev
/encoderIn.hpp
src
/menu.cpp
src
/menu.hpp
src
/menuBase.cpp
src
/menuBase.hpp
File History: README.md
Diff View
File View
Show blame details
partial alternative input system
44 minutes ago by Rui Azevedo
8e0f9d
travis build badge
11 hours ago by Rui Azevedo
f3cf0d
docs
13 hours ago by Rui Azevedo
9e431d
readme...
18 hours ago by Rui Azevedo
b8860f
preview prototype working
5 days ago by Rui Azevedo
3059ed
lcd menu working ok
09/25/2016 by Rui Azevedo
ed77d5
transoposed local to v3 repo
09/24/2016 by Rui Azevedo
e72267
auto selection of pgm mode if available for board
08/04/2016 by Rui Azevedo
ad0188
deactivated non pgm modes due to bugs...
08/03/2016 by Rui Azevedo
7e2118
conditional use of pgm_read_byte_near
08/03/2016 by Rui Azevedo
e556fd
PROGMEM usage is conditional now
08/03/2016 by Rui Azevedo
7c4751
fmt
08/02/2016 by Rui Azevedo
2a4a30
linked choose to main menu example
08/02/2016 by Rui Azevedo
f06c87
2.4
08/02/2016 by Rui Azevedo
7db106
doc format
08/02/2016 by Rui Azevedo
9b7674
1 more video
08/02/2016 by Rui Azevedo
c59471
video small image
08/02/2016 by Rui Azevedo
2e09ee
doc video
08/02/2016 by Rui Azevedo
dbea98
docs -> more info
08/02/2016 by Rui Azevedo
459645
doc review
08/02/2016 by Rui Azevedo
0aaa20
doc format
08/02/2016 by Rui Azevedo
3c5842
DOC REVIEW
08/02/2016 by Rui Azevedo
cb2094
added clickencoder support
07/28/2016 by Rui Azevedo
28f209
fixing https://github.com/neu-rah/ArduinoMenu/issues/30
07/21/2016 by Rui Azevedo
29e3ce
Update README.md
04/18/2016 by Rui Azevedo
cab794
fixed ATmega328 pcint tables
12/06/2014 by neu-rah
14657b
LCD refresh fixed
11/28/2014 by neu-rah
241a59
Added support for Francisco Malpartida LCD's
09/18/2014 by neu-rah
ca1422
implementing menu width foir GFX consistent selection draw
08/31/2014 by neu-rah
7d7614
typo fix
08/20/2014 by neu-rah
634aa3
doc update
08/19/2014 by neu-rah
47a69f
Update README.md
08/18/2014 by Rui Azevedo
d62393
Update README.md
08/16/2014 by Rui Azevedo
e3858c
Initial commit
08/16/2014 by Rui Azevedo
4fca48
ADDED README.md
End of History
9e431d
Rui Azevedo
1
# ArduinoMenu 3
e3858c
Rui Azevedo
2
ed77d5
Rui Azevedo
3
**AVR generic menu/interactivity system**
4
9e431d
Rui Azevedo
5
[![License: CC BY-NC-SA 4.0](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by-nc-sa/4.0/)
f3cf0d
Rui Azevedo
6
[![Build Status](https://travis-ci.org/neu-rah/ArduinoMenu.svg?branch=dev)](https://travis-ci.org/neu-rah/ArduinoMenu)
8e0f9d
Rui Azevedo
7
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/ff2a9215b7644adda60453d850a36aa3)](https://www.codacy.com/app/ruihfazevedo/ArduinoMenu?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=neu-rah/ArduinoMenu&amp;utm_campaign=Badge_Grade)
f3cf0d
Rui Azevedo
8
ed77d5
Rui Azevedo
9
## Purpose
e72267
Rui Azevedo
10
Full automated navigation system calling user funtions.
11
With this system you can define menus, submenus, input fields and other iteration objects that deal with all input/output and can call user defined handler as a result of user iteration.
12
The user function can be operated as a single action called on click/enter or as a event driven function responding to focus In/Out or Enter/Esc events.
13
The system is designed as a non blocking polling system, allowing parallel task to run.
14
8e0f9d
Rui Azevedo
15
## Simple Example
16
```c++
17
#include <Arduino.h>
18
#include <menu.h>
19
20
#define LEDPIN 13
21
22
int timeOn=500;
23
int timeOff=500;
24
25
MENU(mainMenu, "Blink menu", doNothing, noEvent, wrapStyle
26
  ,FIELD(timeOn,"On","ms",0,1000,100,10,doNothing,noEvent)
27
  ,FIELD(timeOff,"Off","ms",0,1000,100,10,doNothing,noEvent)
28
  ,EXIT("<Back")
29
);
30
31
MENU_INPUTS(in,&Serial);
32
33
serialOut outSerial(Serial);//,serial_panels);//the output device (just the serial port)
34
35
MENU_OUTPUTS(out,&outSerial);
36
NAVROOT(nav,mainMenu,1,in,out);
37
38
void setup() {
39
  Serial.begin(115200);
40
  while(!Serial);
41
  pinMode(LEDPIN, OUTPUT);
42
}
43
44
void loop() {
45
  nav.poll();
46
  digitalWrite(LEDPIN, HIGH);
47
  delay(timeOn);
48
  digitalWrite(LEDPIN, LOW);
49
  delay(timeOff);
50
}
51
```
52
cb2094
Rui Azevedo
53
## Features
b8860f
Rui Azevedo
54
- Small footprint on RAM using PROGMEM on system where it is available.
cb2094
Rui Azevedo
55
- Wide variety of input/output devices supported.
e72267
Rui Azevedo
56
- Easy to define menus (macros).
cb2094
Rui Azevedo
57
- Minimalistic user code base.
e72267
Rui Azevedo
58
- Fields edit values hooked to existing program variables (references).
59
- Fields can edit variables of any type (templates).

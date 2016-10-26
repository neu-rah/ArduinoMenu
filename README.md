# ArduinoMenu 3

**AVR generic menu/interactivity system**

[![License: CC BY-NC-SA 4.0](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by-nc-sa/4.0/)

## Purpose
Full automated navigation system calling user funtions.
With this system you can define menus, submenus, input fields and other iteration objects that deal with all input/output and can call user defined handler as a result of user iteration.
The user function can be operated as a single action called on click/enter or as a event driven function responding to focus In/Out or Enter/Esc events.
The system is designed as a non blocking polling system, allowing parallel task to run.

## Features
- Small footprint on RAM using PROGMEM on system where it is available.
- Wide variety of input/output devices supported.
- Easy to define menus (macros).
- Minimalistic user code base.
- Fields edit values hooked to existing program variables (references).
- Fields can edit variables of any type (templates).
- Reflexive fields, showing variable changes done outside the menu system.
- Numerical field edit and range validation.
- Customizable (colors and cursors).
- Able to work over Serial stream IO as a base level.
- static allocation of RAM, avoiding heap fragmentation, all RAM needed to define menu structure is allocated at program statup.
- events available for menus and prompts
- simply returns when no input available and no draw needed
- lazy drawing, only draws when changed, avoiding time consumption and flicking.

## Dependencies
This library depends on the following libraries:

- Streaming https://github.com/scottdky/Streaming
- Assert4a https://github.com/nettigo/Assert4a

So, be sure to install them too.

## Limits

- when using macros the menu is limited to 16 options (current macro limnit).
- menus **must** have at least 2 options.
- maximum 256 options
- fast access only supports 9 options (1 to 9)

## Base

- Character based information display.
- Line based menu organization.
- Stream IO + specializations.

[![IMAGE ALT TEXT](https://img.youtube.com/vi/wHv5sU-HXVI/2.jpg)](https://youtu.be/wHv5sU-HXVI "Arduino menu 2.0 fields video") [![IMAGE ALT TEXT](https://img.youtube.com/vi/W-TRCziF67g/2.jpg)](https://youtu.be/W-TRCziF67g "Arduino menu basic features video")

## IO devices
### Output devices

Serial https://www.arduino.cc/en/Reference/Serial

Standard arduino LCD library  https://www.arduino.cc/en/Reference/LiquidCrystal

F Malpartida's LCDs (ex: i2c LCD)
https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

Adafruit's GFX devices
https://github.com/adafruit/Adafruit-GFX-Library

UTFT devices
http://www.rinkydinkelectronics.com/library.php?id=51

U8glib devices
https://github.com/olikraus/U8glib_Arduino

### Input devices

Serial https://www.arduino.cc/en/Reference/Serial

quadEncoder - Generic encoder using PCINT

Buttons - simple digital keyboard

Generic keyboard (no PCINT) - configurable for digital or analog keyboards

ClickEncoder https://github.com/0xPIT/encoder

## Menu definition example
example of menu definition (c++ macros)

```c++
//a submenu
MENU(ledMenu,"LED on pin 13",
	OP("LED On",ledOn),
	OP("LED Off",ledOff)
);

//field value toggle on click
TOGGLE(targetVar,trigModes,"Mode: ",
	VALUE("None",trigPoint::None),
	VALUE("On rise",trigPoint::onRise),
	VALUE("On fall",trigPoint::onFall),
	VALUE("Both",trigPoint::isIn)
);

//field value enter edit on click, rotate to change, click to end edit
SELECT(selTest,selMenu,"Select",
  VALUE("Zero",0),
  VALUE("One",1),
  VALUE("Two",2)
);

//field value, click to browse, click to choose
CHOOSE(adc_prescale,sample_clock,"Sample clock",
	VALUE("/128",avrADC::clk_128,setADCClk),
	VALUE("/64",avrADC::clk_64,setADCClk),
	VALUE("/32",avrADC::clk_32,setADCClk),
	VALUE("/16",avrADC::clk_16,setADCClk),
	VALUE("/8",avrADC::clk_8,setADCClk),
	VALUE("/4",avrADC::clk_4,setADCClk),
	VALUE("/2",avrADC::clk_2,setADCClk)
);

//the main menu...
//Fields are numeric and show selected value. click to start change, click to fine tune, click to end
MENU(mainMenu,"Main menu",
	FIELD(frequency,"Freq","Hz",0,16000000,100,1,updateFreq),
	FIELD(dutty,"Duty","%",0,100,1,0,updateDutty),
	OP("Handler test",completeHandlerTest),
	SUBMENU(ledMenu),
	SUBMENU(trigModes),
	SUBMENU(selMenu),
	SUBMENU(sample_clock)
);
```

## syntax

#### Option
```c++
OP(name,function)
```
Menu option, call user function on click

**name**: string to be shown as menu option prompt

**function**: to be called on click

#### Numeric field
```c++
FIELD(variable,name,units,min,max,step,tune,function)
```

Holding and changing numeric values

**variable**: holding the value (must be numeric or support comparison oprators)

**name**: to use as prompt

**units**: to be shown after value

**min,max**: defining numeric value range

**step**: increment/decrement when adjusting value

**tune**: value to increment/decrement when fine tunning
the value

**function**: called on every value change

#### Field value
```c++
VALUE(text,value [,action] )
```

holding possible FIELD values

**text**: to be used as prompt

**value**: to be passed when selected

**action**: optional function to be called on activation

#### Toggle field value
```c++
TOGGLE(variable,id,name,
	VALUE(...),
	...,
	VALUE(...)
)
```

Holding a value and a list of possible values to toggle on click. This is ideal for On/Off Yes/No and other small list of values

**variable**: holding the value

**id**: of this element to be used with SUBMENU

**name**: to be used as prompt

#### Select field value
```c++
SELECT(variable,id,name,
	VALUE(...),
	...,
	VALUE(...)
)
```

define a value from a list of possibilities
click to enter edit mode
rotate to choose value
click to exit edit mode

**variable**: holding the value

**id**: of this element to be used with SUBMENU

**name**: to be used as prompt

#### Choose field value
```c++
CHOOSE(variable,id,name,
	VALUE(...),
	...,
	VALUE(...)
)
```

Holding a value and a list of possible values to select as a submenu. This is ideal for longer lists of values.

**variable**: holding the value

**id**: of this element to be used with SUBMENU

**name**: to be used as prompt

#### Submenu
```c++
SUBMENU(id)
```

link in a submenu as option of the current one

**id**: of element to be used as a submenu, can be a menu, toggle, select or choose

#### Menu
```c++
MENU(id,name,
	...
	OP(...),
	FIELD(...),
	SUBMENU(...),
	...
)
```

define menu structure

**id**: this menu id

**name**: menu name to use as submenu title

## Saving RAM

By default PROGMEM is used on platforms where it is available.

## Include files

**chainStream.h** - join multiple input stream to be used as one, this is used to join quadEncoder stream with a keyboard (encoder button).

**ClickEncoderStream.h** - using alternative encoder https://github.com/0xPIT/encoder

**genericKeyboard.h** - make a custom keyboard by providing a reader function.

**keyStream.h** - simple digital keyboard driver doing digitalRead of pins. this driver can miss clicks on heavy load.

_**macros.h** - macro definitions to help building complex menu structure (included by menu.h)_

**menuFields.h** - allows the menu system to alter/monitor your variables, this file defines the behavior of FIELD, TOGGLE, SELECT and CHOOSE.

**menuGFX.h** - use an Adafruit-GFX-Library compatible screen https://learn.adafruit.com/adafruit-gfx-graphics-library

**menu.h** - basic menu functionality

**menuLCD.h** - use the standard arduino LiquidCrystal screen https://www.arduino.cc/en/Tutorial/HelloWorld

**menuLCDs.h** - use alternative LCD`s https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/schematics#!hardware-configurations-and-initialization

**menuPrint.h** - output to Serial stream numbering, options for quick access

**menuU8G.h** - use a U8Glib compatible screen https://github.com/olikraus/u8glib/wiki/device

**menuUTFT.h** - use UTFT compatible screen http://www.rinkydinkelectronics.com/library.php?id=51

**menuUTouch.h** - touch screen input http://henningkarlsen.com/electronics/library.php?id=56

quadEncoder.h - basic quad-encoder driver using pin change interrupt. Button should be added as a keyboard.1

## API

This API is needed only to extend the menu. For regular usage only the poll function needs to be called.

### menu object functions

```c++
virtual void printTo(navRoot &root,bool sel,menuOut& out);
```

raw print to output device, each object can do its specialized print (one line)

```c++
virtual bool canNav() const;
```
can receive navigation focus and process keys

```c++
virtual bool isMenu() const;
```

has menu data list and can be a navNode target

```c++
virtual bool changed(const navNode &nav,const menuOut& out);
```
object has changed and need redraw

```c++
virtual result activate(FUNC_PARAMS) {return proceed;}
```
this is the system version of enter handler, its used by elements like toggle.


## History

### 3.0
	- complete revision of menu control system
	- menu structure separated in ram objects and flash objects
	- using separate navigation control objects **navNode**
	- central navigation control object **navRoot**
	- using event for all menu objects

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

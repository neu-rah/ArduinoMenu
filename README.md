ArduinoMenu
===========
AVR generic menu/interactivity system

Easy to define menu system with sub-menus and associated function to call
Works either over serial or with LCD + encoder

## Features

- Wide variety of input/output devices supported.
- Low memory usage, strings and options list stored to PROGMEM
- Easy to define menus.
- Minimalistic user code base.
- Field to edit values hooked to existing program variables.
- Fields can edit variables of any type.
- Reflexive fields, showing variable changes (experimental).
- Numerical field edit and range validation.
- User functions called on regular options or field edit.
- Attachable functions to menu enter (experimental).
- Customizable (colors and cursors).
- Able to work over Serial stream for regular or debug mode.

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

Generic encoder using PCINT - quadEncoder

Generic keyboard (no PCINT) - configurable for digital or analog keyboards

ClickEncoder
https://github.com/0xPIT/encoder

more details on wiki and issues discussions

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
VALUE(text,value)
```

holding possible FIELD values

**text**: to be used as prompt

**value**: to be passed when selected

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

**id**: the submenu id

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

## History

### 2.4
- new field type SELECT
- reflexivity, field reflect external changes to values
- field string to progmem

### 2.3

- actions functions need to return bool now (only affects menus)

		false = continue menu
		true = exit menu

- Support for U8GLib screens
- alternative use of ClickEncoder
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

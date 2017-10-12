# ArduinoMenu 4

**Generic menu/interactivity system for the arduino framework**

[![License: CC BY-NC-SA 4.0](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg)](https://spdx.org/licenses/CC-BY-NC-SA-4.0.html)
[![Build Status](https://travis-ci.org/neu-rah/ArduinoMenu.svg?branch=master)](https://travis-ci.org/neu-rah/ArduinoMenu)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/ruihfazevedo)
[![Join Gitter](https://img.shields.io/badge/GITTER-join%20chat-green.svg)](https://gitter.im/ArduinoMenu/Lobby)

## Purpose
Full automated or user code driven navigation system.
With this system you can define menus, submenus, input fields and other iteration objects that deal with all input/output and can call user defined handler as a result of user iteration.
The user function can be operated as a single action called on click/enter or as a event driven function responding to focus In/Out or Enter/Esc events.
The system is designed as a non blocking polling system, allowing concurrent task to run.
Optionally the system can be operated in semi-automated mode, issuing navigation command from user code.

See the [wiki](https://github.com/neu-rah/ArduinoMenu/wiki)

## Simple Example
```c++
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN
#define MAX_DEPTH 1

int timeOn=10;
int timeOff=90;

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,100,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,100,10,1,Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

serialIn serial(Serial);
MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
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
- Easy to define menus (macros).
- Minimalistic user code base.
- Fields edit values hooked to existing program variables (references).
- Editing text fields with per character validation.
- Fields can edit numeric variables of any type (templates).
- Reflexive fields, showing variable changes done outside the menu system.
- Numerical field edit and range validation.
- Enumerated fields for numeric and non-numeric types
- Customizable (colors and cursors).
- Able to work over Serial stream IO as a base level.
- Supports multiple inputs and outputs in parallel.
- static allocation of RAM, avoiding heap fragmentation, all RAM needed to define menu structure is allocated at program startup.
- events available for menus and prompts
- simply returns when no input available and no needed to draw.
- lazy drawing, only draws when changed, avoiding time consumption and flicking.
- sync/async navigation API functions
- async navigation for stateless clients (as web)
- web interface (experimental)
- can be confined to a display area (numeric fields can still overflow the area, user should take account for them)
- Tested on Arduino: AVR, ARM, Teensy 3.2, ESP8266

## Version 2.x videos
[![IMAGE ALT TEXT](https://img.youtube.com/vi/wHv5sU-HXVI/2.jpg)](https://youtu.be/wHv5sU-HXVI "Arduino menu 2.0 fields video") [![IMAGE ALT TEXT](https://img.youtube.com/vi/W-TRCziF67g/2.jpg)](https://youtu.be/W-TRCziF67g "Arduino menu basic features video")

## Applications

**Fielduino** hardware PWM generator using menu to select frequency and dutty.

https://github.com/neu-rah/Fielduino

## Platforms
Atmel AVR, Atmel SAM, Espressif 8266, Intel ARC32, Microchip PIC32, Nordic nRF51, Teensy, TI MSP430

## Install

**Arduino IDE** - Install using the library manager (ArduinoMenu library - Rui Azevedo)

**Platformio** - http://platformio.org/lib/show/1468/ArduinoMenu%20library

## IO devices
### Output devices

**Serial**

https://www.arduino.cc/en/Reference/Serial

- Serial monitor.

**Standard arduino LCD library**

https://www.arduino.cc/en/Reference/LiquidCrystal

- hd44780

**F Malpartida's LCDs**

https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

- HD44780 direct
- HD44780 I2C
- HD44780 SPI

**Matthias Hertel PCF8574 driver**

https://github.com/mathertel/LiquidCrystal_PCF8574hd44780

- HD44780

**Arduino TFT devices**

https://www.arduino.cc/en/Guide/TFT

**Adafruit's GFX devices**

https://github.com/adafruit/Adafruit-GFX-Library

- RGBmatrixPanel, 16x32 and 32x32 RGB LED matrix panels.
- Adafruit_TFTLCD, 2.8" TFT LCD touchscreen breakout and TFT Touch Shield for Arduino.
- Adafruit_HX8340B, 2.2" TFT Display with microSD.
- Adafruit_ST7735, 1.8" TFT Display with microSD.
- Adafruit_PCD8544, for the Nokia 5110/3310 monochrome LCD.
- Adafruit-Graphic-VFD-Display-Library, for 128x64 Graphic VFD.
- Adafruit-SSD1331-OLED-Driver-Library-for-Arduino for the 0.96" 16-bit Color OLED w/microSD Holder.
- Adafruit_SSD1306 for the Monochrome 128x64 and 128x32 OLEDs.

**UTFT devices**

http://www.rinkydinkelectronics.com/library.php?id=51

- ITDB02 series - ITead Studio
- TFT01 series - ElecFreaks
- RGB LCD - NKC Electronics.

**U8glib devices**

https://github.com/olikraus/U8glib_Arduino

SSD1306, 128x64, SSD1306, 128x32, SSD1306, 64x48, SSD1309, 128x64, SSD1322, 256x64, SSD1325, 128x64, SSD1327, 96x96, SH1106, 128x64, UC1601, 128x32, UC1608, 240x64, UC1610, 160x104, UC1611, 240x64, UC1611, 240x128, UC1701, 128x64, UC1701, 102x64, ST7565, 128x64, ST7565, 128x32, ST7920, 128x64, ST7920, 192x32, ST7920, 202x32, LD7032, 60x32, PCD8544, 84x48, TLS8204 , 84x48, PCF8812 , 96x65, KS0108 , 128x64, T6963, 128x64, T6963, 128x128, T6963, 240x64, T6963, 240x128, SBN1661, 122x32, LC7981, 160x80, LC7981, 240x64, LC7981, 240x128, LC7981, 320x64, SSD1351, HT1632, 24x16, Flipdisk, 28x14, Virtual Screen Device

**U8G2 and U8x8 devices**

https://github.com/olikraus/U8g2_Arduino

SSD1305 128X32_NONAME, SSD1305 128X32_NONAME, SSD1306 128X64_NONAME, SSD1306 128X64_VCOMH0, SSD1306 128X64_NONAME, SSD1306 128X64_VCOMH0, SH1106 128X64_NONAME, SH1106 128X64_VCOMH0, SH1106 128X64_NONAME, SH1106 128X64_VCOMH0, SSD1306 128X32_UNIVISION, SSD1306 128X32_UNIVISION, SSD1306 64X48_ER, SSD1306 64X48_ER, SSD1306 64X32_NONAME, SSD1306 64X32_NONAME, SSD1306 96X16_ER, SSD1306 96X16_ER, SSD1309 128X64_NONAME2, SSD1309 128X64_NONAME2, SSD1309 128X64_NONAME0, SSD1309 128X64_NONAME0, SSD1325 NHD_128X64, SSD1325 NHD_128X64, SSD1327 SEEED_96X96, SSD1327 SEEED_96X96, SSD1329 128X96_NONAME, LD7032 60X32, LD7032 60X32, ST7920 192X32, ST7920 192X32, ST7920 192X32, ST7920 128X64, ST7920 128X64, ST7920 128X64, LS013B7DH03 128X128, UC1701 EA_DOGS102, UC1701 MINI12864, PCD8544 84X48, PCF8812 96X65, UC1604 JLX19264, UC1604 JLX19264, UC1608 ERC24064, UC1608 ERC24064, UC1608 ERC240120, UC1608 ERC240120, UC1608 240X128, UC1608 240X128, UC1610 EA_DOGXL160, UC1610 EA_DOGXL160, UC1611 EA_DOGM240, UC1611 EA_DOGM240, UC1611 EA_DOGXL240, UC1611 EA_DOGXL240, ST7565 EA_DOGM128, ST7565 64128N, ST7565 ZOLEN_128X64, ST7565 LM6059, ST7565 ERC12864, ST7565 NHD_C12864, ST7565 NHD_C12832, ST7565 EA_DOGM132, ST7567 PI_132X64, ST7588 JLX12864, ST7588 JLX12864, NT7534 TG12864R, IST3020 ERC19264, SBN1661 122X32, SED1520 122X32, KS0108 128X64, KS0108 ERM19264, LC7981 160X80, LC7981 160X160, LC7981 240X128, T6963 240X128, T6963 240X64, T6963 256X64, T6963 128X64, SSD1322 NHD_256X64, SSD1606 172X72, SSD1607 200X200, IL3820 296X128, IL3820 V2_296X128, SED1330 240X128, RA8835 NHD_240X128, RA8835 320X240, MAX7219 32X8, A2PRINTER 384X240,

**UCGLib devices**

https://github.com/olikraus/Ucglib_Arduino

ST7735, ILI9341, PCF8833, SSD1351, LD50T6160 (v1.2), ILI9163 (v1.2), SSD1331 (v1.3), SEPS225 (v1.3)

**Serial ANSI terminal**

https://github.com/neu-rah/AnsiStream

- Unix terminal

**Web browser** _(experimental)_

- ESP8266 Web Server

### Input devices

Serial https://www.arduino.cc/en/Reference/Serial

quadEncoder - Generic encoder using PCINT (builtin)

Buttons - simple digital keyboard (builtin)

Generic keyboard (no PCINT) - configurable for digital or analog keyboards (builtin)

ClickEncoder https://github.com/0xPIT/encoder

User defined input calling menu navigation API

Web browser (experimental) when using ESP devices

## Dependencies
This library depends on the following libraries:

- Streaming https://github.com/scottdky/Streaming (on debug mode)
- Assert4a https://github.com/nettigo/Assert4a (on debug mode)

Depending on the type of input or output, other libraries might be needed. Essentially any library needed for your devices.

## Limits

- when using macros the menu is limited to 16 options (current macro limnit).
- menus **must** have at least 2 options.
- maximum 127 options.
- fast access (numeric keys) only supports 9 options (1 to 9)
- prompts can overflow on panels with less than 4 characters width
- menu system is character based, so choose monometric font to achieve best results, it will work with any font but the text can overflow.

## Base

- Character based information display.
- Line based menu organization.
- Stream IO + specializations.

## info

more info at

[wiki](https://github.com/neu-rah/ArduinoMenu/wiki) pages, [issues](https://github.com/neu-rah/ArduinoMenu/issues?utf8=%E2%9C%93&q=) or [r-site.net](http://www.r-site.net/?lang=en&at=//op%5B@id=%273090%27%5D)

## Errors / Sugestions

Please report errors, problems or enhancement ideas, I apreciate the feedback. Thanks.

On issues report please specify the input and output drivers or devices as well as target platform.

## notes

encoder now needs begin() to be called on setup (2.x)

input is read from generic streams, included simple streams for encoders and keyboards
	- provided encoder driver uses internal pull-ups and reverse logic

multiple stream packing for input to mix encoder stream with encoder keyboard (usually 1 or 2 keys)

## History

### 4.0
  - More examples
  - Text edit fields with validation *
  - Pad style menus (horizontal list)
  - inline pad menus can input custom formats like IP/Date/Time, experimental and just for devices that can position a cursor 
  - Plugins, alternative menu items potentially device specific
  - Allow multiple concurrent menus
  - Support UCGLib displays
  - Support U8x8 displays
  - Support Arduino TFT
  - Support PCF8574 I2C LCD
  - Support Analog Joystick input
  - Added input burst config option
  - VALUEOBJ macro, user allocated values
  - menuIn class for menu inputs (allows device field invertion) *
  - some options have been distributed to some other classes
  - allow dynamic menu construction (ram) even for AVR's

#### * API changes

##### Options

**invertFieldKeys** option removed, invertion is now supported by specific menuIn objects.

**numValueInput** moved to menuIn object

**navRoot** extra options, previously on global options object
```c++
bool nav2D=false;//not used
bool canExit=true;//v4.0 moved from global options
bool useUpdateEvent=false;//if false, use enterEvent when field value is changed.
idx_t inputBurst=1;//limit of inputs that can be processed before output
```

##### Serial input

Serial is no longer accepted as an input directly, use serialIn instead

##### prompt base API

printTo member function changed from:

```c++
Used printTo(navRoot&,bool,menuOut&,idx_t,idx_t);
```

to

```c++
Used printTo(navRoot&,bool,menuOut&,idx_t,idx_t,idx_t=0);
```

_this should only affect customized components_

inputs are now from class menuIn, derived from Stream but including flag functions for field axis inversion per input class.

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
	- limiting text width to prevent overflow

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

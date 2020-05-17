# Menu API

_under development_

**things here migh be outdated!**


## Some concepts

**Navigation root** - base onbject to control navigation state over an attributed menu structure.

**Active item** - the item that will receive and dispatch function calls, usually a container.

**Focus/selected item** - an active item child element indexed by the current root path level index.

## Base

Base definitions `"menu/base.h"`

**Idx** - adequate index type on platform (can be: `uint8_t`,`size_t`,_etc..._)

**Modes** - Field printing mode

```c++
enum class Modes {Normal,Edit,Tune};
```

**Cmd** - navigation commands

```c++
enum class Cmd:Idx {None=0,Enter=1,Esc=2,Up=4,Down=8,Left=16,Right=32};
```

**Roles** - printing parts
```c++
enum class Roles:Idx {Raw,Panel,Menu,Title,Body,Item,Index,Cursor,Name,Mode,Value,Unit};
```

**Op** - reusing print routines for measure and change tracking

```c++
enum class Op {Measure,Printing,ClearChanges};
```

## APICall walkers

walkers traverse the structure trasnporting info about a funtion to be called at active item, the active element can then call it on te focus/slected child item.

**APICall walkers**

`Cmd`, `Activate`, `Enable`, `Enabled`, `Size`, `PrintMenu`

## Input composition API

Input is a composition chain

**Quiet** is the base for in0p0ut API

`parseKey` - read an atributed input method and call the chain or navigation for reactions.

`parseCmd` - parse a key code and call navigation functions in response or chain the key code.

## Output composition API

**Void** is the base for output API

`raw` - output some data

`newView` - start a new page render

`partialDraw` this device can draw part of the screen

`lastDrawn` set last drawn menu

`isSame` check if given is same as lastDrawn set

`fullDraw` this device always draw a full page render

`top` current menu item index to be rendered at yop of page (scroll control)

`posY` printing Y position in device coordinates

`posY` printing X position in device coordinates

`freeY` remaining free space on Y axis

`freeX` remaining free space on X axis

`orgX` X displacement

`orgY` Y displacement

`width` output total width

`height` output total height

`setCursor` set printiung position, may be ignoresd by device that can not do so, or even replaced by another operation (like newline)

`ascent` device font alignment

`clrLine` clear a line

_format start/end wrappers_

`fmtRaw` - raw info

`fmtPanel` - menu panel

`fmtMenu` - menu page

`fmtTitle` - menu title

`fmtBody` - menu body

`fmtItem` - menu item

`fmtIndex` - item index

`fmtCursor` - page main cursor

`fmtName` - field name

`fmtMode` - field mode cursor

`fmtValue` field value

`fmtUnit` - field unit


## Navigation composition API

**Drift** is the base for navigation API

`print` start a menu page print

`pos` get navigation focus index

`selected` - check if given is currently select at given index

`mode` return field edit mode

`cmd` send a navigation command to active item

`up` - send up navigation command (increase selection index)

`down` - send down navigation command (decrease selection index)

`enter` - send enter navigation command eventually resulting on item activation

`esc` - send escape navigation command, eventually resulting in active item close

`doInput` - call input chain for input read, validation and reaction

`open` - activate current seletion

`close` - deactivate current selection

## Item composition API

**Empty** is the base for items API

`walkId`

`id`

`canNav`

`size`

`print`

`printMenu`

`printTitle`

`printItems`

`enabled`

`enable`

`changed`

`activate`

`action`

`getValue`

`setValue`

`cmd`

`walkPath`

## Fields

Fields target some user data by reference and will react and update them.
AM5 is able to use any native or user defined data type.

**StaticField** Static data reference.

**Field** Data reference.

**NumFieldAbs** Numeric field abstration, just holding functions.

**StaticNumFieldCore** Static data core for numeric fields with some value specific functions.

**NumFieldCore** Data core for numeric fields with some value specific functions.

**StaticNumField** common composition for static numeric fields, including title and unit texts.

**EnumValue** Represents an enumeration value, to be used on a list of alternatives.

**EnumField** Base for enumeration fields.

**ToggleField** Change value by cycling the enumeration alternatives on `enter`.

**SelectField** Locally change values from alternatives list using navigation keys.

**ChooseField** Use alternative list as sub-menu to select a value.

## Composition

ArduinoMenu 5.x (AM5) uses state-of-the-art modern C++ (compatible with C++ 11 due to target platforms).

**Composition**

This allows AM5 to have both rich and complex options while still producing slim code for simpler menus, broading the range or target devices, composition is provided by C++ mixins.

The idea is that instead of relying on complex configuration that uses memory and runtime code to check the config/properties and take adequate actions, it uses composition to either provide complex/fancy features if the features are composed in, or produce light code if fancy features are absent. None of those requires config or runtime config checking, therefore no config needed composing features.

**This is a major boost over AM4!** allowing AM5 to target even tiny AVR controllers like ATTiny85. :+1: while preserving the fancy features that we're used from AM4.

**CRTP**

`Curiously recurring template pattern` (https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)

Allows AM5 API to hide a lot of parameter passing and shifts the objects composition groups to be type defined instead of code defined, making code simpler to extend.

Even that CRTP and Mixins are orthogonal, AM5 managed to combine them using an open chain composition to allow CRTP to plug in when top level is known, because CRTP is about referencing top level derivation from base level, if you are curious about it https://gist.github.com/neu-rah/5e7779c501bc37f2b3a60ae1896bbfc.

_The down side of this are the complex types resulting from composition and doubled by CRTP, but it worth the pain_

## Compile-time static structures

This allows AM5 to be at same level of direct printed/pure code (no structure) menus.
For very simple menus (or complex ones) one can produce pure code menus resulting on riduculous small memory footprints.
This was usually only achieved by hand coding the menu and unusual on liabraries. By using template and metaprogramming AM5 now can encode the menu structure at compile time resulting in a code only menu, no data footprint. We can even combine that with sporadic features that use state data (like enable/disable state) that have some data footprint.

## Printing chain

```c++
//Navigation ------------------------------

//will call item's printMenu
template<typename Out>
inline void print(Out& out);

//Items ------------------------------

//will call output (printer) printMenu if items is a menu or raw print for simple items
template<typename Nav,typename Out,Op op=Op::Printing>
inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0);

//will call output printItem for item formating
template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self,bool fullPrint=true);

//raw printing
template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
inline void print(Nav& nav,Out& out,PathRef ref=self);

//Output ------------------------------

//will call item to print title and sub-items
template<typename It,typename Nav,Op op=Op::Printing>
void printMenu(It& it,Nav& nav);

//will format item call item print for raw printing
template<typename It,typename Nav,Op op=Op::Printing,bool toPrint=true>
void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal);


//newline if `toPrint` otherwise account only for space usage
template<bool toPrint=true> inline static void nl();

//raw output if `toPrint` otherwise account only for spacing
template<typename T,bool toPrint=true> inline static void raw(T) {}

```

## Object cathegories

### struct **Obj**

CRTP implementation

allow components to access methods of the group by having a reference to the top level composition.

without this its up to the code to introduce the top level elements using a lot of parameters. That would make customization harder.

```c++
template<typename T> struct Obj;
```

`Obj::Type` - composition typename.

`Type& obj();` - get composed object (top level).

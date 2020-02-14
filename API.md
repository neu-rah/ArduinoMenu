# Menu API

_under development_

**things here migh be outdated!**

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
This was usually only achieved by hand coding the menu and unusuall on liabraries. By using template and metaprogramming AM5 now can encode the menu structure at compile time resulting in a code only menu, no data footprint. We can event combine that with sporadic features that use state data (like enable/disable state).

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

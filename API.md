# Menu API

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

## Items

using CRTP pattern for top level elements (caps)

**print** - print item or menu to given output device

```c++
template<typename Out,Roles=Roles::Item,Op=Op::Printing>
inline static void print(Out& out,Idx idx=0,Idx top=0,bool fullPrint=false) {}
```

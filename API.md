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

## Printing chain

```c++
//Items
template<typename Nav,typename Out,Op=Op::Printing>
inline void printMenu(Nav&,Out&);

template<typename Nav,typename Out,Op=Op::Printing,Roles=Roles::Raw>
inline void printItems(Nav&,Out&,Idx=0,Idx=0);

template<typename Nav,typename Out,Op op=Op::Printing>
inline void printItem(Nav&,Out&,Idx=0,Idx=0);

template<typename Out,Op=Op::Printing> inline void print(Out&);

//Output
template<typename It,typename Nav,Op=Op::Printing>
void printMenu(It&,Nav&);

template<typename It,Op=Op::Printing,Roles=Roles::Raw>
inline void printItems(It&,Idx=0,Idx=0);

template<typename I> inline static void printItem(I&);

inline static void nl();
template<typename T> inline static void raw(T);

//Navigation

```

```text
nav.print()
  it.printMenu(nav,out)
  ...|out.printMenu(it,nav)
    it.print<Roles::Title>(out)
    it.printItems()
      ...|out.printItems<>(it)
        {out.printItem(it[n])}
          ...|it.print<Roles::Raw>(out)
            out.raw(T)
```

## Object cathegories

### struct **Obj**

CRTP implementation

allow components to access methods of the group by having a reference to the top level composition

without this its up to the code to introduce the top level elements using a lot of parameters. That would make customization harder.

```c++
template<typename T> struct Obj;
```

`Obj::Type` - composition typename.

`Type& obj();` - get composed object (top level).

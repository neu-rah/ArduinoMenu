# AM21 notes

## Output

> Menu/Nav -> [Fmt] -> [Panel] -> [Measure] -> Device\<Void\>

### Panel
**Controls output device space to provide positioning/scrolling.**

Devices can operate without a panel. _ex: free infinit space (Console)_

Only panels that scroll on lack of free space will need a measure tool.
This kind of panels allow multiple line heights to coexist on same menu.

A `RangePanel` operates on menu items side, so they can be used on fixed line height cases.

### Measure
**Provides a method for a panel to calculates free input device area.**

Even text measure can be complicated.
- escape codes `\n \r \t ...`
- Character surrogates
- numeric formats
- generic data format => API

### Area
**describes output device dimension.**


---

can data specify an output overload?
that is an alternative output based on the current device

=> output must cathegorize components hierarchy

this can be used to implement pad menus

1) Cathegorize output
```c++
enum class OutBlock {Format,Panel,Measure,Device};
//...
using Out=OutDef<
  AsFormat<...>
  AsPanel<...>
  AsMeasure<...>
  AsDevice<...>
>;
```

2) Access output cathegories 
```c++
using OutPanel=Out::Output::Panel;
```

3) Data inclusion
```c++
using MainMenu=ItemDef<
  Using<OutPanel, PadFmt >::Part,
  StaticMenu<....>
>;
```

4) Printing path process should pick on `Using` directives

5) Compose 
```c++
using PadOutput<PadFmt<OutPanel>>
```

**problem** alternative composition wont provied an `obj()` that includes the output top part.

**problem** overriding existing behaviors might  be a mess
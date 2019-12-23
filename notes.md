# Notes

## TODO

formats (Fmt) should have a begin/end (or sort off) to do global setting on output devices, ANSI terminal could benefit from this as we need to hide/show cursor.

## Printing

**print cycle example**

here user calls `nav.printMenu(out)` giving desired output object to a navigation root, this is a print process example of printing first option.  

`Nav::printMenu` - navigation root print current menu, defer printing to device  
`Void::printOn` - device can call the actual print as many times as needed or do something else  
`Nav::_printMenu` - navigation root print current menu  
`StaticMenu::printMenu` - ask menu to draw itself so it can customize  
`FullPrinter::printMenu` - output device will create output part roles envolving the data output  
`StaticMenu::printItems` - output device ready to receive items data  
`Pair::printItems` - data list printing, it will cycle its data and ask for item printing while there is free space  
`FullPrinter::printItem` - ask output to format an item  
`Item::printItem` - item fills in its data  
`Empty::printItem` - end of data printing, recycle to raw data print  
`StaticText::print` - current object printing data  
`CanMeasure::print` - this output device can measure items for positioning (has a cursor)  
`Void::print` - end of output definition  
`StreamOut::raw` - raw print of data

_process will loop on items part while there is free space_

_other functions are called to position the scroll if applicable_

## Memory

Memory figures for ATTiny85

2019.11.21 static tree with change and enable/disable control
```text
DATA:    [==        ]  24.6% (used 126 bytes from 512 bytes)
PROGRAM: [======    ]  63.5% (used 5202 bytes from 8192 bytes)
```

2019.11.25 - partial/minimal draw
```text
DATA:    [===       ]  25.6% (used 131 bytes from 512 bytes)
PROGRAM: [=======   ]  67.0% (used 5488 bytes from 8192 bytes)
```

2019.11.26 - partial/minimal draw (bug fixes)
```text
DATA:    [===       ]  25.6% (used 131 bytes from 512 bytes)
PROGRAM: [=======   ]  68.3% (used 5594 bytes from 8192 bytes)
```

## BAD Dyn print chain

```c++
nav.printMenu(out);
NavRoot<N>::printMenu -> out.printOn(*this,out)
Void::printOn<> -> nav._printMenu(nav,out)
NavRoot<N>::_printMenu() -> N::_printMenu()
Nav<>::_printMenu<> -> entry.printMenu<>()
Prompt<I>::printMenu<>() -> printMenu()
StaticMenu<Title,Body>::printMenu() -> out.printMenu<>()

IMenuOut::printMenu<>() -> printMenu()
MenuOut<>::printMenu() -> O::printMenu<>()
FullPrinter<>::printMenu<>() -> it.printItems<>()
///////////////////////////////////////////////////////
IItem::printItems<>() -> printItems()
Prompt<>::printItems() -> it.printItems<>()
Item<>::printItems<>() -> out.printItem<>()

IMenuOut::printItem<>()
MenuOut<>::printItem() -> O::printItem<>()
FullPrinter<>::printItem<>() -> it.printItem<>()

IItem::printItem<>() -> printItem()
Prompt<>::printItem() -> Base::printItem<>()
Item<>::printItem<>() -> I::printItem<>()
Empty<>::printItem<>() -> it.print<>()
StaticText<>::print<>() -> out.print<>()

IMenuOut::print<>() -> print()
MenuOut<>::print() -> O::print<>()
CanMeasure<>::print<>() -> O::print<>()
Void::print<>() -> out.raw()
StreamOut<>::raw<>()
```

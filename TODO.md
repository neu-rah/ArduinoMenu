# TO DO:

## actions and events

- ...

## printers

- invert printers composition order to be ontop of viewports
- split FullPrinter into smaller printers

## manage free/used space

- panel->viewport

viewport design

- viewport must sit between the printers and the raw device  
+ keep track of space usage and position cursor accordingly  
- should allow creation of areas (new viewports) with special definitions => as a composition to be transparent
- new views can be based on free area, to account for usage  
- support 2D scroll  
- they should ignore stuff outside the free viewport area before reaching the printer
- measuring is taken here too
- independent of menu structure or size, but must collaborate to adjust scroll position
- use menu range to manage device panels

+ **=>** need font size and glyph measure API, optionally account for UTF8, already inplace a minimal measure device (text)

## tree nav

- separate the base nav
- use menu range to manage navigation levels

## accelerators

- optionally use a single table with actions

## redraw need check

- draw, calc and redraw need should have the same functional root

## extra field types

- redirect input commands to fields... but keep vtable small

- implement text options for EEPROM based data

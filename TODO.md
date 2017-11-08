# ArduinoMenu TODO's

**review points for next version (V5)**

The goal of this revision is to:

- Simplify and slim menu core for better fit on small MCU's.
- Modularize parts of the core making them interchangeable.
- Accommodate new features.

## Menu parts

### Input

Currently undergoing a complete input review and redesign to free the logic dependency of streams and characters along with the confusing conversion between char's and cmd's.
New inputs should provide only cmds and eventually deal with chars internally. This greatly simplifies the making of new input drivers not stream related.

Doing some experiments to abstract even more from Arduino framework and generalize inputs https://github.com/neu-rah/OneLib

### Output

Not started yet.

- Panels logic should be removed from the menu, instead the panels should be an optional overlay of the output device
- Core should only call mixins functions, allowing external customization. Mixins should overlay the output driver and provide a custom printout.
  This allows the menu to have both full built-in renders and full user render.
  Still we have to account for used space... need to think more on this.
- Allow multi-line or even full panel options, this is related to the above point.

### Core/Navigation

Not started yet.

Above mentioned output mixins and new inputs will have impact on reducing core code.

Navigation system currently takes care of selected item and panel/device scroll position. To still do so with scroll position and also support custom drawing we have to account for used space or leave the burden to the output overlay. Have to decide it.

Make the core customizable allowing multicolumn navigation

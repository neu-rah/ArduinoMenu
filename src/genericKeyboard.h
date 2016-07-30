/**************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extendable: Yes

generic keyboard driver
supply a function that reads your keyboard and returns one of the keycodes defined in menu.h + menu.cpp or -1 if none available.
int(*)()
ex: int myReader() ...
***/

//if you hold/repeat a key for this ammount of time we will consider it an escape
#ifndef ESCAPE_TIME
#define ESCAPE_TIME 1500
#endif
//emulate a stream keyboard, this is not using interrupts as a good driver should do
// AND is not using a buffer either!
class genericKeyboard:public Stream {
public:
  int (*keyReader)();
  int lastkey;
  unsigned long pressMills=0;
  genericKeyboard(int(*reader)()):keyReader(reader),lastkey(-1) {}
  int available(void) {
    int ch=peek();
    if (lastkey==-1) {
      lastkey=ch;
      pressMills=millis();
    } else if (ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME) return 1;
    if (ch==lastkey) return 0;
    return 1;
  }
  int peek(void) {return keyReader();}
  int read() {
    int ch=peek();
    if (ch==lastkey) return -1;
    int tmp=lastkey;
    bool longPress=ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME;
    pressMills=millis();
    lastkey=ch;
    return longPress?menu::escCode:tmp;//long press will result in escape
  }
  void flush() {}
  size_t write(uint8_t v) {return 0;}
};

/**************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extendable: Yes

quick and dirty keyboard driver
metaprog keyboard driver where N is the number of keys
all keys are expected to be a pin (buttons)
we can have reverse logic (pull-ups) by entering negative pin numbers
ex: -A0 means: pin A0 normally high, low when button pushed (reverse logic)
***/

struct keyMap {
  int8_t pin;
  int8_t code;
};


#ifndef BOUNCE_TICK
#define BOUNCE_TICK 30
#endif

//if you hold/repeat a key for this ammount of time we will consider it an escape
#ifndef ESCAPE_TIME
#define ESCAPE_TIME 1500
#endif
//emulate a stream keyboard, this is not using interrupts as a good driver should do
// AND is not using a buffer either!
template <int N>
class keyLook:public Stream {
public:
  keyMap* keys;
  int lastkey;
  unsigned long pressMills=0;
  keyLook<N>(keyMap k[]):keys(k),lastkey(-1) {}
  int available(void) {
    //Serial<<"available"<<endl;
    int ch=peek();
    if (lastkey==-1) {
      lastkey=ch;
      pressMills=millis();
    } 
	else if (ch == -1 && millis()-pressMills < BOUNCE_TICK)
	{
		lastkey = -1;  //released = it's bounce. reset lastkey
        return 0;
	}
	else if (ch != -1 && millis()-pressMills > BOUNCE_TICK) return 1;
	else if (ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME) return 1;
	
    if (ch==lastkey) return 0;
	
    return 1;
    /*int cnt=0;
    for(int n=0;n<N;n++) {
      int8_t pin=keys[n].pin;
      if (digitalRead(pin<0?-pin:pin)!=(pin<0) ) cnt++;
    }
    return cnt;*/
  }
  int peek(void) {
    //Serial<<"peek"<<endl;
    for(int n=0;n<N;n++) {
      int8_t pin=keys[n].pin;
      if (digitalRead(pin<0?-pin:pin)!=(pin<0) ) return keys[n].code;
    }
    return -1;
  }
  int read() {
    //Serial<<"read"<<endl;
    int ch=peek();
    if (ch==lastkey) return -1;
    int tmp=lastkey;
    bool longPress=ESCAPE_TIME&&millis()-pressMills>ESCAPE_TIME;
    //Serial<<"read lastkey="<<lastkey<<" ch="<<ch<<endl;
    //Serial<<"down time:"<<millis()-pressMills<<endl;
    pressMills=millis();
    lastkey=ch;
    return longPress?menu::escCode:tmp;//long press will result in escape
  }
  void flush() {}
  size_t write(uint8_t v) {return 0;}
};

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

//emulate a stream keyboard, this is not using interrupts as a good driver should do
// AND is not using a buffer either!
template <int N>
class keyLook:public Stream {
public:
  keyMap* keys;
  keyLook<N>(keyMap k[]):keys(k) {}
  int available(void) {
    int cnt=0;
    for(int n=0;n<N;n++) {
      int8_t pin=keys[n].pin;
      if (digitalRead(pin<0?-pin:pin)!=(pin<0) ) cnt++;
    }
    return cnt;
  }
  int peek(void) {
    for(int n=0;n<N;n++) {
      int8_t pin=keys[n].pin;
      if (digitalRead(pin<0?-pin:pin)!=(pin<0) ) return keys[n].code;
    }
    return -1;
  }
  int read() {
    for(int n=0;n<N;n++) {
      int8_t pin=keys[n].pin;
      if (digitalRead(pin<0?-pin:pin)!=(pin<0)) return keys[n].code;
    }
    return -1;
  }
  void flush() {}
  size_t write(uint8_t v) {return 0;}
};



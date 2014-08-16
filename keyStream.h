struct keyMap {
  int8_t pin;
  int8_t code;
};

//emulate a stream keyboard, this is not using interrupts as a good driver should do
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



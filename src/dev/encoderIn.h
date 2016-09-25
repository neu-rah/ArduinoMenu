/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extendable: Yes

quadrature encoder driver (PCINT)
quadrature encoder stream (fake, not using buffers)
*/

#include <pcint.h> //https://github.com/neu-rah/PCINT

class encoderIn {
public:
  volatile int pos;
  int pinA,pinB;
  encoderIn(int a,int b):pinA(a),pinB(b) {}
  void begin() {
    pinMode(pinA, INPUT);
    digitalWrite(pinA,1);
    pinMode(pinB, INPUT);
    digitalWrite(pinB,1);
    //attach pin change handlers
    PCattachInterrupt(pinA, mixHandler((void(*)(void*))encoderInUpdateA,this), CHANGE);
    PCattachInterrupt(pinB, mixHandler((void(*)(void*))encoderInUpdateB,this), CHANGE);
  }
  //PCint handlers
  static void encoderInUpdateA(class encoderIn *e) {
    if (digitalRead(e->pinA)^digitalRead(e->pinB)) e->pos--;
    else e->pos++;
  }
  static void encoderInUpdateB(class encoderIn *e) {
    if (digitalRead(e->pinA)^digitalRead(e->pinB)) e->pos++;
    else e->pos--;
  }
};

//emulate a stream based on encoderIn movement returning +/- for every 'sensivity' steps
//buffer not needer because we have an accumulator
class encoderInStream:public Stream {
public:
  encoderIn &enc;//associated hardware encoderIn
  int sensivity;
  int oldPos;
  encoderInStream(encoderIn &enc,int sensivity):enc(enc), sensivity(sensivity) {}
  inline void setSensivity(int s) {sensivity=s;}
  int available(void) {return abs(enc.pos-oldPos)/sensivity;}
  int peek(void) override {
    int d=enc.pos-oldPos;
    if (d<=-sensivity)return '-';
    if (d>=sensivity) return '+';
    return -1;
  }
  int read() override {
    int d=enc.pos-oldPos;
    if (d<=-sensivity) {
      oldPos-=sensivity;
      return '-';
    }
    if (d>=sensivity) {
      oldPos+=sensivity;
      return '+';
    }
    return -1;
  }
  void flush() {oldPos=enc.pos;}
  size_t write(uint8_t v) {oldPos=v;return 1;}
};

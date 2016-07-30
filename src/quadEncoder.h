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

class quadEncoder {
public:
  volatile int pos;
  int pinA,pinB;
  quadEncoder(int a,int b):pinA(a),pinB(b) {}
  void begin() {
    pinMode(pinA, INPUT);
    digitalWrite(pinA,1);
    pinMode(pinB, INPUT);
    digitalWrite(pinB,1);
    //attach pin change handlers
    PCattachInterrupt(pinA, mixHandler((void(*)(void*))quadEncoderUpdateA,this), CHANGE);
    PCattachInterrupt(pinB, mixHandler((void(*)(void*))quadEncoderUpdateB,this), CHANGE);
  }
  //PCint handlers
  static void quadEncoderUpdateA(class quadEncoder *e) {
    if (digitalRead(e->pinA)^digitalRead(e->pinB)) e->pos--;
    else e->pos++;
  }
  static void quadEncoderUpdateB(class quadEncoder *e) {
    if (digitalRead(e->pinA)^digitalRead(e->pinB)) e->pos++;
    else e->pos--;
  }
};

//emulate a stream based on quadEncoder movement returning +/- for every 'sensivity' steps
//buffer not needer because we have an accumulator
class quadEncoderStream:public Stream {
public:
  quadEncoder &enc;//associated hardware quadEncoder
  int sensivity;
  int oldPos;
  quadEncoderStream(quadEncoder &enc,int sensivity):enc(enc), sensivity(sensivity) {}
  inline void setSensivity(int s) {sensivity=s;}
  int available(void) {return abs(enc.pos-oldPos)/sensivity;}
  int peek(void) {
    int d=enc.pos-oldPos;
    if (d<=-sensivity)return '-';
    if (d>=sensivity) return '+';
    return -1;
  }
  int read() {
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

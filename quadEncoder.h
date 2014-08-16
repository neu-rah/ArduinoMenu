#include "pcint.h"

//PCInt library is missing (Arduino has none), handlers are conflicting due to private handler implementations
class quadEncoder {
public:
  volatile int pos;
  int pinA,pinB;
  quadEncoder(int a,int b):pinA(a),pinB(b) {
    pinMode(a, INPUT);
    digitalWrite(a,1);
    pinMode(b, INPUT);
    digitalWrite(b,1);
    //attach pin change handlers
    PCattachInterrupt(a, mixHandler((void(*)(void*))quadEncoderUpdateA,this), CHANGE);
    PCattachInterrupt(b, mixHandler((void(*)(void*))quadEncoderUpdateB,this), CHANGE);
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
#define quadEncoder_BUFFER_SIZE 8
class quadEncoderStream:public Stream {
public:
  quadEncoder &enc;//associated hardware quadEncoder
  int sensivity;
  int oldPos;
  struct {
    unsigned char buffer[quadEncoder_BUFFER_SIZE];
    volatile unsigned int head;
    volatile unsigned int tail;
  } rx_buffer = { { 0 }, 0, 0};
  quadEncoderStream(quadEncoder &enc,int sensivity):enc(enc), sensivity(sensivity) {}
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



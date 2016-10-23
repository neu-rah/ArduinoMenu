#include "encoderIn.hpp"

//PCint handlers
void encoderIn::encoderInUpdateA(class encoderIn *e) {
  if (digitalRead(e->pinA)^digitalRead(e->pinB)) e->pos--;
  else e->pos++;
}
void encoderIn::encoderInUpdateB(class encoderIn *e) {
  if (digitalRead(e->pinA)^digitalRead(e->pinB)) e->pos++;
  else e->pos--;
}

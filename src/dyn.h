/* -*- C++ -*- */
#include "shadows.h"

class promptDyn:public promptShadow {
  public:
    promptDyn(const char* t,action a=doNothing,eventMask e=noEvent,styles s=noStyle)
      :action(a),sysStyles(_noStyle),text(t),events(e),style(s) {}
    virtual constMEM char* getText() const {return (constMEM char*)text;}
};

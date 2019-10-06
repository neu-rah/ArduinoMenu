/* -*- C++ -*- */
#pragma once

//time ticks as boolean
template<int step>
struct Tick {
  inline operator bool() {
    return millis()>=next?next+=step,true:false;
  }
protected:
  unsigned long next=0;
};

//FPS as boolean
// this is an FPS limiter
//return `true` `fps` times per second (max)
//note: this can skip a frame at millis wrap (every 50days ;))
template<int fps>
struct FPS:public Tick<(1000/fps)> {};

//blink without delay at a fixed rate
//just set the led state to this function output.
template<int timeOn,int timeOff>
inline bool sblink() {
  return millis()%(unsigned long)(timeOn+timeOff)<(unsigned long)timeOn;
}

//blink without delay, just set the led state to this function output.
inline bool blink(int timeOn,int timeOff) {
  return millis()%(unsigned long)(timeOn+timeOff)<(unsigned long)timeOn;
}

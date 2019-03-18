/* -*- C++ -*- */
#pragma once

template<typename T>
struct Used {T w,h};

//reports used space
template<typename T>
struct Panel {T x,y,w,h;};

//important!
//draw, change check and size prevision should all come from same code!
//and affected by the same state

//a viewport is a panel in evolution
//tracking the space remaining
//and restricting the drawing (as a Panel does)
template<typename T>
struct ViewPort:public Panel<T> {
  ViewPort operator-(Used o) {

  }
};

/* -*- C++ -*- */
#pragma once

//an example of extending the base schema, here a multilanguage text

template<typename Sys,typename Code>
struct Lang {
  static Code sel;
  static const char *texts[][Code::cnt];
  using RawOut=typename Sys::RawOutput;
  template<size_t id,typename O=typename Sys::Item>
  struct Text:public O {
    inline RawOut& print(RawOut& o) {
      o<<texts[id][Lang::sel];
      return O::print(o);
    }
  };
};

template<typename Sys,typename Code>
Code Lang<Sys,Code>::sel=(Code)0;

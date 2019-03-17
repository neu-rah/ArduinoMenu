/* -*- C++ -*- */
#pragma once

#include <vector>
using namespace std;

template<typename Sys,typename O=Sys::Item>
class Menu:public O,public vector<IfPrompt*> {
public:
  using vector<IfPrompt*>::vector;
  Menu(const char* o,vector<IfPrompt*>&& v):O(o),vector<IfPrompt*>(v) {}
  inline size_t sz() const {return vector<IfPrompt*>::size();}
  inline IfPrompt& get(size_t i) {return *vector<IfPrompt*>::operator[](i);}
  inline IfPrompt& operator[](size_t i) {return *vector<IfPrompt*>::operator[](i);}
protected: using vector<IfPrompt*>::operator[];
};

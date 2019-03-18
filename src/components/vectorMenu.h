/* -*- C++ -*- */
#pragma once

#include <vector>
#include <initializer_list>
using namespace std;

template<typename Sys,typename O=Item<Sys>>
class VectorMenu:public O,public vector<IfPrompt*> {
public:
  using vector<IfPrompt*>::vector;
  // VectorMenu(const char* o,vector<IfPrompt*>&& v):O(o),vector<IfPrompt*>(v) {}
  VectorMenu(vector<IfPrompt*>&& v):vector<IfPrompt*>(v) {}
  // VectorMenu(initializer_list<IfPrompt*> v):vector<IfPrompt*>(v) {}
  // VectorMenu(const char* o,initializer_list<IfPrompt*> v):O(o),vector<IfPrompt*>(v) {}
  // template<typename... V>
  // VectorMenu(V... v):vector<IfPrompt*>{v...} {}
  // template<typename... V>
  // VectorMenu(V... v):vector<IfPrompt*>(initializer_list<IfPrompt*>{v...}) {}
  template<typename... V>
  VectorMenu(const char* o,V... v):O(o),vector<IfPrompt*>{v...} {}
  inline size_t sz() const {return vector<IfPrompt*>::size();}
  inline IfPrompt& get(size_t i) {return *vector<IfPrompt*>::operator[](i);}
  inline IfPrompt& operator[](size_t i) {return *vector<IfPrompt*>::operator[](i);}
protected: using vector<IfPrompt*>::operator[];
};

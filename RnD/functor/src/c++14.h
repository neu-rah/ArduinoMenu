/* -*- C++ -*- */
#pragma once

#include <iostream>
using namespace std;

//with c++14, curryed function
// add::a->b->c
auto add=[](auto a){
  return [=](auto b){
    return [=](){
      return a+b;};};};

template<typename O,O o>
struct StaticData {};

template<typename O,O o>
void out(StaticData<O,o>) {cout<<o;}

template<typename O,O o>
auto fmap(const auto f,StaticData<O,o>) {return StaticData<decltype(f(o)),f(o)>();}

// template<typename O,typename... OO>
// struct Node {};
//
// template<typename O,typename... OO>
// auto fmap(auto f,Node<O,OO...> node) {
//   return Node<f(O&(*this)),fmap(f,)
// }

// int main() {
//   cout<<add(2)(3)()<<endl;
//   cout<<add(2.5f)(3.39)()<<endl;
//   return 0;
// }

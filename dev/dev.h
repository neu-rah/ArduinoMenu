/* -*- C++ -*- */
#pragma once
#include <iostream>
using namespace std;
#include "../src/menu.h"
#include "../src/components/multilang.h"
#include "../src/components/vectorMenu.h"

//this is my test zone, things might be broken

//alias to SysDef using cout
using Sys=SysDef<decltype(cout)>;

//multilang requires this to be defined:
//an enumeration of languages
enum Code:uint8_t {en=0,pt,cnt};
//an enumeration of strings (id's)
enum TextIds:size_t {yesText=0,noText,okText,cancelText};
//a table of strings with [ids][lang]
template<typename Sys,typename Code>
const char *Lang<Sys,Code>::texts[][Code::cnt]{
  {"Yes","Bora"},
  {"No","Não"},
  {"Ok","Na boa"},
  {"Cancel","Nada disso!"},
};

//using components, usually we have a restricter set, but this is a demo...
template<typename O> using Prompt=Sys::Prompt<O>;

//a multilanguage title
template<size_t id>
using Title=Sys::asTitle<Lang<Sys,Code>::Text<id>>;

/* -*- C++ -*- */
#pragma once
#include <iostream>
#include "../../AM5.h"
#include "../../components/multilang.h"

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
  {"Yes","Ome vamos"},
  {"No","Na me cheira"},
  {"Ok","Tá modes"},
  {"Cancel","Tá quieto praí!"},
};

//using components, usually we have a restricter set, but this is a demo...
template<typename O> using Prompt=Sys::Prompt<O>;
using Static=Sys::Static<>;

//a multilanguage title
template<size_t id>
using Title=Sys::asTitle<Lang<Sys,Code>::Text<id>>;

const char* op1_text="op1";
Prompt<Sys::asTitle<Static::Text<&op1_text>>> op1;
Prompt<Sys::asTitle<Sys::Text<>>> op2("op2");

Prompt<Sys::Menu<Sys::asTitle<Sys::Text<>>>> subMenu("non-static title",{&op1,&op2});

const char* sm_title="static menu";
Prompt<Sys::Static<Sys::asTitle<Static::Text<&sm_title>>>::Menu<2>> sm({&op1,&op2});

//a dynamic (vector) menu with static text title
const char* mainMenu_title="Main menu";
Prompt<Sys::Menu<Static::Text<&mainMenu_title>>> mainMenu {
  &op1,//a static text option
  &op2,//a non-static text option
  &subMenu,//dynamic menu with a non-static text title
  &sm,//static menu with static title text
  new Prompt<Lang<Sys,Code>::Text<yesText>>,
  new Prompt<Title<noText>>,
  new Prompt<Title<okText>>,
  new Prompt<Title<cancelText>>,
};

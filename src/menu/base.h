/* -*- C++ -*- */
#pragma once
/**
* @file base.h
* @author Rui Azevedo
* @brief generic definitions and aux. stuff
*/

#include <lpp.h>
using namespace lpp;

#include <menu/debug.h>

template<typename... OO>
using TinyCore=True<OO...>;

using idx_t=TinyCore<Char,As<unsigned int>>::App::Type;

enum class Roles {Panel,Menu,Title,Body,Prompt,Index,Cursor,Name,Mode,Value,Unit};
enum class Modes {Normal,Edit,Tune};

#if defined(ARDUINO) && (defined(__AVR_ATtiny13__))
using __FlashStringHelper=const char*;
#endif

template<typename I=Nil<>> struct Empty;
template<typename I=Nil<>> struct Void;
template<typename I=Nil<>> struct Drift;
template<typename N> class StaticNav;
template<typename N=Drift<>,typename Idx=idx_t> class NavPosBase;

struct Item;
struct IMenuOut;
struct INavNode;
struct NavAgent;

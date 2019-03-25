/* -*- C++ -*- */
#pragma once

#include <menuBase.h>
#include <menuOut.h>
#include <items.h>

template<typename O>
MenuOut& MenuOutCap<O>::operator<<(Item& i) {i.out(*this);return *this;}

inline MenuOut& operator<<(MenuOut& o, Item& i) {i.out(o);return o;}

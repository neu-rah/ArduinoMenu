/* -*- C++ -*- */
#pragma once

#include <limits>
#include "debug.h"

using Idx=std::size_t;
constexpr Idx idx_max=std::numeric_limits<Idx>::max();

#define endl "\r\n"

enum class Modes {Normal,Edit,Tune};
enum class Roles:Idx {Panel,Menu,Title,Body,Prompt,Index,Cursor,Name,Mode,Value,Unit,Raw};

constexpr char* roleNames[]{
  "Panel","Menu","Title","Body","Prompt","Index",
  "Cursor","Name","Mode","Value","Unit","Raw"
};

template<typename O>
constexpr inline O& operator<<(O& o,Roles r) {return o<<roleNames[(Idx)r];}

// struct TextMeasure;

template<Roles Part>
struct AllowRole {
  template<Roles P> inline static constexpr bool allowed() {return Part==P;}
};

template<Roles Part>
struct DenyRole {
  template<Roles P> inline static constexpr bool allowed() {return Part!=P;}
};

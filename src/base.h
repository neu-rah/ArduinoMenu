/* -*- C++ -*- */
#pragma once

namespace AM5 {

  using ConstText=const char[];
  using idx_t = int;//size_t sucks for scrolling

  struct Empty {
    template<typename H>
    static inline void out() {H::Printer::raw('???');}
    template<typename H>
    static inline void out(size_t) {}
    constexpr static inline size_t size() {return 0;}
    constexpr static inline size_t pos() {return 0;}
    constexpr static inline size_t top() {return 0;}
    static inline void setTop(size_t) {}
    constexpr static inline bool isRange() {return false;}
    // constexpr static inline idx_t orgX() {return 0;}
    // constexpr static inline idx_t orgY() {return 0;}
    // constexpr static inline idx_t width() {return 80;}
    // constexpr static inline idx_t height() {return size();}//TODO: this need access to item head again
    constexpr static inline bool up() {return false;}
    constexpr static inline bool down() {return false;}
    constexpr static inline bool left() {return down();}
    constexpr static inline bool right() {return up();}
    constexpr static inline bool enter() {return false;}
    constexpr static inline bool esc() {return false;}
    static inline void enable(bool) {}
    constexpr static inline bool enabled() {return true;}
    template<typename H,bool io> inline void fmtItem(H& p) {}
    template<typename H,bool io> inline void fmtMenu(H& p) {}
    template<typename H,bool io> inline void fmtMenuBody(H& p) {}
    template<typename H,bool io> inline void fmtTitle(H& p) {}
    template<typename H,bool io> inline void fmtIndex(H& p) {}
    template<typename H,bool io> inline void fmtCursor(H& p) {}
  };
};

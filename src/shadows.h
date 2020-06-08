/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_SHADOWS
  #define RSITE_ARDUINO_MENU_SHADOWS

  #include "menuBase.h"

  namespace Menu {

    struct promptShadowRaw {
      actionRaw a;//the hooked callback function
      systemStyles sysStyles;
      const char* text;
      constMEM eventMask events;//registered events
      styles style;
    };
    class promptShadow:public action {
      protected:
      public:
        systemStyles sysStyles;
        const char* text;
        eventMask events;//registered events (mask)
        styles style;
      public:
        promptShadow(const char* t,action a=doNothing,eventMask e=noEvent,styles s=noStyle,systemStyles ss=_noStyle)
          :action(a),sysStyles(ss),text(t),events(e),style(s) {}
        inline constText* getText() const {return (constText*)memPtr(text);}
        inline systemStyles _sysStyles() const {return (systemStyles)memEnum(&sysStyles);}
        inline eventMask _events() const {return (eventMask)memEnum(&events);}
        inline styles _style() const {return (styles)memEnum(&style);}
    };
    union promptShadows {
      promptShadowRaw raw;
      promptShadow obj;
    };
    struct textFieldShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      eventMask events;//registered events
      styles style;
      char* buffer;
      const char* const* validators;
      idx_t sz;
    };
    class textFieldShadow:public promptShadow {
      protected:
      public:
        char* buffer;
        const char* const* validators;
        idx_t sz;
        textFieldShadow(
          const char* label,
          char* b,
          idx_t sz,
          char* const* v,
          action a=doNothing,
          eventMask e=noEvent,
          styles style=noStyle,
          systemStyles ss=(Menu::systemStyles)(_noStyle|_canNav|_parentDraw)
        ):promptShadow(label,a,e,style,ss),buffer(b),validators(v),sz(sz) {}
        idx_t _sz() const {return (idx_t)memIdx(sz);}
        char* _buffer() const {return (char*)memPtr(buffer);}
        char* const* _validators() const {return (char* const*)memPtr(validators);}
        inline constText* operator[](idx_t i) const {
          return (char*)memPtr(((char**)_validators())[i]);
        }
    };
    union textFieldShadows {
      textFieldShadowRaw raw;
      textFieldShadow obj;
    };
    struct menuNodeShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char* text;
      eventMask events;//registered events
      styles style;
      idx_t sz;
      prompt* constMEM* data;
    };
    class menuNodeShadow:public promptShadow {
      protected:
      public:
        idx_t sz;
        prompt* constMEM* data;
      public:
        menuNodeShadow(const char* text,idx_t sz,prompt* constMEM* data,action a,eventMask e,styles style,systemStyles ss=(systemStyles)(_menuData|_canNav))
        :promptShadow(text,a,e,style,ss),sz(sz),data(data) {}
        idx_t _sz() const {return (idx_t)memIdx(sz);}
        prompt* constMEM* _data() const {return (prompt* constMEM*)memPtr(data);}
        inline prompt& operator[](idx_t i) const {
          return *(prompt*)memPtr(((prompt**)_data())[i]);
          //return *(prompt*)memPtr(((prompt**)memPtr(((menuNodeShadow*)shadow)->data))[i]);
        }
    };

    union menuNodeShadows {
      menuNodeShadowRaw raw;
      menuNodeShadow obj;
    };

    struct fieldBaseShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char* text;
      eventMask events;//registered events
      styles style;
      constText* units;
    };
    class fieldBaseShadow:public promptShadow {
      public:
        constText* units;
        fieldBaseShadow(constText* text,constText*units,action a=doNothing,eventMask e=noEvent,styles s=noStyle,systemStyles ss=((Menu::systemStyles)(Menu::_canNav|Menu::_parentDraw)))
          :promptShadow(text,a,e,s,ss),units(units) {}
        inline constText* _units() {return (constText*)memPtr(units);}
    };
    template<typename T>
    struct menuFieldShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char* text;
      eventMask events;//registered events
      styles style;
      constText* units;
      T* value;
      constMEM T low,high,step,tune;
    };
    template<typename T>
    class menuFieldShadow:public fieldBaseShadow {
      protected:
      public:
        T* value;
        constMEM T low,high,step,tune;
      public:
        menuFieldShadow(T &value,constText* text,constText*units,T low,T high,T step,T tune,action a=doNothing,eventMask e=noEvent,styles s=noStyle,systemStyles ss=((Menu::systemStyles)(Menu::_canNav|Menu::_parentDraw)))
          :fieldBaseShadow(text,units,a,e,s,ss),value(&value),low(low),high(high),step(step),tune(tune) {}
        inline T& target() const {return *(T*)memPtr(value);}
        inline T getTypeValue(const T* from) const {
          //TODO: dynamic versions require change of preprocessor to virtual
          #ifdef USING_PGM
            T tmp;
            memcpy_P(&tmp, from, sizeof(T));
            return tmp;
          #else
            return *from;
          #endif
        }
        inline T _low() const {return getTypeValue(&low);}
        inline T _high() const {return getTypeValue(&high);}
        inline T _step() const {return getTypeValue(&step);}
        inline T _tune() const {return  getTypeValue(&tune);}
    };
    template<typename T>
    union menuFieldShadows {
      menuFieldShadowRaw<T> raw;
      menuFieldShadow<T> obj;
    };

    template<typename T>
    struct menuValueShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char* text;
      eventMask events;//registered events
      styles style;
      T value;
    };
    template<typename T>
    class menuValueShadow:public promptShadow {
      protected:
      public:
        T value;
      public:
        inline menuValueShadow(constText* text,T value,action a=doNothing,eventMask e=noEvent)
          :promptShadow(text,a,e),value(value) {}
        inline T getTypeValue(const T* from) const {
          //TODO: dynamic versions require change of preprocessor to virtual
          #ifdef USING_PGM
            T tmp;
            memcpy_P(&tmp, from, sizeof(T));
            return tmp;
          #else
            return *from;
          #endif
        }
        inline T target() const {return getTypeValue(&value);}
    };

    template<typename T>
    union menuValueShadows {
      menuValueShadowRaw<T> raw;
      menuValueShadow<T> obj;
    };

    template<typename T>
    struct menuVariantShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char* text;
      constMEM eventMask events;//registered events
      styles style;
      idx_t sz;
      prompt* constMEM* data;
      //int width;//field or menu width
      //int ox,oy;//coordinate origin displacement
      T* value;
    };
    template<typename T>
    class menuVariantShadow:public menuNodeShadow {
      public:
        T* value;
        menuVariantShadow(
          constText* text,
          T &target,
          idx_t sz,
          prompt* constMEM* data,
          action a,
          eventMask e,
          styles style,
          systemStyles ss=(systemStyles)(_menuData|_canNav)
        ):menuNodeShadow(text,sz,data,a,e,style,ss),value(&target) {}
      inline T& target() const {return *((T*)memPtr(value));}
    };
    template<typename T>
    union menuVariantShadows {
      menuVariantShadowRaw<T> raw;
      menuVariantShadow<T> obj;
    };
  }//namespace Menu

#endif

/* -*- C++ -*- */
#ifndef RSITE_ARDUINO_MENU_SHADOWS
  #define RSITE_ARDUINO_MENU_SHADOWS

  #include "menuBase.h"

  namespace Menu {

    struct promptShadowRaw {
      actionRaw a;//the hooked callback function
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      styles style;
    };
    class promptShadow:public action {
      protected:
        systemStyles sysStyles;
        const char*text;
        const eventMask events;//registered events (mask)
        styles style;
      public:
        promptShadow(const char* t,action a=doNothing,eventMask e=noEvent,styles s=noStyle)
          :action(a),sysStyles(_noStyle),text(t),events(e),style(s) {}
        inline const char* getText() const {return (const char*)memPtr(text);}
        inline const systemStyles _sysStyles() const {return (systemStyles)memEnum(&sysStyles);}
        inline const eventMask _events() const {return (eventMask)memEnum(&events);}
        inline const styles _style() const {return (styles)memEnum(&style);}
    };
    struct menuNodeShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      styles style;
      idx_t sz;
      prompt* const* data;
    };
    class menuNodeShadow:public promptShadow {
      protected:
        idx_t sz;
        prompt* const* data;
      public:
        menuNodeShadow(const char* text,idx_t sz,prompt* const* data,action a,eventMask e,styles style)
        :promptShadow(text,a,e,style),sz(sz),data(data) {}
        idx_t _sz() const {return (idx_t)memIdx(sz);}
        prompt* const* _data() const {return (prompt* const*)memPtr(data);}
        inline prompt& operator[](idx_t i) const {
          return *(prompt*)memPtr(((prompt**)_data())[i]);
          //return *(prompt*)memPtr(((prompt**)memPtr(((menuNodeShadow*)shadow)->data))[i]);
        }
    };

    template<typename T>
    struct menuFieldShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      styles style;
      T* value;
      const char* units;
      const T low,high,step,tune;
    };
    template<typename T>
    class menuFieldShadow:public promptShadow {
      protected:
        T* value;
        const char* units;
        const T low,high,step,tune;
      public:
        menuFieldShadow(T &value,const char * text,const char *units,T low,T high,T step,T tune,action a=doNothing,eventMask e=noEvent,styles s=noStyle)
          :value(&value),units(units),low(low),high(high),step(step),tune(tune),promptShadow(text,a,e,s) {}
        inline T& target() const {return *(T*)memPtr(value);}
        inline const char* _units() {return (const char*)memPtr(units);}
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
    struct menuValueShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      styles style;
      T value;
    };
    template<typename T>
    class menuValueShadow:public promptShadow {
      protected:
        T value;
      public:
        inline menuValueShadow(const char * text,T value,action a=doNothing,eventMask e=noEvent)
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
    struct menuVariantShadowRaw {
      actionRaw a;
      systemStyles sysStyles;
      const char*text;
      const eventMask events;//registered events
      styles style;
      idx_t sz;
      prompt* const* data;
      //int width;//field or menu width
      //int ox,oy;//coordinate origin displacement
      T* value;
    };
    template<typename T>
    class menuVariantShadow:public menuNodeShadow {
      protected:
        T* value;
      public:
        menuVariantShadow(const char* text,T &target,idx_t sz,prompt* const* data,action a,eventMask e,styles style)
        :menuNodeShadow(text,sz,data,a,e,style),value(&target) {}
      inline T& target() const {return *((T*)memPtr(value));}
    };
  }//namespace Menu
#endif

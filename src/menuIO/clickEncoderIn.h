/* -*- C++ -*- */
/**************

ClickEncoderStream.h

Jun. 2016
Modified by Christophe Persoz and Rui Azevedo.
Based on keyStream.h developed by Rui Azevado.
and ClickEncoder library by Peter Dannegger.
https://github.com/christophepersoz/encoder

Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

quick and dirty keyboard driver
metaprog keyboard driver where N is the number of keys
all keys are expected to be a pin (buttons)
we can have reverse logic (pull-ups) by entering negative pin numbers
ex: -A0 means: pin A0 normally high, low when button pushed (reverse logic)

***/


#ifndef __ClickEncoderStream_h__
  #define __ClickEncoderStream_h__

  #include <Arduino.h>

  #ifndef ARDUINO_SAM_DUE
    // Arduino specific libraries
    // #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega328P__)
      // #include <stdint.h>
      //#include <avr/io.h>
      //#include <avr/interrupt.h>
      #include <ClickEncoder.h>
    // #endif

    #include "../menuDefs.h"

    namespace Menu {

      //emulate a stream based on clickEncoder movement returning +/- for every 'sensivity' steps
      //buffer not needer because we have an accumulator
    class ClickEncoderStream:public menuIn {
      public:
        ClickEncoder &enc; //associated hardware clickEncoder
        int8_t sensivity;
        int oldPos;
        int pos;
        ClickEncoder::Button btn;

        inline void update() {
          pos += enc.getValue();

          if (btn == ClickEncoder::Open)//do not override previous input
            btn = enc.getButton();
        }

        ClickEncoderStream(ClickEncoder &enc,int sensivity)
          :enc(enc),
          sensivity(sensivity),
          oldPos(0),
          pos(0),
          btn(ClickEncoder::Open) {
            pos = enc.getValue();
          }


        inline void setSensivity(int s) {
            sensivity = s;
        }

        int available(void) {
            return peek() != -1;
        }

        int peek(void) {
          update();
          if (btn == ClickEncoder::Clicked)
            return options->navCodes[enterCmd].ch;//menu::enterCode;

          if (btn == ClickEncoder::DoubleClicked)
            return options->navCodes[escCmd].ch;//menu::escCode;

          btn=ClickEncoder::Open;//2017, must consume not used input to proceed

          int d = pos - oldPos;
          if (d <= -sensivity)
              return options->navCodes[downCmd].ch;//menu::downCode;
          if (d >= sensivity)
              return options->navCodes[upCmd].ch;//menu::upCode;
          return -1;
        }

        int read()
        {
            int ch = peek();
            btn = ClickEncoder::Open;
            if (ch == options->navCodes[upCmd].ch)//menu::upCode)
                oldPos += sensivity;
            else if (ch == options->navCodes[downCmd].ch)//menu::downCode)
                oldPos -= sensivity;
            return ch;
        }

        void flush() {
            btn=ClickEncoder::Open;//2017 clear current key
            update();
            oldPos = pos;
        }

        size_t write(uint8_t v) {
            oldPos = v;
            return 1;
        }
      };
    }//namespace Menu

  #endif

#endif /* ClickEncoderStream_h */

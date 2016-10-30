/**************

 ClickEncoderStream.h

 Jun. 2016
 Modified by Christophe Persoz and Rui Azevedo.
 Based on keyStream.h developed by Rui Azevado.
 and ClickEncoder library by Peter Dannegger.
 https://github.com/christophepersoz/encoder

 Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
 creative commons license 3.0: Attribution-ShareAlike CC BY-SA
 This software is furnished "as is", without technical support, and with no
 warranty, express or implied, as to its usefulness for any purpose.

 Thread Safe: No
 Extendable: Yes

 quick and dirty keyboard driver
 metaprog keyboard driver where N is the number of keys
 all keys are expected to be a pin (buttons)
 we can have reverse logic (pull-ups) by entering negative pin numbers
 ex: -A0 means: pin A0 normally high, low when button pushed (reverse logic)
 ***/


#ifndef __ClickEncoderStream_h__
#define __ClickEncoderStream_h__

// Arduino specific libraries
#ifdef defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega328P__)
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include <Arduino.h>
#include <ClickEncoder.h>

/*  emulate a stream based on clickEncoder movement returning +/- for every 'sensivity' steps
 buffer not needer because we have an accumulator
 */
class ClickEncoderStream:public Stream {
public:
    ClickEncoder &enc; //associated hardware clickEncoder
    int8_t sensivity;
    int oldPos;
    int pos;
    ClickEncoder::Button btn;

    inline void update() {
        pos += enc.getValue();

        if (btn == ClickEncoder::Open)
            btn = enc.getButton();
    }

    ClickEncoderStream(ClickEncoder &enc,int sensivity)
    :enc(enc),
    pos(0),
    oldPos(0),
    btn(ClickEncoder::Open),
    sensivity(sensivity) {
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
            return menu::enterCode;

        if (btn == ClickEncoder::DoubleClicked)
            return menu::escCode;

        int d = pos - oldPos;
        if (d <= -sensivity)
            return menu::downCode;
        if (d >= sensivity)
            return menu::upCode;
        return -1;
    }

    int read()
    {
        int ch = peek();
        btn = ClickEncoder::Open;
        if (ch == menu::upCode)
            oldPos += sensivity;
        else if (ch == menu::downCode)
            oldPos -= sensivity;
        return ch;
    }

    void flush() {
        update();
        oldPos = pos;
    }

    size_t write(uint8_t v) {
        oldPos = v;
        return 1;
    }
};


#endif /* ClickEncoderStream_h */

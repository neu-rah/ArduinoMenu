/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2020 Rui Azevedo
* @brief ArduinoMenu ANSI terminal output
*/

#include "extra/ansiCodes.h"

namespace Menu {
	struct AnsiFmt {
		template<typename O>
		struct Part:O {
			using Base=O;
			using This=Part<O>;
			using Base::Base;
			template<bool toPrint=true>
			inline void clrLine(Idx nr) {
				if(toPrint) ansiFill(O::orgX(),nr,O::orgX()+O::width(),nr)(*this);
				setCursor(O::orgX(),nr);
			}
			inline void setCursor(int x,int y) {xy(x,y)(*this);}
		  template<bool io,bool toPrint=true>
		  inline void fmtPanel(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
				if (io) {
					setCursor(O::orgX(),O::orgY());
					setBackgroundColor(BLUE)(*this);
					setForegroundColor(WHITE)(*this);
					boldOn()(*this);
					if (toPrint)
						ansiFill(O::orgX(),O::orgY(),O::orgX()+O::width(),O::orgY()+O::height())(*this);
					O::template fmtPanel<io,toPrint>(n,s,e,m);
				} else {
					O::template fmtPanel<io,toPrint>(n,s,e,m);
					boldOff()(*this);
					defaultForeground()(*this);
					defaultBackground()(*this);
				}
				setCursor(O::orgX(),O::orgY());
		  }
		  template<bool io,bool toPrint=true>
		  inline void fmtMenu(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
				if (io) {
					hideCursor()(*this);
					setBackgroundColor(BLUE)(*this);
					setForegroundColor(WHITE)(*this);
					boldOn()(*this);
				} else {
					O::template fmtMenu<io,toPrint>(n,s,e,m);
					boldOff()(*this);
					defaultForeground()(*this);
					defaultBackground()(*this);
					// showCursor()(*this);
					setCursor(0,0);
				}
				// setCursor(O::orgX(),O::orgY());
		  }
		};
	};
};

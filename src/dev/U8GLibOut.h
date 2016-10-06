/********************
Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Use graphics screens as menu output, based on U8GLIB graphic display
www.r-site.net

 printPrompt(), Added posX to take into account ox via menu.ox and setPosition(ox,oy), need to update other files to support that.

***/
#ifndef RSITE_ARDUINOP_MENU_U8G
	#define RSITE_ARDUINOP_MENU_U8G
  #include <U8glib.h>
	#include "menu.h"

  namespace Menu {

    class menuU8G:public gfxOut {
      public:
	      U8GLIB& gfx;
				int8_t fontMargin=2;
				const uint16_t (&colors)[nColors][2][2];
	      menuU8G(U8GLIB& gfx,const uint16_t (&c)[nColors][2][2],int resX=6,int resY=9)
					:gfxOut(gfx.getWidth()/resX,gfx.getHeight()/resY,resX,resY),colors(c),gfx(gfx) {
						Serial<<"height: "<<maxY<<" lines"<<endl;
	        //gfx.setFont(u8g_font_04b_03); // Good result
	        gfx.setFontPosBottom(); // U8Glib font positioning
	      }

				void setColor(colorDefs c,bool selected=false,status s=enabledStatus) override {
					//Serial<<"setColor:"<<colors[c][selected][s]<<endl;
					gfx.setColorIndex(memWord(&colors[c][selected][s]));
				}

				void clearLine(int ln,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus) override {
						//Serial<<"clearLine "<<ln<<" color:"<<color<<" selected"<<selected<<" status:"<<(int)stat<<endl;
						setColor(color,selected,stat);
						gfx.drawBox(posX,posY+ln*resY,posX+maxX*resX,posY+(ln+1)*resY);
	          setCursor(0,ln);
	      }
	      void clear() override {
	          // No need to clear, the U8Glib display loop clear screen on each refresh
	      }
	      void setCursor(int x,int y) override {
	          gfx.setPrintPos(posX+x*resX,posY+(y+1)*resY-fontMargin);
	      }
	      size_t write(uint8_t ch) override {
	          gfx.write(ch); // print the ASCII correspoding char instead of print which display the decimal value of the char.
	          return 1;
	      }
	      /*void printPrompt(prompt &o,bool selected,int idx,int posX,int posY,int width) override {
	          gfx.setColorIndex(selected?hiliteColor:bgColor);
	          gfx.drawBox(posX*resX,posY*resY,maxX*resX,resY);
	          gfx.setColorIndex(selected?(o.enabled?enabledColorHi:disabledColorHi):(o.enabled?enabledColor:disabledColor));
	          setCursor(posX,posY+1); //+1 compensate the height of the font and the way how U8Glib works
	          o.printTo(*this);
	      }*/
	  		void printMenu(navNode &nav) override {
					idx_t ot=top;
          while(nav.sel>=top+maxY) top++;
          while(nav.sel<top) top--;
					if (top!=lastTop||lastSel!=nav.sel||top!=ot||drawn!=nav.target||nav.target->changed(nav,*this)) {
						//Serial<<"printMenu"<<endl;
						for(idx_t i=0;i<maxY;i++) {
              if (i+top>=nav.sz()) break;
              //setCursor(0,i);
              prompt& p=nav[i+top];
							clearLine(i,bgColor,nav.sel==i+top,p.enabled);
							//setColor(cursorColor,p.enabled);
              //p.print(i+top==nav.sel?options.selectedCursor:' ');
							setColor(fgColor,nav.sel==i+top,p.enabled);
							setCursor(posX,posY+i);
              p.printTo(i,nav,*this);
            }
            lastTop=top;
            lastSel=nav.sel;
          }
	    }
  	};
}
#endif RSITE_ARDUINOP_MENU_LCD

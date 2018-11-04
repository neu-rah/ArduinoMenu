/* -*- C++ -*- */

/********************
Oct. 2016 Stephen Denne https://github.com/datacute
Based on U8GLibOut.h from Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz

Use graphics screens as menu output, based on U8G2 graphic display
www.r-site.net

***/
#ifndef RSITE_ARDUINOP_MENU_UART
#define RSITE_ARDUINOP_MENU_UART
#include "menuDefs.h"

namespace Menu {

class uartOut:public gfxOut {
	public:
		enum systemFont {
			font16 = 16,
			font24 = 24,
			font32 = 32
		};
		int8_t offsetX=0;
		int8_t offsetY=0;
		int cursorX = 0;
		int cursorY = 0;
		char buf[100];
		int currentColor = 0;
		int m_bgColor = 0;
		systemFont m_Font;
		Stream& stream;
		const colorDef<uint8_t> (&colors)[nColors];
		uartOut(
				Stream& stream,
				const colorDef<uint8_t> (&c)[nColors],
				idx_t* t,
				panelsList &p,
				systemFont font = font24,
				idx_t offsetX=0,
				idx_t offsetY=0,
				int fontMarginY=1
			) :gfxOut(font/2,font,t,p,(styles)(menuOut::redraw|menuOut::rasterDraw)),stream(stream),colors(c)
		{
			this->offsetX=offsetX;
			this->offsetY=offsetY;
			this->fontMarginY=fontMarginY;
			this->m_Font = font;
		}

		size_t write(uint8_t ch) override
		{
			sprintf(buf,"SBC(%d);DCV%d(%d,%d,'%c',%d);", m_bgColor, m_Font, cursorX, cursorY - resY + fontMarginY, (char)ch, currentColor);
			sendCommand(buf);
			cursorX = cursorX + resX;
			return 1;
		}

		inline uint8_t getColor(colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) const
		{
			return memByte(&(stat==enabledStatus?colors[color].enabled[selected+edit]:colors[color].disabled[selected]));
		}

		void setBgColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false)
		{
			m_bgColor = getColor(c,selected,s,edit);
		}

		void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool edit=false) override
		{
			currentColor = getColor(c,selected,s,edit);
		}

		void clearLine(idx_t ln,idx_t panelNr=0,colorDefs color=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override
		{
			const panel p=panels[panelNr];
			setBgColor(color,selected,stat,edit);
			drawBox(p.x*resX + offsetX, (p.y+ln)*resY + offsetY, maxX()*resX, resY);
		}

		void clear() override
		{
			sendCommand("CLR(7);");
			panels.reset();
		}

		void clear(idx_t panelNr) override
		{
			const panel p=panels[panelNr];
			setBgColor(bgColor,false,enabledStatus,false);;
			drawBox(p.x*resX + offsetX,p.y*resY + offsetY,p.w*resX,p.h*resY);
			panels.nodes[panelNr]=NULL;
		}

		void box(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override
		{
			const panel p=panels[panelNr];
			setBgColor(c, selected, stat, edit);
			drawFrame((p.x+x)*resX,(p.y+y)*resY, w*resX, h*resY);
		}

		void rect(idx_t panelNr,idx_t x,idx_t y,idx_t w=1,idx_t h=1,colorDefs c=bgColor,bool selected=false,status stat=enabledStatus,bool edit=false) override
		{
			const panel p=panels[panelNr];
			setBgColor(c, selected, stat, edit);
			drawBox((p.x+x)*resX, (p.y+y)*resY, w*resX, h*resY);
		}

		void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override
		{
			const panel p=panels[panelNr];
			cursorX = (p.x+x)*resX+fontMarginX + offsetX;
			cursorY = (p.y+y+1)*resY-fontMarginY + offsetY;
		}

		void drawCursor(idx_t ln,bool selected,status stat,bool edit=false,idx_t panelNr=0) override
		{
			const panel p=panels[panelNr];
			setBgColor(cursorColor,selected,stat);
			drawFrame(p.x*resX + offsetX, (p.y+ln)*resY + offsetY, maxX()*resX, resY);
		}

		idx_t printRaw(const char* at,idx_t len) override
		{
			sprintf_P(buf,PSTR("SBC(%d);DCV%d(%d,%d,'%S',%d);"), m_bgColor, m_Font, cursorX, cursorY - resY + fontMarginY, at, currentColor);
			sendCommand(buf);
			cursorX = cursorX + strlen_P(at)*resX;
			return strlen_P(at);
		}

	private:

		void checkStreamReady()
		{
			int cnt = 0, timeOutCounter = 1000;
			while (timeOutCounter-- > 0)
			{
				delay(1);
				if (stream.available() > 0)
				{
					stream.read();
					if (++cnt == 4) break;
				}
			}
		}

		void sendCommand(char * cmd)
		{
			stream.println(cmd);
			//Serial.println(cmd);
			checkStreamReady();
		}

		void drawFrame(int x, int y, int w, int h)
		{
			sprintf(buf, "BOX(%d,%d,%d,%d,%d);", x, y, x + w, y + h, m_bgColor);
			sendCommand(buf);
		}

		void drawBox(int x, int y, int w, int h)
		{
			sprintf(buf, "BOXF(%d,%d,%d,%d,%d);", x, y, x + w, y + h, m_bgColor);
			sendCommand(buf);
		}
	};
}
#endif //RSITE_ARDUINOP_MENU_UART

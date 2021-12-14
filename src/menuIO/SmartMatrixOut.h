#ifndef RSITE_ARDUINOP_MENU_SM
#define RSITE_ARDUINOP_MENU_SM

#include <Layer_Background.h>
#include "..\menuDefs.h"
#include <type_traits>

namespace Menu
{
    inline menuOut::styles operator | (menuOut::styles lhs, menuOut::styles rhs)
    {
        using T = std::underlying_type_t <menuOut::styles>;
        return static_cast<menuOut::styles>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }
        
    inline menuOut::styles& operator |= (menuOut::styles& lhs, menuOut::styles rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    
    template<typename RGB, uint8_t backgroundLayerOptions>
    class SmartMatrixOut : public gfxOut
    {

        using SMBackground=SMLayerBackground<RGB,backgroundLayerOptions>;

        RGB currentColour{0,0,0};
        uint16_t cursorX=0, cursorY=0,line=0;

    public:
        SMBackground &gfx;
        const colorDef<RGB> (&colors)[nColors];
        SmartMatrixOut(SMBackground &gfx, const colorDef<RGB> (&c)[nColors], idx_t *t, panelsList &p, fontChoices font=fontChoices::font5x7, menuOut::styles s=minimalRedraw)
            : gfxOut(fontLookup(font)->Width, fontLookup(font)->Height, t, p, s), gfx(gfx), colors(c)
            {
                gfx.setFont(font);
                setCursor(0,0);
            }

        virtual ~SmartMatrixOut() {}
        
        size_t write(uint8_t ch) override
        {
            if(ch=='\n')
            {
                setCursor(0,line+1);
            }
            else if(ch!='\r')
            {
                gfx.drawChar(cursorX,cursorY,currentColour,ch);
                cursorX+=resX;
            }

            return 1;
        }

        inline RGB getColor(colorDefs color = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) const
        {
            return memWord(&(stat == enabledStatus ? colors[color].enabled[selected + edit] : colors[color].disabled[selected]));
        }

        void setColor(colorDefs c, bool selected = false, status s = enabledStatus, bool e = false) override
        {
            currentColour=getColor(c,selected,s,e);
        }

        void clearLine(idx_t ln, idx_t panelNr = 0, colorDefs color = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) override
        {
            const panel p = panels[panelNr];
            gfx.fillRectangle(p.x * resX, (p.y + ln) * resY + fontMarginY, (p.x+p.w) * resX, (p.y + ln + 1)*resY, getColor(color, selected, stat, edit));
            //setCursor(0,ln);
        }
        void clear() override
        {
            panels.reset();
            gfx.fillScreen(getColor(bgColor, false, enabledStatus, false));
            setCursor(0, 0);
            setColor(fgColor);
        }

        void box(idx_t panelNr, idx_t x, idx_t y, idx_t w = 1, idx_t h = 1, colorDefs c = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) override
        {
            const panel p = panels[panelNr];
            gfx.drawRectangle((p.x + x) * resX, (p.y + y) * resY + fontMarginY, (p.x + x + w) * resX, (p.y + y + h) * resY, getColor(c, selected, stat, edit));
        }

        void rect(idx_t panelNr, idx_t x, idx_t y, idx_t w = 1, idx_t h = 1, colorDefs c = bgColor, bool selected = false, status stat = enabledStatus, bool edit = false) override
        {
            const panel p = panels[panelNr];
            gfx.fillRectangle((p.x + x) * resX, (p.y + y) * resY + fontMarginY, (p.x + x + w) * resX, (p.y + y + h) * resY, getColor(c, selected, stat, edit));
        }

        void clear(idx_t panelNr) override
        {
            const panel p = panels[panelNr];
            gfx.fillRectangle(p.x * resX, p.y * resY, (p.x+p.w) * resX, (p.y+p.h) * resY, getColor(bgColor, false, enabledStatus, false));
            panels.nodes[panelNr] = NULL;
        }

        void setCursor(idx_t x, idx_t y, idx_t panelNr = 0) override
        {
            line=y;
            const panel p = panels[panelNr];
            cursorX=(p.x + x) * resX;
            cursorY= (p.y + line) * resY + fontMarginY;
        }

        void drawCursor(idx_t ln, bool selected, status stat, bool edit = false, idx_t panelNr = 0) override
        {
            const panel p = panels[panelNr];
            // gfxOut::drawCursor(ln,selected,stat);
            gfx.drawRectangle(p.x * resX, (p.y + ln) * resY + fontMarginY, (p.x+p.w) * resX, (p.y + ln +1) * resY, getColor(cursorColor, selected, enabledStatus, false));
        }
    };

}; //namespace adaGfxOut
#endif

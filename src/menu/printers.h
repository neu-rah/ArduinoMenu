/* -*- C++ -*- */
#pragma once
/**
* @file printers.h
* @author Rui Azevedo
* @brief ArduinoMenu part printers
*/

/** \defgroup Printers Part printing
 *  @{
 */

/**
* The FullPrinter component allows output devices to print data and formats
* TODO: subdivide into smaller printers...
*/
template<typename P=TextMeasure>
struct FullPrinter:public P {
  template<typename Nav,typename Out,typename I>
  inline void printMenu(Nav& nav,Out& out,I& i) {
    trace(MDO<<"FullPrinter::printMenu"<<endl);
    out.template fmtPanel<true>(nav,out,i,0);
    out.template fmtMenu<true>(nav,out,i,0);
    out.template fmtTitle<true>(nav,out,i,0);
    i.print(nav,out,i);
    out.template fmtTitle<false>(nav,out,i,0);
    out.template fmtBody<true>(nav,out,i,0);

    if (Out::isRange()) {
      //ensure that selection option is withing range
      //TODO: option can have variable height...
      //this is NOT taking it into account => rework
      while(out.top()>nav.pos())
        out.setTop(out.top()-1);
      while(nav.pos()>=out.top()+out.freeY())
        out.setTop(out.top()+1);
    }

    trace(MDO<<"size:"<<i.size()<<endl);
    for(idx_t n=out.top();n<i.size();n++) {
      if (!out.freeY()) break;
      P::clrLine(out,P::posY());
      out.template fmtItem<true>(nav,out,*this,n);
      out.template fmtIndex<true>(nav,out,*this,n);
      out.template fmtIndex<false>(nav,out,*this,n);
      out.template fmtCursor<true>(nav,out,*this,n);
      out.template fmtCursor<false>(nav,out,*this,n);
      // _trace(MDO<<"#"<<n<<endl);
      i.printItem(nav,out,i,n);
      out.template fmtItem<false>(nav,out,*this,n);
    }

    out.template fmtBody<false>(nav,out,i,0);
    out.template fmtMenu<false>(nav,out,i,0);
    out.template fmtPanel<false>(nav,out,i,0);
  }
};
/** @}*/

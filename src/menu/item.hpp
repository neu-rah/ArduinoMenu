#include "item.h"
#include "imenu.h"

template<typename Title,typename Body>
template<typename It,typename Nav,typename Out>
void StaticMenu<Title,Body>::printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref,Idx n) {
  _trace(
    MDO<<"StaticMenu::printMenu of:";
    it.print(debugOut);
    MDO<<endl
  );
  if (pd&&ref.len==1) out.printParent(it,nav,out);
  else if (ref) Base::tail.printMenu(pd,Base::tail,nav,out,ref,ref.head());//TODO: really? what about n?
  else {
    // out.template printMenu<This,Nav,Out,OutOp::Printing>(*this,nav,out);//static print should use this
    out.template printMenu<It,Nav,Out,OutOp::Printing>(it,nav,out);//dynamic print should use this
    if (out.partialDraw()) out.template printMenu<It,Nav,Out,OutOp::ClearChanges>(it,nav,out);
  }
}

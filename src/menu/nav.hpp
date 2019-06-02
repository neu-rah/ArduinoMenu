#include "nav.h"

template<typename N>
void NavRoot<N>::printMenu() {
  _trace(MDO<<"NavRoot::printMenu()"<<endl);
  printMenu(*this);
  // printMenu(DynamicNav<typename N::OutType,typename N::DataType,This>(*this));
}

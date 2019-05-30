#include "nav.h"

template<typename N>
void NavRoot<N>::printMenu() {
  printMenu(*this);
  // printMenu(DynamicNav<typename N::OutType,typename N::DataType,This>(*this));
}

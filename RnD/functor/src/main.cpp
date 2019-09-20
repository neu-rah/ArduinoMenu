
template<typename O,typename... OO>
struct Map {

  using Next=Map<OO...>;

  template<typename It,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    O::template printTo<It,P,N>();
    Next::template printTo<It,P,N>();
  }

  template<Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo<O,P,N>() {printTo<O,P,N>();}

  // template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  // inline static void printItem(size_t i) {}//print a sub-item by index
  // template<typename It,typename Out,typename Nav=Drift<>>
  // inline static void printMenu() {
  //   _trace(MDO<<"Empty::printMenu");
  //   It::template printTo<Out,Roles::Panel,Nav>();
  //   It::template printMenu<Out,Nav>();
  // }//print full menu
  // inline static idx_t size() {return 0;}
  // template<Roles r,bool io,typename It,typename Out,typename Nav>
  // inline static void fmt(idx_t n) {Out::template fmt<r,io,It,Out,Nav>(n);}
};

template<typename O>
struct Map<O> {

  template<typename It,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {
    O::template printTo<It,P,N>();
  }

  template<Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo<O,P,N>() {printTo<O,P,N>();}

};

int main() {
  return 0;
}

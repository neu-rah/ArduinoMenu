#include "menu.h"
using namespace Menu;

bool prompt::hasTitle(navNode& nav) const {return (nav.target->has(showTitle)||(nav.root->showTitle&&!nav.target->has(noTitle)));}

idx_t prompt::printRaw(menuOut& out,idx_t len) const {
  trace(Serial<<"prompt::printRaw"<<endl;print_P(Serial,getText(),len));
  return print_P(out,getText(),len);
}

Used prompt::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
  trace(Serial<<"prompt::printTo"<<endl);
  #ifdef MENU_FMT_WRAPS
  out.fmtStart(menuOut::fmtPrompt,root.node(),idx);
  #endif
  idx_t r=printRaw(out,len);
  if (is((systemStyles)(_menuData|_parentDraw|_asPad))
    //&&((&((menuNode*)root.node().target)->operator[](idx))==this)
  ) {
      if (root.node().target==this)
        out.printMenu(root.node(), panelNr);
      else
        out.previewMenu(root,*(menuNode*)this,panelNr);
  }
  #ifdef MENU_FMT_WRAPS
  out.fmtEnd(menuOut::fmtPrompt,root.node(),idx);
  #endif
  return r;
}

#ifdef MENU_ASYNC
prompt* menuNode::seek(idx_t* uri,idx_t len) {
  trace(Serial<<"menuNode::seek"<<endl);
  if (len&&uri[0]>=0&&uri[0]<sz()) {
    prompt& e=operator[](uri[0]);
    assert(e.isMenu());
    return e.seek(++uri,--len);
  } else return NULL;
}
bool menuNode::async(const char *uri,navRoot& root,idx_t lvl) {
  trace(Serial<<"menuNode::async"<<endl);
  if ((!*uri)||(uri[0]=='/'&&!uri[1])) return this;
  uri++;
  idx_t n=0;
  while (*uri) {
    char* d=strchr(numericChars,uri[0]);
    if (d) n=n*10+((*d)-'0');
    else break;
    uri++;
  }
  if (root.path[lvl].target!=this) {
    //Serial<<"escaping"<<endl;
    while(root.level>lvl) root.doNav(escCmd);
  }
  //Serial<<*(prompt*)this<<" doNav idxCmd:"<<n<<endl;Serial.flush();
  //if (this->operator[](n).type()!=fieldClass) {//do not enter edit mode on fields over async
    //Serial<<"doNav idxCmd"<<endl;
    root.doNav(navCmd(idxCmd,n));
  //}
  //Serial<<"recurse on ["<<n<<"]-"<<operator[](n)<<" uri:"<<uri<<" lvl:"<<lvl<<endl;Serial.flush();
  return operator[](n).async(uri,root,++lvl);
}
#endif

void textField::doNav(navNode& nav,navCmd cmd) {
  trace(Serial<<"textField::doNav:"<<cmd.cmd<<endl);
  //Serial.println("textField doNav!");
  switch(cmd.cmd) {
    case enterCmd:
      if (edited&&!charEdit) {
        edited=false;
        cursor=0;
        nav.root->exit();
      } else {
        charEdit=!charEdit;
        dirty=true;
        edited=true;
      }
      break;
    case escCmd:
      dirty=true;
      if (charEdit) charEdit=false;
      else {
        edited=false;
        cursor=0;
        nav.root->exit();
      }
      break;
    case upCmd:
      if (charEdit) {
        const char* v=validator(cursor);
        char *at=strchr(v,buffer()[cursor]);
        idx_t pos=at?at-v+1:1;
        if (pos>=(idx_t)strlen(v)) pos=0;
        buffer()[cursor]=v[pos];
        dirty=true;
      } else {
        if(cursor<(idx_t)strlen(buffer())-1) cursor++;
        edited=false;
      }
      dirty=true;
      break;
    case downCmd:
      if (charEdit) {
        const char* v=validator(cursor);
        char *at=strchr(v,buffer()[cursor]);//at is not stored on the class to save memory
        idx_t pos=at?at-v-1:0;
        if (pos<0) pos=strlen(v)-1;
        buffer()[cursor]=v[pos];
        dirty=true;
      } else {
        if (cursor) cursor--;
        edited=false;
      }
      dirty=true;
      break;
    default:break;
  }
  trace(Serial<<"cursor:"<<cursor<<endl);
}

Used textField::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
  trace(Serial<<*this<<" textField::printTo"<<endl);
  // out.fmtStart(menuOut::fmtPrompt,root.node(),idx);
  idx_t at=0;
  bool editing=this==root.navFocus;
  trace(Serial<<"editing:"<<editing<<" len:"<<len;)
  idx_t l=navTarget::printTo(root,sel,out,idx,len,panelNr);
  if (l<len) {
    out.write(editing?":":" ");
    l++;
  }
  // idx_t c=l;
  //idx_t top=out.tops[root.level];
  idx_t tit=hasTitle(root.node())?1:0;
  idx_t line=idx+tit;//-out.tops[root.level];
  trace(Serial<<" tit:"<<tit<<" line:"<<line<<" cursor:"<<cursor<<" l:"<<l<<" len:"<<len<<endl;)
  idx_t c=l+1;
  while(buffer()[at]&&l++<len)
    if (at==cursor&&editing) {
      // Serial<<"idx:"<<idx<<" line:"<<line<<" at:"<<at<<" l:"<<l<<endl;
      // c=l+1;
      l+=out.startCursor(root,l,line,charEdit);//draw textual cursor or color code start
      out.write(buffer()[at++]);//draw focused character
      l+=out.endCursor(root,l,line,charEdit);//draw textual cursor or color code end
    } else out.write(buffer()[at++]);
  out.editCursor(root,c+cursor,line,editing,charEdit);//reposition a non text cursor
  return l;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// menuNode - any menu object that has a list of members
//
////////////////////////////////////////////////////////////////////////////////
bool menuNode::changed(const navNode &nav,const menuOut& out,bool sub,bool test) {
  trace(Serial<<*this<<" menuNode::changed"<<endl);
  if (dirty) {
    trace(if (test) Serial<<"just dirty!"<<endl);
    return true;
  }
  bool appd=has((systemStyles)(_asPad|_parentDraw));
  if (appd) {
    trace(Serial<<"appd!"<<endl;);
    for(int i=0;i<sz();i++)
      if (operator[](i).changed(nav,out,false,test)) {
        trace(if (test) Serial<<"APPD! "<<operator[](i).type()<<endl);
        return true;
      }
  } else {
    trace(Serial<<*this<<"!appd"<<endl;);
    if (!(nav.target==this&&sub)) {
      trace(if (test&&dirty) Serial<<"indirect!"<<endl);
      return dirty;// second hand check, just report self
    }
    idx_t level=nav.root->level;
    if (parentDraw()) {
      trace(Serial<<"return changed of parent-draw element"<<endl);
      trace(if (test) Serial<<"parentDraw()!"<<endl);
      return nav.root->path[level-1].target->changed(nav.root->path[level-1],out,sub,test);
    }
    // idx_t tit=hasTitle(nav.root->path[lev])?1:0;//TODO: this might not be correct.. checking
    idx_t my=out.maxY()-((has(showTitle)||(nav.root->showTitle&&!has(noTitle)))?1:0);
    trace(Serial<<"level:"<<level<<" target:"<<*nav.root->navFocus<<" "<<nav.root->navFocus->has(_parentDraw)<<" "<<nav.root->navFocus->has(_asPad)<<endl);
    idx_t lev=level-(nav.root->navFocus->has(_parentDraw)&&nav.root->navFocus->isMenu());
    // trace(Serial<<"tit:"<<tit<<endl;);
    idx_t t=out.tops[lev];
    trace(Serial<<"t:"<<t<<endl;);
    if (sub) for(int i=0;i<my;i++,t++) {
      if (t>=sz()) break;
      trace(Serial<<"checking:"<<operator[](t)<<endl);
      if (operator[](t).changed(nav,out,false,test)) {
        trace(if (test) Serial<<"sub changed!"<<endl);
        return true;
      }
    }
  }
  return false;
}

void menuNode::clearChanged(const navNode &nav,const menuOut& out,bool sub) {
  trace(Serial<<" menuOut::clearChanged "<<nav);
  dirty=false;
  if (has((systemStyles)(_asPad|_parentDraw))) {
    for(int i=0;i<sz();i++)
      operator[](i).clearChanged(nav,out,false);
  } else {
    if (!(nav.target==this&&sub)) return;
    idx_t level=nav.root->level;
    if (parentDraw())
      return nav.root->path[level-1].target->clearChanged(nav.root->path[level-1],out,sub);
    idx_t my=out.maxY()-((has(showTitle)||(nav.root->showTitle&&!has(noTitle)))?1:0);
    idx_t lev=level-(nav.root->navFocus->has(_parentDraw)&&nav.root->navFocus->isMenu());
    idx_t t=out.tops[lev];
    for(idx_t i=0;i<my;i++,t++) {//only signal visible
      if (t>=sz()) break;//menu ended
      operator[](t).clearChanged(nav,out,false);
    }
  }
  #ifdef DEBUG
  if(changed(nav,out,sub,true)) {
    Serial<<"ERROR clear changed fail!"<<endl;
    Serial<<*this<<endl;
    Serial<<"level:"<<nav.root->level<<endl;
    // Serial<<"type:"<<type()<<endl;
    Serial.flush();
    while(1);
  }
  #endif
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// navTarget - any menu object that can process keys/input
//
////////////////////////////////////////////////////////////////////////////////

void navTarget::doNav(navNode& nav,navCmd cmd) {
  trace(Serial<<"navTarget::doNav"<<endl);
  nav.doNavigation(cmd);
}

void navTarget::parseInput(navNode& nav,menuIn& in) {
  trace(Serial<<"navTarget::parseInput"<<endl);
  doNav(nav,nav.navKeys(in.read()));
}

#ifdef MENU_ASYNC
bool fieldBase::async(const char *uri,navRoot& root,idx_t lvl) {
  trace(Serial<<"fieldBase::async"<<endl);
  if ((!*uri)||(uri[0]=='/'&&!uri[1])) return true;
  else if (uri[0]=='/') {
    StringStream i(++uri);
    parseInput(root.node(), i);
    return true;
  }
  return true;
}
#endif

void fieldBase::doNav(navNode& nav,navCmd cmd) {
  trace(Serial<<"fieldBase::doNav "<<cmd<<endl);
  switch(cmd.cmd) {
    //by default esc and enter cmds do the same by changing the value
    //it might be set by numeric parsing when allowed
    case idxCmd: //Serial<<"menuField::doNav with idxCmd"<<endl;
    case escCmd:
      tunning=true;//prepare for exit
    case enterCmd:
      if (tunning||nav.root->nav2D||!canTune()) {//then exit edition
        tunning=false;
        dirty=true;
        constrainField();
        nav.event(nav.root->useUpdateEvent?updateEvent:enterEvent);
        nav.root->exit();
        return;
      } else tunning=true;
      dirty=true;
      break;
    case upCmd:
      stepit(1);
      break;
    case downCmd:
      stepit(-1);
      break;
    default:break;
  }
  /*if (ch==options->getCmdChar(enterCmd)&&!tunning) {
    nav.event(enterEvent);
  }*/
  if (dirty)//sending enter or update event
    nav.event(nav.root->useUpdateEvent?updateEvent:enterEvent);
}

Used fieldBase::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
  trace(Serial<<"fieldBase::printTo"<<endl);
  idx_t l=prompt::printTo(root,sel,out,idx,len,panelNr);
  bool ed=this==root.navFocus;
  //bool sel=nav.sel==i;
  if (l<len) {
    //TODO: remove cursor when printing asPad menus
    // if(!root.node().has(_asPad))//do notdraw edit cursors for pad menu items?
    //this is not really working!
    out.print((root.navFocus==this&&sel)?(tunning?'>':':'):' ');
    l++;
    if (l<len) {
      #ifdef MENU_FMT_WRAPS
  out.fmtStart(menuOut::fmtField,root.node(),idx);
      #endif
      out.setColor(valColor,sel,enabled,ed);
      //out<<reflex;
      l+=printReflex(out);//NOTE: this can exceed the limits!
      #ifdef MENU_FMT_WRAPS
  out.fmtEnd(menuOut::fmtField,root.node(),idx);
      #endif
      if (l<len) {
        #ifdef MENU_FMT_WRAPS
  out.fmtStart(menuOut::fmtUnit,root.node(),idx);
        #endif
        out.setColor(unitColor,sel,enabled,ed);
        l+=print_P(out,units(),len);
        #ifdef MENU_FMT_WRAPS
  out.fmtEnd(menuOut::fmtUnit,root.node(),idx);
        #endif
      }
    }
  }
  return l;
}

/////////////////////////////////////////////////////////////////
Used menuVariantBase::printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
  trace(Serial<<"menuVariantBase::printTo"<<endl);
  idx_t l=len;
  l-=prompt::printTo(root,sel,out,idx,len,panelNr);
  idx_t at=sync(sync());
  bool ed=this==root.navFocus;
  //bool sel=nav.sel==i;
  if (len-l<0) return 0;
  out.print(this==&root.active()?':':' ');
  l--;
  #ifdef MENU_FMT_WRAPS
    if (out.fmtStart(type()==selectClass?menuOut::fmtSelect:menuOut::fmtChoose,root.node(),idx)==proceed) {
  #endif
    // Serial<<"variant ";
    // print_P(Serial,operator[](at).getText());
    // Serial<<endl;
    out.setColor(valColor,sel,prompt::enabled,ed);
    if (l>0) l-=operator[](at).printRaw(out,l);
  #ifdef MENU_FMT_WRAPS
    }
  #endif
  #ifdef MENU_FMT_WRAPS
  out.fmtEnd(type()==selectClass?menuOut::fmtSelect:menuOut::fmtChoose,root.node(),idx);
  #endif
  trace(Serial<<"menuVariantBase::printTo ended!"<<endl);
  return len-l;
}

idx_t menuVariantBase::togglePrintTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) {
  trace(Serial<<"menuVariantBase::togglePrintTo"<<endl);
  idx_t l=prompt::printTo(root,sel,out,idx,len,panelNr);
  idx_t at=sync(sync());
  bool ed=this==root.navFocus;
  //bool sel=nav.sel==i;
  out.setColor(valColor,sel,prompt::enabled,ed);
  //out<<menuNode::operator[](at);
  if (len-l>0) {
    #ifdef MENU_FMT_WRAPS
  out.fmtStart(menuOut::fmtToggle,root.node(),idx);
    #endif
    l+=operator[](at).printRaw(out,len-l);
    #ifdef MENU_FMT_WRAPS
  out.fmtEnd(menuOut::fmtToggle,root.node(),idx);
    #endif
  }
  return l;
}

void menuVariantBase::doNav(navNode& nav,navCmd cmd) {
  trace(Serial<<"menuVariantBase::doNav"<<endl);
  nav.sel=sync();
  navCmd c=nav.doNavigation(cmd);
  sync(nav.sel);
  if (c.cmd==enterCmd) {
    nav.root->exit();
  }
}

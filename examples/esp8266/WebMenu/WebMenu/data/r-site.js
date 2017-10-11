/*function updateField(e) {
  window.location.href=$(e.target).attr("data-path")+"/"+e.target.value;
}
function updateSelect(e) {
  console.log($(e.target).attr("data-path")+"/"+e.target.selectedIndex);
  window.location.href=$(e.target).attr("data-path")+"/"+e.target.selectedIndex;
}
function init() {
  console.log("r-site menu page script starting.");
  var f=$("input.aml_field");
  f.change(updateField);
  var s=$(".aml_ops select");
  s.change(updateSelect)
}*/

wsSrc="192.168.1.64"

var connection = {}
function menuIn(o) {connection.send(o.id.split('~').join('/')+"/"+o.value);}

function sendBinValue(nr,path,value) {
  nr*=1;
  path+="";
  value*=1;
  if (path.endsWith("~")) path=path.slice(0,-1);
  path=path.split('~').slice(1);
  var len=path.length+4;
  var payload=new Uint8Array(len);
  payload[0]=nr;
  payload[1]=nr>>8;
  for(var n in path) payload[n*1+2]=parseInt(path[n]);
  payload[len-2]=value;
  payload[len-1]=value>>8;
  console.log(payload,len);
  connection.send(payload, len);
}

function flushBin() {
  if (lastBinData) {
    if (binBusy) setTimeout(flushBin,200);
    else {
      console.log("flushing bin");
      sendBinInp.apply(this,lastBinData);
      lastBinData=false;
    }
  }
}

binBusy=false;
lastBinData=false;
function sendBinInp(nr,path,inp) {
  if (binBusy) {
    console.log("overlapping!");
    if (!lastBinData) setTimeout(flushBin,200);
    lastBinData=arguments;
    return;
  }
  binBusy=true;
  nr*=1;
  path+="";
  inp+="";
  if (path.endsWith("/")) path=path.slice(0,-1);
  path=path.split('/').slice(1);
  var len=path.length+3+inp.length+1;
  var payload=new Uint8Array(len);
  payload[0]=nr;
  payload[1]=nr>>8;
  payload[2]=path.length;
  for(var n in path) payload[n*1+3]=parseInt(path[n]);
  for(var i in inp) payload[3+path.length+i*1]=inp.charCodeAt(i*1);
  payload[len-1]=0;
  console.log(payload,len);
  connection.send(payload, len);
  //binBusy=false;
}

function updateField(e) {
  return updateField_(e.target);
}

function updateField_(o) {
  var t$=$(o);
  var url=t$.attr("data-path")+"/"+o.value;
  if (url.indexOf("/menu?at=")==0) url=url.substr(9);
  sendBinInp(activePanel.attr("id"),url,(t$.attr("data-idx")*1+1)+o.value);
}
function updateSelect(e) {updateSelect_(e.target);}

function updateSelect_(o) {
  var url=$(o).attr("data-path")+"/"+o.selectedIndex;
  if (url.indexOf("/menu?at=")==0) url=url.substr(9);
  console.log("sending: "+url);
  connection.send(url);
}
var activePanel=[];
function init() {
  console.log("r-site menu page script starting.");
  activePanel=$(".aml_panel");
  var f=$("input.menu-range-field");
  //f.change(updateField);
  f.on("input",updateField);
  var s=$(".aml_ops select");
  s.change(updateSelect)
}

$(function() {
  console.log( "ready!" );
  connection =new WebSocket('ws://'+wsSrc+':81/', ['arduino']);
  connection.onopen = function () {connection.send('Connect ' + new Date());};
  connection.onerror = function (error) {console.log('WebSocket Error ', error);};
  connection.onmessage = function (e) {eval(e.data);};
  init();
});

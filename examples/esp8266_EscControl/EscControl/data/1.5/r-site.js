// wsSrc = sourceURL;
wsSrc=window.location.hostname;
var connection = {};

// alert("r-site.js loaded!");
var lastResult=null;
var previousResult=null;

function connectWS() {
  console.log("ws://" + wsSrc + ":81/", ["arduino"]);
  connection = new WebSocket("ws://" + wsSrc + ":81/", ["arduino"]);
  connection.onopen = init;
  connection.onerror = function(e) {console.log("WebSocket Error ", e)};
  connection.onmessage = function(e) {
    try {
      console.log(JSON.parse(e.data));//just for checking it
    } catch(err) {
      console.log("NO JSON!",e.data);
    }
    eval("lastResult="+e.data);
    //updatePage();
  };
  return connection.readyState;
}

function updatePage() {
  if (lastResult) mkPath(lastResult);//add path info
  if (lastResult==previousResult) return;
  if (previousResult&&lastResult.menu.title.prompt===previousResult.menu.title.prompt)
      walkStates(lastResult.menu,previousResult.menu);
  previousResult=lastResult;
}

function refreshBtn(_,o) {
  var p=$(o).attr("data-src");
  var g=$(o).attr("data-grp");
  var nv=jpath(g,lastResult.menu);
  if (nv.value==$(o).text())
    console.log(p,g,nv);
}

//get a single value out a menu state page by path
//it matches the page postion and selects requested item
//so path is absolute
function jpath(p,m) {
  if (m.path!=p.slice(0,m.path.length)) return;
  var ps=p.slice(m.path.length).split("/").slice(1);
  return _jpath(m,ps);
  function _jpath(m,o) {
    var i=(m.pad||m.items)
    if (!i&&o.length>0) return;
    return (o.length&&i)?_jpath(i[o[0]],o.slice(1)):m
  }
}

//traverses previous and last state and
//update controls of changed values
function walkStates(i,j) {
  if(!(i&&j)) return;
  // console.log("walking states...");
  if (i.path!=j.path) return;
  // console.log("path",i.path);
  if (i.field&&j.field&&i.field!=j.field) {//numeric field!
    var target=$("[data-src='"+i.path+"'");
    if (target.length) {
      console.log("setting new field",i.path,i.field,target);
      if (target.hasClass("slider")) target.slider("setValue",i.field);
      else target.val(i.field);
    }
  } if (i.options&&j.options&&i.value!=j.value) {//enumerated field
    var nvi=i.options.indexOf(i.value);
    console.log("looking for",i.path);
    var target=$("[data-src='"+i.path+"/"+nvi+"']");
    if (target.length) {
      console.log("clicking on button");
      target.click();
    } else {
      target=$("select[data-src='"+i.path+"']");
      if (target.length) {
        console.log("changing selection",i.path,i.select);
        target.val(i.select);
      }

    }
  }
  if (typeof i[Symbol.iterator] === 'function'&&typeof j[Symbol.iterator] === 'function')
    for(var n=0;n<i.length;n++)
      walkStates(i[n],j[n]);
  if(i.pad) walkStates(i.pad,j.pad)
  if(i.items) walkStates(i.items,j.items)
}

function mkPath(m) {
  if (m.menu) _mkPath(m.menu.items,m.menu.path||"")
  function _mkPath(i,path) {
    // console.log("_mkPath:",path,i);
    if (i.idx) i.path=path+"/"+i.idx;
    if (typeof i[Symbol.iterator] === 'function')
      for(n of i)
        _mkPath(n,i.path||path);
    if(i.pad) _mkPath(i.pad,i.path||path)
  }
}

function menu(o,cnt) {
  if(connection.readyState!=1) {//not connected (yet)?
    console.log("not connected yet...")
    if (connection.readyState==3) {//disconnected?
      console.log("connecting websocket");
      connectWS();
    }
    setTimeout(function() {//check latter...
      console.log("checking websocket connetion");
      if (connection.readyState!=1&&cnt>5) {//give up?
        alert("websocket disconnected, please refresh page");//TODO: put mutilang here
        return;
      } else menu(o,cnt?cnt+1:0);
    },500);
    return;
  }
  console.log("send ws",o);
  return connection.send(o);
}

function updateField(e) {
  return updateField_(e.target)
}

function updateField_(e) {
  var n = $(e).attr("data-src") + "/" + e.value + String.fromCharCode(255);
  menu(n);
}

function updateLink(e) {
  return updateLink_(e.target)
}

function updateLink_(e) {
  var n = $(e),
    o = n.attr("data-grp"),
    t = n.attr("data-src");
  // console.log("removing selections");
  $("a[data-grp='" + o + "']").removeClass("btn-primary btn-success");
  // console.log("adding selection to", e);
  n.addClass("btn-primary");
  // console.log("sending: " + t);
  menu(t);
  // connection.send(t)
}

function updateSelect(e) {
  updateSelect_(e.target)
}

function updateSelect_(e) {
  var n = $(e).attr("data-src") + "/" + e.selectedIndex;
  // console.log("sending: " + n);
  menu(n);
  // connection.send(n);
}

function init() {
  console.log("menu websocket server connected.");
  console.log("r-site menu page script starting.");
  activePanel = $(".aml_panel");
  $(".aml_op>input").removeAttr("onchange").off("change").on("change", updateField);
  $(".aml_op>select").removeAttr("onchange").off("change").on("change", updateSelect);
  $("a.aml_btn").removeAttr("href").off("click").on("click", updateLink);
  // $(".aml_op input.ledField").removeAttr("onchange").off("change").on("change", logField);
}
$(function() {
  // console.log("ready!");
  connectWS();
  $("p.out").text() && $("#outDlg").modal();
  jQuery().slider && $(".slider").slider({formatter: function(e) {return e + "%"}});
});

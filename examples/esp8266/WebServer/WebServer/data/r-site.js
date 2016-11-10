function updateField() {}
function init() {
  var f=document.querySelectorAll("input.field");
  for(var n=0;n<f.length;n++) {
    var i=f.item(n);
    console.log(i);
    i.onchange=function() {console.log(arguments);}
  }
}
window.onload = init();

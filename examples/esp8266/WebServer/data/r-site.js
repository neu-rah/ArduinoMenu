alert('scripts loaded');
function updateField() {}
function init() {
  var f=document.querySelector("input .field");
  for(o in f) console.log(o);
}
window.onload = init();

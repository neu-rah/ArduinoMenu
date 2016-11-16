function updateField(e) {
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
}

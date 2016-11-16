function updateField(e) {
  window.location.href=$(e.target).attr("data-path")+"/"+e.target.value;
}
function init() {
  console.log("r-site menu page script starting.");
  var f=$("input.aml_field");
  console.log(f);
  f.change(updateField);
}

const ansir = require('ansi-regex');

function pack(type,em,output) {
  var errors = [];
  output.split(/\r?\n/).forEach(line => {
    const error = em.exec(line);
    if (error) {
      console.log("file:",error[1]);
      var l=parseInt(error[2])-1;
      var c=parseInt(error[3])-1;
      errors.push({
        severity: type,
        location: {
          file: error[1],
          position: [[l,c],[l,c+1]]
        },
        excerpt: error[4]
      });
      // errors.push({
      //   // file: error[1].split("/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/")[1],
      //   file: error[1],//relative path? what a crap! we will be receiving request from library files and includes!
      //   line: error[2],
      //   col: error[3],
      //   message: error[4],
      //   trace: "zZz"
      // });
    }
  });
  console.log("errors:",errors);
  return errors;
}

module.exports = {
  name: "C++ main.cpp",
  sh: true,//else not capturing output
  cmd: "g++ main.cpp -o{PROJECT_PATH}/.out/am5 -I ~/Sketchbook/LIBDEV/ArduinoMenu/src -DNO_MENU_DEBUG -DMENU_DEBUG_OUT=cout -std=c++11 -ggdb",
  preBuild: function () {
    console.log('This is run **before** the build command');
  },
  postBuild: function () {
    console.log('This is run **after** the build command');
  },
  functionMatch: function (output) {
    output=output.split(ansir()).join('');//remove platformio ansi stuff
    // console.log("functionMatch",output);
    const em = /^([^:]*):([0-9]*):([0-9]*):\s*error:(.*)$/;
    const im = /^([^:]*):([0-9]*):([0-9]*):\s*note:(.*)$/;
    return pack("error",em,output).concat(pack("info",im,output));
  },
  targets: {
    run:{
      name: "C++ run",
      cmd: "{PROJECT_PATH}/.out/am5",
      keymap:"f5",
    }
  }
};

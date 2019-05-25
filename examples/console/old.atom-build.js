//this is for use with an heavy (incompatible and incomplete) modification of the original atom build package
//as the original author states the package needs to be completely rebuild to properlly integrate with atom lint 2.x
//delete it and use command line compile as this wont work with the original build package
//(Rui Azevedo)

const ansir = require('ansi-regex');

// function pack(type,em,output) {
//   var errors = [];
//   output.split(/\r?\n/).forEach(line => {
//     const error = em.exec(line);
//     if (error) {
//       console.log("file:",error[1]);
//       var l=parseInt(error[2])-1;
//       var c=parseInt(error[3])-1;
//       errors.push({
//         severity: type,
//         location: {
//           file: error[1],
//           position: [[l,c],[l,c+1]]
//         },
//         excerpt: error[4]
//       });
//     }
//   });
//   console.log("errors:",errors);
//   return errors;
// }

module.exports = {
  name: "C++ main.cpp",
  sh: true,//else not capturing output
  cmd: "g++ main.cpp -o{PROJECT_PATH}/.out/am5 -I{PROJECT_PATH}/src -DMENU_DEBUG -DMDO=cout -std=c++11 -ggdb",
  preBuild: function () {
    console.log('This is run **before** the build command');
  },
  postBuild: function () {
    console.log('This is run **after** the build command');
  },
  functionMatch: function (output) {
    output=output.split(ansir()).join('');//remove platformio ansi stuff
    // console.log("functionMatch",output);
    /*const em = /^([^:]*):([0-9]*):([0-9]*):\s*error:(.*)$/;
    const im = /^([^:]*):([0-9]*):([0-9]*):\s*note:(.*)$/;
    const rf = /^([^:]*):([0-9]*):([0-9]*):\s*(required\s*from\s*.+)$/;*/
    // const sel = /^([^:]*):([0-9]*):([0-9]*):\s*(error:|note:|required from)(.*)$/;
    const sel = /^([^:]*):([0-9]*):([0-9]*):\s*(error:|note:)(.*)$/;
    var msgs = [];
    output.split(/\r?\n/).forEach(line => {
      const ms = sel.exec(line);
      if (ms) {
        var file=ms[1];
        var l=parseInt(ms[2])-1;
        var c=parseInt(ms[3])-1;
        var type=ms[4]==="error:"?"error":"info";
        var msg=(type==="error"?"":ms[4])+ms[5];
        msgs.push({
          severity: type,
          location: {
            file: file,
            position: [[l,c],[l,c+1]]
          },
          excerpt: msg
        });
      }
    });
    console.log(msgs);
    return msgs;
    // return    pack("error",em,output)
    //   .concat(pack("info",im,output))
    //   .concat(pack("info",rf,output));
  },
  targets: {
    run:{
      name: "C++ run",
      cmd: "{PROJECT_PATH}/.out/am5",
      keymap:"f5",
    }
  }
};

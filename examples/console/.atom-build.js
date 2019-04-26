module.exports = {
  name: "C++ main.cpp",
  cmd: "g++ main.cpp -o{PROJECT_PATH}/.out/am5 -I ~/Sketchbook/LIBDEV/ArduinoMenu/src -DNO_MENU_DEBUG -DMENU_DEBUG_OUT=cout -std=c++11 -ggdb",
  preBuild: function () {
    console.log('This is run **before** the build command');
  },
  postBuild: function () {
    console.log('This is run **after** the build command');
  },
  functionMatch: function (output) {
    const em = /^([^:]*):([0-9]*):([0-9]*):\s*error:(.*)$/;
    var errors = [];
    output.split(/\r?\n/).forEach(line => {
      const error = em.exec(line);
      if (error)
        errors.push({
          // file: error[1].split("/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/")[1],
          file: error[1],
          line: error[2],
          col: error[3],
          message: error[4],
          trace: "zZz"
        });
    });
    console.log("errors:",errors);
    return errors;
  },
  targets: {
    run:{
      name: "C++ run",
      cmd: "{PROJECT_PATH}/.out/am5",
      keymap:"f5",
    }
  }
};

{
  qmk,
  fetchFromGitHub,
  stdenv,
  git,
}:
rec {
  qmk_firmware = fetchFromGitHub {
    owner = "qmk";
    repo = "qmk_firmware";
    rev = "0.30.3";
    sha256 = "sha256-+BhxRbH+o++/4vm9NAJ7cE1EyTHGG2v5/+L/7xahFvI=";
    fetchSubmodules = true;
    leaveDotGit = true;
  };
  compile =
    {
      keyboard,
      keymap,
      variant ? "",
    }:
    let
      buildDir = "build";
      keyboardDir = "keyboards/${keyboard}";
      keyboardVariant = if variant == "" then "${keyboard}" else "${keyboard}/${variant}";
      keymapName = "qmk_playground";
      keymapDir = "${keyboardDir}/keymaps/${keymapName}";
    in
    stdenv.mkDerivation {
      name = "qmk-playground";
      src = qmk_firmware;
      buildInputs = [
        qmk
        git
      ];
      postPatch = ''
        mkdir -p ${keymapDir}
        cp -r ${keymap}/* ${keymapDir}/
      '';
      buildPhase = ''
        qmk compile \
          --env BUILD_DIR=${buildDir} \
          --keyboard ${keyboardVariant} \
          --keymap ${keymapName}
      '';
      installPhase = ''
        mkdir -p $out/bin
        cp ${buildDir}/*.{hex,bin,elf,dfu,uf2,eep} $out/bin
      '';
      dontFixup = true;
    };
}

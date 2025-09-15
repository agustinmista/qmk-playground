{
  description = "QMK Playground";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;

        # QMK out-of-tree builder derivation
        qmk-playground = pkgs.callPackage ./qmk-playground.nix { };

        # Reify keyboard configurations from the keyboards directory
        keyboardsDir = ./keyboards;
        subdirPath = subdir: keyboardsDir + "/${subdir}";
        toConfig = name: _: import (subdirPath name) // { keymap = subdirPath name; };
        keyboardSubdirs = lib.filterAttrs (_: type: type == "directory") (builtins.readDir keyboardsDir);
        keyboardConfigs = builtins.mapAttrs toConfig keyboardSubdirs;
      in
      {
        devShells.default = pkgs.mkShell {
          name = "qmk-playground";
          buildInputs = [
            pkgs.qmk
          ];
        };
        packages = pkgs.lib.mapAttrs (_: kbd: qmk-playground.compile kbd) keyboardConfigs;
      }
    );
}

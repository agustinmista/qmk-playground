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
        qmk-playground = pkgs.callPackage ./qmk-playground.nix { };
        keyboards = import ./keyboards;
      in
      {
        devShells.default = pkgs.mkShell {
          name = "qmk-playground";
          buildInputs = [
            pkgs.qmk
          ];
        };
        packages = {
          firmware = pkgs.lib.mapAttrs (_: kbd: qmk-playground.compile kbd) keyboards;
        };
      }
    );
}

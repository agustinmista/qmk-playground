{
  description = "QMK Playground";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nixcaps.url = "github:agustinmista/nixcaps";
  };

  outputs =
    {
      nixpkgs,
      flake-utils,
      nixcaps,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        compile = nixcaps.packages.${system}.compile;
        lib = pkgs.lib;

        forEachSubPackage =
          dir: f:
          let
            subdirs = lib.filterAttrs (_: type: type == "directory") (builtins.readDir dir);
            subpkgs = lib.mapAttrs (name: _: pkgs.callPackage (dir + "/${name}") { }) subdirs;
          in
          builtins.mapAttrs (_: pkg: f pkg) subpkgs;
      in
      {
        packages = forEachSubPackage ./keyboards compile;
      }
    );
}

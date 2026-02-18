{
  description = "QMK Playground";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nixcaps.url = "github:agustinmista/nixcaps";
  };

  outputs =
    inputs@{
      nixpkgs,
      flake-utils,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;
        nixcaps = inputs.nixcaps.lib.${system};

        # Read all the keyboard definitions from the `keyboards` directory
        keyboardsDir = "keyboards";

        keyboards = lib.mapAttrs (name: _: import ./${keyboardsDir}/${name}) (
          lib.filterAttrs (_: type: type == "directory") (builtins.readDir ./${keyboardsDir})
        );

        # Map a function over all the keyboard definitions
        forEachKeyboard = f: builtins.mapAttrs (_: pkg: f pkg) keyboards;
      in
      {
        # Build firmware with `nix build .#<keyboard>`
        packages = forEachKeyboard nixcaps.mkQmkFirmware;

        # Flash firmwares with `nix run .#<keyboard>`
        apps = forEachKeyboard nixcaps.flashQmkFirmware;

        # Default shell with prebuilt compile_commands.json for all keyboards
        devShells.default = pkgs.mkShell {
          QMK_HOME = "${inputs.nixcaps.inputs.qmk_firmware}";
          buildInputs = [ pkgs.qmk ];
          shellHook = builtins.concatStringsSep "\n" (
            builtins.attrValues (
              builtins.mapAttrs (name: compileCommands: ''
                echo "> Setting up compile_commands for ${name} ..."
                ln -sf \
                  ${compileCommands}/compile_commands.json \
                  ${keyboardsDir}/${name}/compile_commands.json
              '') (forEachKeyboard nixcaps.mkCompileDb)
            )
          );
        };
      }
    );
}

{
  description = "QMK Playground";
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-24.05";
  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      devShell.${system} = pkgs.mkShell {
        name = "qmk-playground";
        buildInputs = [
          pkgs.clang-tools
          pkgs.dfu-programmer
          pkgs.dfu-util
          pkgs.diffutils
          pkgs.qmk
        ];
      };
    };
}

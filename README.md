# QMK Playground

[![Build QMK firmware](https://github.com/agustinmista/qmk-playground/actions/workflows/build.yml/badge.svg)](https://github.com/agustinmista/qmk_playground/actions/workflows/build.yml)

This project aims to provide a simple out-of-tree solution to build custom QMK keyboard firmware using Nix.

## Usage:

1. Create a folder `keyboards/<your-kbd>/` and add:
   - Your custom keymap files (e.g. `keymap.c`, `config.h`, `rules.mk`).
   - A `default.nix` file with a function returning an attribute set with the fields:
     - `keyboard`: the path inside `qmk_firmware/keyboards` to the root of your keyboard model (e.g. `preonic` or `drop/thekey/v2`).
       - The Nix derivation will copy `keyboards/<your-kbd>` into `qmk_firmware/keyboards/<keyboard>/keymaps/<your-kbd>`.
     - `variant` (optional): the concrete variant of your keyboard, in case there exist more than one.
       - This is what you usually need to pass to via `--keyboard` to `qmk compile` in addition to the `<keyboard>` name. (e.g. `--keyboard preonic/rev3_drop` -> `{ keyboard = "preonic"; variant = "rev3_drop"; }`).
       - If empty, it defaults to calling `qmk compile` with just `--keyboad <keyboard>`.
     - `target` (optional): the basename used for the firmware file. Defaults to `fw`.
     - `flash` (optional): a function that receives the resolved path to the compiled firmware (without extension!) and returns a list of commands to flash it into your keyboard.
       - When provided, these commands will be interpolated into an executable script in `result/bin/flash`.
       - You can use dependencies from Nixpkgs by adding them as inputs to the top-level function. See `keyboards/ergodox_ez` for a concrete example.
       - This is until I figure how to do this via `qmk flash` with Nix in an efficient way.
2. Make sure to `git add` you new files.
3. Run `nix build .#<your-kbd>`.
   - The first time will take slightly longer, as we need to download the `qmk_firmware` repo and its submodules.
   - Your compiled firmware will be linked to `result/bin/<target>.{hex,bin,...}`.
4. Flash the firmware with your favorite utility or via `result/bin/flash` if `flash` was provided.

## CI/CD

This project provides a GitHub action that builds and uploads the firmware for every keymap defined in `keyboards`. These firmwares can be found at the bottom of the summary of each CI run.

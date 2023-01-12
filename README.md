# QMK Playground

[![Build QMK firmware](https://github.com/agustinmista/qmk_playground/actions/workflows/build.yml/badge.svg)](https://github.com/agustinmista/qmk_playground/actions/workflows/build.yml)

This project aims to provide a simple out-of-tree solution to build custom QMK keyboard firmware without having to install any tooling (other than Visual Studio Code and Docker). The build process is designed to run inside the official `qmk_cli` Docker image, and there's a devcontainer definition ready to use in Visual Studio Code. Finally, a GitHub action discovers, builds, and publishes all the firmwares on push.

## Usage:

1. Fork this repository and open it in Visual Studio Code
2. A wild pop-up appears! Press "Reopen in Container"
3. Create a folder `<your-kbd>` under `keyboards`
4. In `keyboards/<your-kbd>/` add:
   * Your custom keymap files (e.g. `keymap.c`, `config.h`, `rules.mk`)
   * A file called `env` with the following variables:
     + `KBD_PATH`: The path inside `qmk_firmware/keyboards` to your keyboard model (e.g. `preonic` or `massdrop/thekey_v2`)
     + `KBD_VARIANT`: The concrete variant of your keyboard, in case there exist more than one (e.g. `preonic/rev3_drop`)
5. Build your custom firmware running `make KBD=<your-kbd>`
6. If everything goes as planned, your firmware should be placed inside the `build` directory

## CI/CD

This project provides a GitHub action that builds and uploads the firmware for every keymap defined in `keyboards`. These firmwares can be found at the bottom of the summary of each CI run. Sadly, there's no way to retrieve these artifacts programmatically, so down below we use [nightly.link](https://nightly.link/) to create permalinks to them. If you want to use it too, you need to authorize this app in your repository and change the link accordingly.

### Prebuilt firmwares

The latest prebuilt firmware are available [here](https://nightly.link/agustinmista/qmk_playground/workflows/build/main).

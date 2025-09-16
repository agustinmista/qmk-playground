# QMK Playground

[![Build QMK firmware](https://github.com/agustinmista/qmk-playground/actions/workflows/build.yml/badge.svg)](https://github.com/agustinmista/qmk_playground/actions/workflows/build.yml)

## Usage

```bash
$ # compile the firmware
$ nix build .#<keyboard>
$ # flash it (if a flasher script was provided)
$ sudo ./result/bin/flash
```

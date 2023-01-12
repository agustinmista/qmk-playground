# qmk_playground

My QMK configuration, build environment, and CI/CD.

The build process is designed to run inside the official `qmk_cli` Docker image, and there's a devcontainer definition ready to use for this purpose in Visual Studio Code. For convenience, a GitHub action discovers, builds, and publishes all the firmwares on push.

Usage:

```bash
$ make KBD=<the-kbd>
```

Where `<the-kbd>` must be a folder defined inside `keyboards`. This folder must contain all the keymap files, as well as an `env` file defining the `KBD_PATH` and
`KBD_VARIANT` variables. `KBD_PATH` should point to the relative path of the corresponding keyboard within `qmk_firmware/keyboards`, whereas `KBD_VARIANT` is the keyboard variant passed via `-kb` to `qmk_firmware`'s build system.

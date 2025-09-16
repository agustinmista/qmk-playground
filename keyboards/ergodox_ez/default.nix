{ teensy-loader-cli }:
{
  keyboard = "ergodox_ez";
  variant = "base";
  src = ./.;
  flash = target: ''
    echo "Flashing ${target}.hex ..."
    ${teensy-loader-cli}/bin/teensy-loader-cli -mmcu=atmega32u4 -v -w ${target}.hex
  '';
}

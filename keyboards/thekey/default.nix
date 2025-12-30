{ dfu-programmer }:
{
  keyboard = "drop/thekey/v2";
  src = ./.;
  flash = target: ''
    echo "Flashing ${target}.hex ..."
    ${dfu-programmer}/bin/dfu-programmer atmega32u4 erase
    ${dfu-programmer}/bin/dfu-programmer atmega32u4 flash ${target}.hex
    ${dfu-programmer}/bin/dfu-programmer atmega32u4 reset
  '';
}

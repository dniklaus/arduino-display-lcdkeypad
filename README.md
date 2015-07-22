arduino-display-lcdkeypad
=========================

Arduino LCD Keypad Shield driver, supporting both [LiquidCrystal](http://arduino.cc/en/Reference/LiquidCrystal) (4 or 8 bit parallel data bus) and [LiquidTWI2](https://github.com/lincomatic/LiquidTWI2) (I2C bus and RGB color backlight) based HW types.

The LiquidCrystal HW can be ordered from here:
* http://www.robotshop.com/dfrobot-lcd-keypad-shield-arduino-1.html
* http://hobbyking.com/hobbyking/store/uh_viewItem.asp?idProduct=25087
* http://www.dx.com/p/arduino-2-6-lcd-keypad-shield-expansion-board-134404#.U4z6LygUdpU

Uses the LiquidCrystal driver (http://arduino.cc/en/Reference/LiquidCrystal)



The LiquidTWI2 HW can be ordered from here:
* http://www.adafruit.com/product/772 (blue & white)
* http://www.adafruit.com/products/716 (RGB color, negative)
* http://www.adafruit.com/products/398 (RGB color, positive)

Uses the LiquidTWI2 driver (https://github.com/lincomatic/LiquidTWI2)



Supports the 5 key pad buttons (one button at a time only)

Requires:

* LiquidCrystal (Arduino Library: http://arduino.cc/en/Reference/LiquidCrystal)
* LiquidTWI2 (A lean, high speed I2C LCD Library for Arduino: https://github.com/lincomatic/LiquidTWI2)
* Timer (https://github.com/dniklaus/arduino-utils-timer)

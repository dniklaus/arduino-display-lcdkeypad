[![Stories in Ready](https://badge.waffle.io/dniklaus/arduino-display-lcdkeypad.png?label=ready&title=Ready)](https://waffle.io/dniklaus/arduino-display-lcdkeypad)
arduino-display-lcdkeypad
=========================

Arduino LCD Keypad Shield driver, supporting two HW types: 
* 4 or 8 bit parallel data bus ([LiquidCrystal](http://arduino.cc/en/Reference/LiquidCrystal) driver)
* I2C bus and RGB color backlight ([LiquidTWI2](https://github.com/lincomatic/LiquidTWI2) driver)

The HW type supported by **LiquidCrystal** can be ordered from here:
* https://www.robotshop.com/en/lcd-keypad-shield-arduino.html
* https://hobbyking.com/en_us/kingduino-lcd-keypad-shield-1.html
* http://www.dx.com/p/arduino-2-6-lcd-keypad-shield-expansion-board-134404#.U4z6LygUdpU

The schematic of this LcdKeypad Shield can be found [here](http://forum.arduino.cc/index.php?action=dlattach;topic=95618.0;attach=15041).

Uses the LiquidCrystal driver (http://arduino.cc/en/Reference/LiquidCrystal)



The **LiquidTWI2** HW can be ordered from here:
* http://www.adafruit.com/product/772 (blue & white)
* http://www.adafruit.com/products/716 (RGB color, negative)

Uses a copy of the files taken from the LiquidTWI2 driver: https://github.com/lincomatic/LiquidTWI2



This driver supports the 5 key pad buttons (one button at a time only).
This driver detects whether a LiquidTWI2 HW or a LiquidCrystal HW type is present and automatically chooses the appropriate driver variant.
As long as this driver is used, the **I2C address 0x20** is reserved for the LiquidTWI2 HW type (even when you are actually using the LiquidCrystal HW type).

**Requires**:
* Wire (Arduino Library: https://www.arduino.cc/en/Reference/Wire)
* LiquidCrystal (Arduino Library: http://arduino.cc/en/Reference/LiquidCrystal)
* Timer (https://github.com/dniklaus/wiring-timer, Arduino Library Manager: wiring-timer



**Usage**:

```cpp
// Timer library, https://github.com/dniklaus/arduino-utils-timer
#include <Timer.h>

// LcdKeypad, https://github.com/dniklaus/arduino-display-lcdkeypad
#include <LcdKeypad.h>

LcdKeypad* myLcdKeypad = 0;

// Implement specific LcdKeypadAdapter in order to allow receiving key press events
class MyLcdKeypadAdapter : public LcdKeypadAdapter
{
private:
  LcdKeypad* m_lcdKeypad;
  unsigned char m_value;
public:
  MyLcdKeypadAdapter(LcdKeypad* lcdKeypad)
  : m_lcdKeypad(lcdKeypad)
  , m_value(7)
  { }

  // Specific handleKeyChanged() method implementation - define your actions here
  void handleKeyChanged(LcdKeypad::Key newKey)
  {
    if (0 != m_lcdKeypad)
    {
      if (LcdKeypad::UP_KEY == newKey)
      {
        m_value++;
      }
      else if (LcdKeypad::DOWN_KEY == newKey)
      {
        m_value--;
      }
      m_lcdKeypad->setCursor(0, 1);  // position the cursor at beginning of the second line
     
      m_lcdKeypad->print(m_value);             // print the value on first line of the display
      m_lcdKeypad->print("                ");  // wipe out characters behind the printed value
     
      // RGB colored backlight: set according to the current value
      // monochrome backlight: set backlight on or off according to the current value
      m_lcdKeypad->setBacklight(static_cast<LcdKeypad::LcdBacklightColor>(LcdKeypad::LCDBL_WHITE & m_value));
    }
  }
};
    
void setup()
{
  myLcdKeypad = new LcdKeypad();  // instatiate an object of the LcdKeypad class, using default parameters
  
  // Attach the specific LcdKeypadAdapter implementation (dependency injection)
  myLcdKeypad->attachAdapter(new MyLcdKeypadAdapter(myLcdKeypad));
  
  myLcdKeypad->setCursor(0, 0);   // position the cursor at beginning of the first line
  myLcdKeypad->print("Value:");   // print a Value label on the second line of the display
}
    
void loop()
{
  scheduleTimers();  // Get the timer(s) ticked, in particular the LcdKeypad dirver's keyPollTimer
}
```


**Description**:

In the global area (outside of the `setup()` and `loop()` functions), define a specific `LcdKeypadAdapter` implementation, particularly implement the `handleKeyChanged()` method where you define the actions to be performed on specific key press events.

In the `setup()` function instatiate an object of the `LcdKeypad` class. Here the appropriate driver type will be selected according to the present HW. Attach your specific `LcdKeypadAdapter` implementation to the driver so you get the key pressed notifications.

In the `loop()` function just let the timer get ticked by calling `scheduleTimers()`. This will keep the key pressed event detection running.

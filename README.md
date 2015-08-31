arduino-display-lcdkeypad
=========================

Arduino LCD Keypad Shield driver, supporting both [LiquidCrystal](http://arduino.cc/en/Reference/LiquidCrystal) (4 or 8 bit parallel data bus) and [LiquidTWI2](https://github.com/lincomatic/LiquidTWI2) (I2C bus and RGB color backlight) based HW types.

The **LiquidCrystal** HW can be ordered from here:
* http://www.robotshop.com/dfrobot-lcd-keypad-shield-arduino-1.html
* http://hobbyking.com/hobbyking/store/uh_viewItem.asp?idProduct=25087
* http://www.dx.com/p/arduino-2-6-lcd-keypad-shield-expansion-board-134404#.U4z6LygUdpU

The schematic of this LcdKeypad Shield can be found [here](http://forum.arduino.cc/index.php?action=dlattach;topic=95618.0;attach=15041).

Uses the LiquidCrystal driver (http://arduino.cc/en/Reference/LiquidCrystal)



The **LiquidTWI2** HW can be ordered from here:
* http://www.adafruit.com/product/772 (blue & white)
* http://www.adafruit.com/products/716 (RGB color, negative)
* http://www.adafruit.com/products/398 (RGB color, positive)

Uses the LiquidTWI2 driver (https://github.com/lincomatic/LiquidTWI2)


This driver supports the 5 key pad buttons (one button at a time only).

**Requires**:
* Wire (Arduino Library)
* LiquidCrystal (Arduino Library: http://arduino.cc/en/Reference/LiquidCrystal)
* LiquidTWI2 (A lean, high speed I2C LCD Library for Arduino: https://github.com/lincomatic/LiquidTWI2)
* Timer (https://github.com/dniklaus/arduino-utils-timer)



**Usage**:

    // Timer library, https://github.com/dniklaus/arduino-utils-timer
    #include <Timer.h>
    
    // LcdKeypad, https://github.com/dniklaus/arduino-display-lcdkeypad
    #include <LcdKeypad.h>
    
    LcdKeypad* myLcdKeypad = 0;
    
    class MyLcdKeypadAdapter : public LcdKeypadAdapter
    {
    private:
      LcdKeypad* m_lcdKeypad;
      unsigned char m_value;
    public:
      MyLcdKeypadAdapter(LcdKeypad* lcdKeypad)
      : m_lcdKeypad(lcdKeypad)
      , m_value(0)
      { }
      
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
          m_lcdKeypad->setBacklight(static_cast<LcdKeypad::LcdBacklightColor>(LcdKeypad::LCDBL_WHITE & m_value));
        }
      }
    };
    
    setup()
    {
      // use this line if you use a I2C based LcdKeypad shield
      myLcdKeypad = new LcdKeypad(LcdKeypad::MCPT_MCP23017, 0x20, LcdKeypad::LCD_DT_TWI2);

      // use this line if you use a 4 bit parallel data LcdKeypad shield
      //  myLcdKeypad = new LcdKeypad(LcdKeypad::LCD_DT_CRYST);
      
      myLcdKeypad->attachAdapter(new MyLcdKeypadAdapter(myLcdKeypad));
    }
    
    loop
    {
      scheduleTimers();
    }

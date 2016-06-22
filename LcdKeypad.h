/*
 * LcdKeypad.h
 *
 *  Created on: 15.10.2013
 *      Author: niklausd
 */

#ifndef LCDKEYPAD_H_
#define LCDKEYPAD_H_

#include "Print.h"

class Timer;
class LcdKeypadAdapter;
class LiquidTWI2;
class LiquidCrystal;

#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LcdKeypad : public Print
{
public:
  typedef enum
  {
      LCD_DT_TWI2  = 0
    , LCD_DT_CRYST = 1
  } LcdDeviceType;

  typedef enum
  {
      LCDC_5x8DOTS  = 0x00
    , LCDC_5x10DOTS = 0x04
  } LcdCharDots;

  typedef enum
  {
      MCPT_MCP23008 = 0
    , MCPT_MCP23017 = 1
  } MCPType;

  typedef enum
  {
      LCDBL_OFF     = 0
    , LCDBL_RED     = 1
    , LCDBL_GREEN   = 2
    , LCDBL_YELLOW  = 3
    , LCDBL_BLUE    = 4
    , LCDBL_VIOLET  = 5
    , LCDBL_TEAL    = 6
    , LCDBL_WHITE   = 7
  } LcdBacklightColor;

  typedef enum
  {
      NO_KEY        = 0
    , SELECT_KEY    = 1
    , LEFT_KEY      = 2
    , UP_KEY        = 3
    , DOWN_KEY      = 4
    , RIGHT_KEY     = 5
  } Key;

  /**
   *  self selecting constructor (tries LCD_DT_TWI2 first, and then LCD_DT_CRYST if LCD_DT_TWI2 not successfully found)
   */
  LcdKeypad(MCPType mcptype = MCPT_MCP23017, uint8_t i2cAddr = 0x20, uint8_t detectDevice = 1, uint8_t backlightInverted = 0,
            int lcdRSPin = s_defaultLcdRSPin, int lcdEnPin = s_defaultLcdEnPin,
            int lcdD4Pin = s_defaultLcdD4Pin, int lcdD5Pin = s_defaultLcdD5Pin,
            int lcdD6Pin = s_defaultLcdD6Pin, int lcdD7Pin = s_defaultLcdD7Pin,
            int lcdBackLightCtrlPin = s_defaultLcdBackLightCtrlPin, bool isLcdBackLightOn = s_defaultIsLcdBackLightOn);

  virtual ~LcdKeypad();

  void setBackLightOn(bool isLcdBackLightOn);

  void handleButtons();
  Key getCurrentKey();

  bool isNoKey();
  bool isUpKey();
  bool isDownKey();
  bool isSelectKey();
  bool isLeftKey();
  bool isRightKey();

  void attachAdapter(LcdKeypadAdapter* adapter);
  LcdKeypadAdapter* adapter();

public:
  void begin(uint8_t cols, uint8_t rows, uint8_t dotsize = LCDC_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  void createChar(uint8_t location, uint8_t charmap[]);
  void setCursor(uint8_t col, uint8_t row);

  void setBacklight(LcdBacklightColor lcdBacklightColor);

#if defined(ARDUINO) && (ARDUINO >= 100) //scl
  size_t write(uint8_t value);
#else
  void write(uint8_t value);
#endif

  void command(uint8_t value);

  void setMCPType(MCPType mcptype);

private:
  void setBackLightControl();
  uint8_t readButtons();

private:
  int               m_lcdBackLightCtrlPin;
  LcdBacklightColor m_backlightColor;
  Key               m_currentKey;
  Timer*            m_keyPollTimer;
  LcdKeypadAdapter* m_adapter;
  LiquidTWI2*       m_liquidTwi2;
  LiquidCrystal*    m_liquidCrystal;

  static const int  s_defaultLcdRSPin;
  static const int  s_defaultLcdEnPin;
  static const int  s_defaultLcdD4Pin;
  static const int  s_defaultLcdD5Pin;
  static const int  s_defaultLcdD6Pin;
  static const int  s_defaultLcdD7Pin;
  static const int  s_defaultLcdBackLightCtrlPin;
  static const bool s_defaultIsLcdBackLightOn;

  static const int  s_defaultKeyAdcPin;
  static const int  s_defaultKeyPollTime;
  static const int  s_rightKeyLimit;
  static const int  s_upKeyLimit;
  static const int  s_downKeyLimit;
  static const int  s_leftKeyLimit;
  static const int  s_selectKeyLimit;

private: // forbidden default functions
  LcdKeypad& operator = (const LcdKeypad& src);  // assignment operator
  LcdKeypad(const LcdKeypad& src);               // copy constructor
};

//-----------------------------------------------------------------------------

class LcdKeypadAdapter
{
public:
  virtual void handleKeyChanged(LcdKeypad::Key newKey) = 0;
  virtual ~LcdKeypadAdapter() { }

protected:
  LcdKeypadAdapter() { }

private: // forbidden default functions
  LcdKeypadAdapter& operator = (const LcdKeypadAdapter& src);  // assignment operator
  LcdKeypadAdapter(const LcdKeypadAdapter& src);               // copy constructor
};

//-----------------------------------------------------------------------------

#endif /* LCDKEYPAD_H_ */

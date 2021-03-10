/*
 * LcdKeypad.cpp
 *
 *  Created on: 15.10.2013
 *      Author: niklausd
 */

#include "Arduino.h"
#include "Wire.h"
#include "SpinTimer.h"
#include "LiquidTWI2.h"
#include "LiquidCrystal.h"
#include "LcdKeypad.h"

//=========================================================

class KeyPollTimerAction : public SpinTimerAction
{
public:
  KeyPollTimerAction(LcdKeypad* lcdKeypad)
  : m_lcdKeypad(lcdKeypad)
  { }

  void timeExpired()
  {
    if (0 != m_lcdKeypad)
    {
      m_lcdKeypad->handleButtons();
    }
  }

private:
  LcdKeypad* m_lcdKeypad;
};

//=========================================================

const int  LcdKeypad::s_defaultLcdRSPin    = 8;
const int  LcdKeypad::s_defaultLcdEnPin    = 9;
const int  LcdKeypad::s_defaultLcdD4Pin    = 4;
const int  LcdKeypad::s_defaultLcdD5Pin    = 5;
const int  LcdKeypad::s_defaultLcdD6Pin    = 6;
const int  LcdKeypad::s_defaultLcdD7Pin    = 7;

const int  LcdKeypad::s_defaultLcdBackLightCtrlPin = 10;
const bool LcdKeypad::s_defaultIsLcdBackLightOn = false;

const int  LcdKeypad::s_defaultKeyAdcPin   = A0;
const int  LcdKeypad::s_defaultKeyPollTime = 50;

const int  LcdKeypad::s_rightKeyLimit      =  65;
const int  LcdKeypad::s_upKeyLimit         = 221;
const int  LcdKeypad::s_downKeyLimit       = 395;
const int  LcdKeypad::s_leftKeyLimit       = 602;
const int  LcdKeypad::s_selectKeyLimit     = 873;

LcdKeypad::LcdKeypad(MCPType mcptype, uint8_t i2cAddr, uint8_t detectDevice, uint8_t backlightInverted,
                     int lcdRSPin, int lcdEnPin, int lcdD4Pin, int lcdD5Pin, int lcdD6Pin, int lcdD7Pin,
                     int lcdBackLightCtrlPin, bool isLcdBackLightOn)
: m_lcdBackLightCtrlPin(lcdBackLightCtrlPin)
, m_backlightColor(LCDBL_OFF)
, m_currentKey(NO_KEY)
, m_keyPollTimer(new SpinTimer(s_defaultKeyPollTime, new KeyPollTimerAction(this), SpinTimer::IS_RECURRING, SpinTimer::IS_AUTOSTART))
, m_adapter(0)
, m_liquidTwi2(0)
, m_liquidCrystal(0)
{
  Wire.begin();

  // first try to reach the LiquidTWI2 through I2C
  Wire.beginTransmission(i2cAddr);
  int error = Wire.endTransmission();
  if (0 == error)
  {
    // the device is reachable so now create LiquidTWI2 object
    m_liquidTwi2 = new LiquidTWI2(i2cAddr, detectDevice, backlightInverted);
    setMCPType(mcptype);

    // scan for the device & set up the LCD's number of columns and rows:
    begin(16, 2);

    if (0 == m_liquidTwi2->LcdDetected())
    {
	    // device not detected (might be another item on this I2C address)
      delete m_liquidTwi2; 
      m_liquidTwi2 = 0;
    }
    else
    {
      setBackLightControl();
    }
  }

  if (0 == m_liquidTwi2)
  {
    // no success with LiquidTWI2, try the LiquidCrystal type
    m_liquidCrystal = new LiquidCrystal(lcdRSPin, lcdEnPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);

    setBackLightControl();

    // button adc input
    pinMode(s_defaultKeyAdcPin, INPUT);         // ensure Key ADC pin is an input
    digitalWrite(s_defaultKeyAdcPin, LOW);      // ensure pull-up is off on Key ADC pin

    // set up the LCD's number of columns and rows:
    begin(16, 2);
  }
}

LcdKeypad::~LcdKeypad()
{
  delete m_liquidCrystal; 
  m_liquidCrystal = 0;
  
  delete m_liquidTwi2;    
  m_liquidTwi2    = 0;
  
  delete m_keyPollTimer->action();
  m_keyPollTimer->attachAction(0);

  delete m_keyPollTimer;  
  m_keyPollTimer  = 0;
}

void LcdKeypad::attachAdapter(LcdKeypadAdapter* adapter)
{
  m_adapter = adapter;
}

LcdKeypadAdapter* LcdKeypad::adapter()
{
  return m_adapter;
}

void LcdKeypad::setBacklight(LcdBacklightColor lcdBacklightColor)
{
  m_backlightColor = lcdBacklightColor;
  setBackLightControl();
}

void LcdKeypad::setBackLightOn(bool isLcdBackLightOn)
{
  m_backlightColor = isLcdBackLightOn ? LCDBL_WHITE : LCDBL_OFF;
  setBackLightControl();
}

void LcdKeypad::setBackLightControl()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->setBacklight(m_backlightColor);
  }
  else if (0 != m_liquidCrystal)
  {
    bool isLcdBackLightOn = (LCDBL_OFF != m_backlightColor);
    if (isLcdBackLightOn)
    {
      Serial.println("LcdKeypad::setBackLightControl, isLcdBackLightOn=1");
      pinMode(m_lcdBackLightCtrlPin, INPUT);
      digitalWrite(m_lcdBackLightCtrlPin, LOW);
    }
    else
    {
      Serial.println("LcdKeypad::setBackLightControl, isLcdBackLightOn=0");
      pinMode(m_lcdBackLightCtrlPin, OUTPUT);
      digitalWrite(m_lcdBackLightCtrlPin, LOW);
    }
  }
}

void LcdKeypad::handleButtons()
{
  Key polledKey = NO_KEY;

  if (0 != m_liquidTwi2)
  {
    uint8_t buttons = m_liquidTwi2->readButtons();
    if (buttons)
    {
      if (buttons & BUTTON_RIGHT)
      {
        polledKey = RIGHT_KEY;
      }
      else if (buttons & BUTTON_UP)
      {
        polledKey = UP_KEY;
      }
      else if (buttons & BUTTON_DOWN)
      {
        polledKey = DOWN_KEY;
      }
      else if (buttons & BUTTON_LEFT)
      {
        polledKey = LEFT_KEY;
      }
      else if (buttons & BUTTON_SELECT)
      {
        polledKey = SELECT_KEY;
      }
    }
  }
  else if (0 != m_liquidCrystal)
  {
    int keyPress = analogRead(s_defaultKeyAdcPin);

    if (keyPress < s_rightKeyLimit)
    {
      polledKey = RIGHT_KEY;
    }
    else if (keyPress < s_upKeyLimit)
    {
      polledKey = UP_KEY;
    }
    else if (keyPress < s_downKeyLimit)
    {
      polledKey = DOWN_KEY;
    }
    else if (keyPress < s_leftKeyLimit)
    {
      polledKey = LEFT_KEY;
    }
    else if (keyPress < s_selectKeyLimit)
    {
      polledKey = SELECT_KEY;
    }
  }

  if (polledKey != m_currentKey)
  {
    m_currentKey = polledKey;
    if (0 != m_adapter)
    {
      m_adapter->handleKeyChanged(polledKey);
    }
  }
}

LcdKeypad::Key LcdKeypad::getCurrentKey()
{
  return m_currentKey;
}

bool LcdKeypad::isNoKey()
{
  return (NO_KEY == m_currentKey);
}

bool LcdKeypad::isUpKey()
{
  return (UP_KEY == m_currentKey);
}

bool LcdKeypad::isDownKey()
{
  return (DOWN_KEY == m_currentKey);
}

bool LcdKeypad::isSelectKey()
{
  return (SELECT_KEY == m_currentKey);
}

bool LcdKeypad::isLeftKey()
{
  return (LEFT_KEY == m_currentKey);
}

bool LcdKeypad::isRightKey()
{
  return (RIGHT_KEY == m_currentKey);
}

void LcdKeypad::begin(uint8_t cols, uint8_t rows, uint8_t dotsize)
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->begin(cols, rows, dotsize);
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->begin(cols, rows, dotsize);
  }
}

void LcdKeypad::clear()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->clear();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->clear();
  }
}

void LcdKeypad::home()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->home();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->home();
  }
}

void LcdKeypad::noDisplay()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->noDisplay();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->noDisplay();
  }
}

void LcdKeypad::display()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->display();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->display();
  }
}

void LcdKeypad::noBlink()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->noBlink();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->noBlink();
  }
}

void LcdKeypad::blink()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->blink();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->blink();
  }
}

void LcdKeypad::noCursor()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->noCursor();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->noCursor();
  }
}

void LcdKeypad::cursor()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->cursor();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->cursor();
  }
}

void LcdKeypad::scrollDisplayLeft()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->scrollDisplayLeft();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->scrollDisplayLeft();
  }
}

void LcdKeypad::scrollDisplayRight()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->scrollDisplayRight();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->scrollDisplayRight();
  }
}

void LcdKeypad::leftToRight()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->leftToRight();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->leftToRight();
  }
}

void LcdKeypad::rightToLeft()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->rightToLeft();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->rightToLeft();
  }
}

void LcdKeypad::autoscroll()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->autoscroll();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->autoscroll();
  }
}

void LcdKeypad::noAutoscroll()
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->noAutoscroll();
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->noAutoscroll();
  }
}

void LcdKeypad::createChar(uint8_t location, uint8_t charmap[])
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->createChar(location, charmap);
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->createChar(location, charmap);
  }
}

void LcdKeypad::setCursor(uint8_t col, uint8_t row)
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->setCursor(col, row);
  }
  else if (0 != m_liquidCrystal)
  {
    m_liquidCrystal->setCursor(col, row);
  }
}

size_t LcdKeypad::write(uint8_t value)
{
#if defined(ARDUINO) && (ARDUINO >= 100) //scl
#endif

  size_t ret = 0;
  if (0 != m_liquidTwi2)
  {
    ret = m_liquidTwi2->write(value);
  }
  else if (0 != m_liquidCrystal)
  {
    ret = m_liquidCrystal->write(value);
  }
  return ret;
}

void LcdKeypad::command(uint8_t value)
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->command(value);
  }
  else if (0 != m_liquidCrystal)
  {
    //m_liquidCrystal->command(value);
  }
}

void LcdKeypad::setMCPType(MCPType mcptype)
{
  if (0 != m_liquidTwi2)
  {
    m_liquidTwi2->setMCPType(mcptype);
  }
}

uint8_t LcdKeypad::readButtons()
{
  uint8_t buttons = 0;
  if (0 != m_liquidTwi2)
  {
    buttons = m_liquidTwi2->readButtons();
  }
  return buttons;
}

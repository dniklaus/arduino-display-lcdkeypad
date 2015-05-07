/*
 * LcdKeypad.cpp
 *
 *  Created on: 15.10.2013
 *      Author: niklausd
 */

#include "Arduino.h"
#include "Timer.h"
#include "LiquidCrystal.h"
#include "LcdKeypad.h"


//=========================================================

class KeyPollTimerAdapter : public TimerAdapter
{
public:
  KeyPollTimerAdapter(LcdKeypad* lcdKeypad)
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

const int  LcdKeypad::s_defaultLcdRSPin = 8;
const int  LcdKeypad::s_defaultLcdEnPin = 9;
const int  LcdKeypad::s_defaultLcdD4Pin = 4;
const int  LcdKeypad::s_defaultLcdD5Pin = 5;
const int  LcdKeypad::s_defaultLcdD6Pin = 6;
const int  LcdKeypad::s_defaultLcdD7Pin = 7;

const int  LcdKeypad::s_defaultLcdBackLightCtrlPin = 10;
const bool LcdKeypad::s_defaultIsLcdBackLightOn    = false;

const int  LcdKeypad::s_defaultKeyAdcPin   = A0;
const int  LcdKeypad::s_defaultKeyPollTime = 50;

const int  LcdKeypad::s_rightKeyLimit  =  65;
const int  LcdKeypad::s_upKeyLimit     = 221;
const int  LcdKeypad::s_downKeyLimit   = 395;
const int  LcdKeypad::s_leftKeyLimit   = 602;
const int  LcdKeypad::s_selectKeyLimit = 873;

LcdKeypad::LcdKeypad(int lcdRSPin, int lcdEnPin,
                     int lcdD4Pin, int lcdD5Pin, int lcdD6Pin, int lcdD7Pin,
                     int lcdBackLightCtrlPin, bool isLcdBackLightOn)
: LiquidCrystal(lcdRSPin, lcdEnPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin)
, m_lcdBackLightCtrlPin(lcdBackLightCtrlPin)
, m_isLcdBackLightOn(isLcdBackLightOn)
, m_currentKey(NO_KEY)
, m_keyPollTimer(new Timer(new KeyPollTimerAdapter(this), Timer::IS_RECURRING, s_defaultKeyPollTime))
, m_adapter(0)
{
  setBackLightControl();

  // button adc input
  pinMode(s_defaultKeyAdcPin, INPUT);         //ensure Key ADC pin is an input
  digitalWrite(s_defaultKeyAdcPin, LOW);      //ensure pullup is off on Key ADC pin

  // set up the LCD's number of columns and rows:
  begin(16, 2);
}

LcdKeypad::~LcdKeypad()
{
  delete m_keyPollTimer->adapter();
  delete m_keyPollTimer; m_keyPollTimer = 0;
}

void LcdKeypad::attachAdapter(LcdKeypadAdapter* adapter)
{
  m_adapter = adapter;
}

LcdKeypadAdapter* LcdKeypad::adapter()
{
  return m_adapter;
}

void LcdKeypad::setBackLightOn(bool isLcdBackLightOn)
{
  m_isLcdBackLightOn = isLcdBackLightOn;
  setBackLightControl();
}

void LcdKeypad::setBackLightControl()
{
  if (m_isLcdBackLightOn)
  {
    pinMode(m_lcdBackLightCtrlPin, INPUT);
    digitalWrite(m_lcdBackLightCtrlPin, LOW);
  }
  else
  {
    pinMode(m_lcdBackLightCtrlPin, OUTPUT);
    digitalWrite(m_lcdBackLightCtrlPin, LOW);
  }
}

void LcdKeypad::handleButtons()
{
  int keyPress = analogRead(s_defaultKeyAdcPin);

  Key polledKey = NO_KEY;

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
  return (LcdKeypad::NO_KEY == m_currentKey);
}

bool LcdKeypad::isUpKey()
{
  return (LcdKeypad::UP_KEY == m_currentKey);
}

bool LcdKeypad::isDownKey()
{
  return (LcdKeypad::DOWN_KEY == m_currentKey);
}

bool LcdKeypad::isSelectKey()
{
  return (LcdKeypad::SELECT_KEY == m_currentKey);
}

bool LcdKeypad::isLeftKey()
{
  return (LcdKeypad::LEFT_KEY == m_currentKey);
}

bool LcdKeypad::isRightKey()
{
  return (LcdKeypad::RIGHT_KEY == m_currentKey);
}

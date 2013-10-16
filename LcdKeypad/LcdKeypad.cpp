/*
 * LcdKeypad.cpp
 *
 *  Created on: 15.10.2013
 *      Author: niklausd
 */

#include "Arduino.h"
#include "Timer.h"
#include "TimerAdapter.h"
#include "Debounce.h"
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

const int LcdKeypad::s_defaultLcdRSPin = 8;
const int LcdKeypad::s_defaultLcdEnPin = 9;
const int LcdKeypad::s_defaultLcdD4Pin = 4;
const int LcdKeypad::s_defaultLcdD5Pin = 5;
const int LcdKeypad::s_defaultLcdD6Pin = 6;
const int LcdKeypad::s_defaultLcdD7Pin = 7;

const int LcdKeypad::s_defaultLcdBackLightPWMPin    =  10;
const int LcdKeypad::s_defaultLcdBackLightIntensity = 100;

const int LcdKeypad::s_defaultKeyAdcPin   = A0;
const int LcdKeypad::s_defaultKeyPollTime = 50;

const int LcdKeypad::s_rightKeyLimit  =  65;
const int LcdKeypad::s_upKeyLimit     = 221;
const int LcdKeypad::s_downKeyLimit   = 395;
const int LcdKeypad::s_leftKeyLimit   = 602;
const int LcdKeypad::s_selectKeyLimit = 873;

LcdKeypad::LcdKeypad(int lcdRSPin, int lcdEnPin,
                     int lcdD4Pin, int lcdD5Pin, int lcdD6Pin, int lcdD7Pin,
                     int lcdBackLightPWMPin,     int lcdBackLightIntensity)
: LiquidCrystal(lcdRSPin, lcdEnPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin)
, m_lcdBackLightPWMPin(lcdBackLightPWMPin)
, m_lcdBackLightIntensity(lcdBackLightIntensity)
, m_currentKey(NO_KEY)
, m_keyDebouncer(new Debounce())
, m_keyPollTimer(new Timer(new KeyPollTimerAdapter(this), true, s_defaultKeyPollTime))
{
  pinMode(m_lcdBackLightPWMPin, OUTPUT);
  setBackLightIntensity();

  // button adc input
  pinMode(s_defaultKeyAdcPin, INPUT);         //ensure Key ADC pin is an input
  digitalWrite(s_defaultKeyAdcPin, LOW);      //ensure pullup is off on Key ADC pin

  // set up the LCD's number of columns and rows:
  begin(16, 2);
}

LcdKeypad::~LcdKeypad()
{
  delete m_keyPollTimer; m_keyPollTimer = 0;
  delete m_keyDebouncer; m_keyDebouncer = 0;
}

void LcdKeypad::setBackLightIntensity(int lcdBackLightIntensity)
{
  m_lcdBackLightIntensity = lcdBackLightIntensity;
  setBackLightIntensity();
}

void LcdKeypad::setBackLightIntensity()
{
  analogWrite(m_lcdBackLightPWMPin, m_lcdBackLightIntensity);
}

void LcdKeypad::handleButtons()
{
  if (m_keyDebouncer->isEventAbleToPass())
  {
    int keyPress = analogRead(s_defaultKeyAdcPin);

    if (keyPress < s_rightKeyLimit)
    {
      m_currentKey = RIGHT_KEY;
    }
    else if (keyPress < s_upKeyLimit)
    {
      m_currentKey = UP_KEY;
    }
    else if (keyPress < s_downKeyLimit)
    {
      m_currentKey = DOWN_KEY;
    }
    else if (keyPress < s_leftKeyLimit)
    {
      m_currentKey = LEFT_KEY;
    }
    else if (keyPress < s_selectKeyLimit)
    {
      m_currentKey = SELECT_KEY;
    }
    else
    {
      m_currentKey = NO_KEY;
    }
  }
}

int LcdKeypad::getCurrentKey()
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

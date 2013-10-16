/*
 * LcdKeypad.h
 *
 *  Created on: 15.10.2013
 *      Author: niklausd
 */

#ifndef LCDKEYPAD_H_
#define LCDKEYPAD_H_

#include "LiquidCrystal.h"

class LcdKeypad: public LiquidCrystal
{
public:
  typedef enum
  {
      NO_KEY     = 0
    , SELECT_KEY = 1
    , LEFT_KEY   = 2
    , UP_KEY     = 3
    , DOWN_KEY   = 4
    , RIGHT_KEY  = 5
  } key;

  LcdKeypad(int lcdRSPin              = s_defaultLcdRSPin,
            int lcdEnPin              = s_defaultLcdEnPin,
            int lcdD4Pin              = s_defaultLcdD4Pin,
            int lcdD5Pin              = s_defaultLcdD5Pin,
            int lcdD6Pin              = s_defaultLcdD6Pin,
            int lcdD7Pin              = s_defaultLcdD7Pin,
            int lcdBackLightPWMPin    = s_defaultLcdBackLightPWMPin,
            int lcdBackLightIntensity = s_defaultLcdBackLightIntensity);
  virtual ~LcdKeypad();

  void setBackLightIntensity(int lcdBackLightIntensity);

  void handleButtons();

  int getCurrentKey();

  bool isNoKey();
  bool isUpKey();
  bool isDownKey();
  bool isSelectKey();
  bool isLeftKey();
  bool isRightKey();

private:
  void setBackLightIntensity();

private:
  int m_lcdBackLightPWMPin;
  int m_lcdBackLightIntensity;
  int m_currentKey;
  class Debounce* m_keyDebouncer;
  class Timer* m_keyPollTimer;

private:
  static const int s_defaultLcdRSPin;
  static const int s_defaultLcdEnPin;
  static const int s_defaultLcdD4Pin;
  static const int s_defaultLcdD5Pin;
  static const int s_defaultLcdD6Pin;
  static const int s_defaultLcdD7Pin;
  static const int s_defaultLcdBackLightPWMPin;
  static const int s_defaultLcdBackLightIntensity;

  static const int s_defaultKeyAdcPin;
  static const int s_defaultKeyPollTime;
  static const int s_rightKeyLimit;
  static const int s_upKeyLimit;
  static const int s_downKeyLimit;
  static const int s_leftKeyLimit;
  static const int s_selectKeyLimit;
};

#endif /* LCDKEYPAD_H_ */

/*
 * LcdKeypad.h
 *
 *  Created on: 15.10.2013
 *      Author: niklausd
 */

#ifndef LCDKEYPAD_H_
#define LCDKEYPAD_H_

#include "LiquidCrystal.h"

class LcdKeypad : public LiquidCrystal
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

  LcdKeypad(int  lcdRSPin              = s_defaultLcdRSPin,
            int  lcdEnPin              = s_defaultLcdEnPin,
            int  lcdD4Pin              = s_defaultLcdD4Pin,
            int  lcdD5Pin              = s_defaultLcdD5Pin,
            int  lcdD6Pin              = s_defaultLcdD6Pin,
            int  lcdD7Pin              = s_defaultLcdD7Pin,
            int  lcdBackLightCtrlPin   = s_defaultLcdBackLightCtrlPin,
            bool isLcdBackLightOn      = s_defaultIsLcdBackLightOn);
  virtual ~LcdKeypad();

  void setBackLightOn(bool isLcdBackLightOn);

  void handleButtons();

  int getCurrentKey();

  bool isNoKey();
  bool isUpKey();
  bool isDownKey();
  bool isSelectKey();
  bool isLeftKey();
  bool isRightKey();

private:
  void setBackLightControl();

private:
  int m_lcdBackLightCtrlPin;
  bool m_isLcdBackLightOn;
  int m_currentKey;
  class Debounce* m_keyDebouncer;
  class Timer* m_keyPollTimer;

private:
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
  LcdKeypad& operator = (const LcdKeypad& );  // assignment operator
  LcdKeypad(const LcdKeypad& src);            // copy constructor

};

#endif /* LCDKEYPAD_H_ */

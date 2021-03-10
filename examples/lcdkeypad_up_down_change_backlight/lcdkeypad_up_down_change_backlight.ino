// SpinTimer library, https://github.com/dniklaus/spin-timer, 
//   add it by using the Arduino IDE Library Manager (search for spin-timer)
#include <SpinTimer.h>

// LcdKeypad, https://github.com/dniklaus/arduino-display-lcdkeypad, 
//   add it by using the Arduino IDE Library Manager (search for arduino-display-lcdkeypad)
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
      m_lcdKeypad->setCursor(0, 1);            // position the cursor at beginning of the second line
      m_lcdKeypad->print(m_value);             // print the value on the second line of the display
      m_lcdKeypad->print("                ");  // wipe out characters behind the printed value
     
      // RGB colored backlight: set according to the current value
      // monochrome backlight: set backlight on or off according to the current value
      m_lcdKeypad->setBacklight(static_cast<LcdKeypad::LcdBacklightColor>(LcdKeypad::LCDBL_WHITE & m_value));
    }
  }
};
    
void setup()
{
  myLcdKeypad = new LcdKeypad();  // instantiate an object of the LcdKeypad class, using default parameters
  
  // Attach the specific LcdKeypadAdapter implementation (dependency injection)
  myLcdKeypad->attachAdapter(new MyLcdKeypadAdapter(myLcdKeypad));
  
  myLcdKeypad->setCursor(0, 0);   // position the cursor at beginning of the first line
  myLcdKeypad->print("Value:");   // print a Value label on the first line of the display
}
    
void loop()
{
  scheduleTimers();  // Get the timer(s) ticked, in particular the LcdKeypad dirver's keyPollTimer
}

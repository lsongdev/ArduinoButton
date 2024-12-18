#include "KButton.h"

KButton::KButton(const int pin, const bool activeLow, const bool pullupActive) {
  _pin = pin;

  if (activeLow) {
    // the button connects the input pin to GND when pressed.
    _buttonPressed = LOW;

  } else {
    // the button connects the input pin to VCC when pressed.
    _buttonPressed = HIGH;
  }

  if (pullupActive) {
    // use the given pin as input and activate internal PULLUP resistor.
    pinMode(pin, INPUT_PULLUP);
  } else {
    // use the given pin as input
    pinMode(pin, INPUT);
  }
}  // OneButton


// explicitly set the number of millisec that have to pass by before a click is assumed stable.
void KButton::setDebounceMs(const unsigned int ms) {
  _debounce_ms = ms;
}  // setDebounceMs


// explicitly set the number of millisec that have to pass by before a click is detected.
void KButton::setClickMs(const unsigned int ms) {
  _click_ms = ms;
}  // setClickMs


// explicitly set the number of millisec that have to pass by before a long button press is detected.
void KButton::setPressMs(const unsigned int ms) {
  _press_ms = ms;
}  // setPressMs


// save function for click event
void KButton::attachClick(callbackFunction newFunction) {
  _clickFunc = newFunction;
}  // attachClick


// save function for doubleClick event
void KButton::attachDoubleClick(callbackFunction newFunction) {
  _doubleClickFunc = newFunction;
}  // attachDoubleClick


// save function for longPressStart event
void KButton::attachLongPressStart(callbackFunction newFunction) {
  _longPressStartFunc = newFunction;
}  // attachLongPressStart


void KButton::reset(void) {
  _state = KButton::OCS_INIT;
  _nClicks = 0;
  _startTime = 0;
}


/**
 * @brief Debounce input pin level for use in SpecialInput.
 */
int KButton::debounce(const int value) {
  now = millis();  // current (relative) time in msecs.
  if (_lastDebouncePinLevel == value) {
    if (now - _lastDebounceTime >= _debounce_ms)
      debouncedPinLevel = value;
  } else {
    _lastDebounceTime = now;
    _lastDebouncePinLevel = value;
  }
  return debouncedPinLevel;
};


/**
 * @brief Check input of the configured pin,
 * debounce input pin level and then
 * advance the finite state machine (FSM).
 */
void KButton::tick(void) {
  if (_pin >= 0) {
    _fsm(debounce(digitalRead(_pin)) == _buttonPressed);
  }
}  // tick()


void KButton::tick(bool activeLevel) {
  _fsm(debounce(activeLevel));
}


/**
 *  @brief Advance to a new state and save the last one to come back in cas of bouncing detection.
 */
void KButton::_newState(stateMachine_t nextState) {
  _state = nextState;
}  // _newState()


/**
 * @brief Run the finite state machine (FSM) using the given level.
 */
void KButton::_fsm(bool activeLevel) {
  unsigned long waitTime = (now - _startTime);

  // Implementation of the state machine
  switch (_state) {
    case KButton::OCS_INIT:
      // waiting for level to become active.
      if (activeLevel) {
        _newState(KButton::OCS_DOWN);
        _startTime = now;  // remember starting time
        _nClicks = 0;
      }  // if
      break;

    case KButton::OCS_DOWN:
      // waiting for level to become inactive.

      if (!activeLevel) {
        _newState(KButton::OCS_UP);
        _startTime = now;  // remember starting time

      } else if ((activeLevel) && (waitTime > _press_ms)) {
        if (_longPressStartFunc) _longPressStartFunc();
        _newState(KButton::OCS_PRESS);
      }  // if
      break;

    case KButton::OCS_UP:
      // level is inactive

      // count as a short button down
      _nClicks++;
      _newState(KButton::OCS_COUNT);
      break;

    case KButton::OCS_COUNT:
      // dobounce time is over, count clicks

      if (activeLevel) {
        // button is down again
        _newState(KButton::OCS_DOWN);
        _startTime = now;  // remember starting time

      } else if ((waitTime >= _click_ms) || (_nClicks == 2)) {
        // now we know how many clicks have been made.

        if (_nClicks == 1) {
          // this was 1 click only.
          if (_clickFunc) _clickFunc();

        } else if (_nClicks == 2) {
          // this was a 2 click sequence.
          if (_doubleClickFunc) _doubleClickFunc();

        }  // if

        reset();
      }  // if
      break;

    case KButton::OCS_PRESS:
      // waiting for pin being release after long press.

      if (!activeLevel) {
        _newState(KButton::OCS_PRESSEND);
        _startTime = now;
      }  // if
      break;

    case KButton::OCS_PRESSEND:
      // button was released.
      reset();
      break;

    default:
      // unknown state detected -> reset state machine
      _newState(KButton::OCS_INIT);
      break;
  }  // if

}  // KButton.tick()


// end.

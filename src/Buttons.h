#ifndef __BUTTONS_H
#define __BUTTONS_H

//#define ONLY_COMPLEX_STATES // Only CLICK, LONGCLICK and DBLCLICK states processing
//#define ONE_BUTTON

#ifndef ONE_BUTTON
#include "List.h"
#endif
#include "Events.h"

struct __packed _button_t {
#ifdef ESP32
  uint8_t pin;
#else
  uint8_t pin : 4;
#endif
  bool level : 1;
  bool paused : 1;
  bool pressed : 1;
  bool dblclickable : 1;
  volatile uint16_t duration;
};

const uint8_t EVT_BTNBASE = 0;

enum buttonstate_t : uint8_t { BTN_RELEASED, BTN_PRESSED, BTN_CLICK, BTN_LONGCLICK, BTN_DBLCLICK };

enum btneventid_t : uint8_t { EVT_BTNRELEASED = EVT_BTNBASE + BTN_RELEASED, EVT_BTNPRESSED = EVT_BTNBASE + BTN_PRESSED,
  EVT_BTNCLICK = EVT_BTNBASE + BTN_CLICK, EVT_BTNLONGCLICK = EVT_BTNBASE + BTN_LONGCLICK, EVT_BTNDBLCLICK = EVT_BTNBASE + BTN_DBLCLICK };

#ifdef ONE_BUTTON
class Button {
public:
  Button(uint8_t pin, bool level, const EventQueue *events = NULL);

  void pause();
  void resume();

protected:
  static const uint16_t CLICK_TIME = 20; // 20 ms. debounce time
  static const uint16_t LONGCLICK_TIME = 1000; // 1 sec.
  static const uint16_t DBLCLICK_TIME = 500; // 0.5 sec.

  static void _isr(Button *_this);
  virtual void onChange(buttonstate_t state);

  _button_t _item;
  uint32_t _isrtime;
  EventQueue *_events;
};
#else

#ifdef ESP32
#define MAX_BUTTONS 24
#else
#define MAX_BUTTONS 10
#endif

class Buttons : public List<_button_t, MAX_BUTTONS> {
public:
  Buttons(const EventQueue *events = NULL) : List<_button_t, MAX_BUTTONS>(), _isrtime(0), _events((EventQueue*)events) {}

  uint8_t add(uint8_t pin, bool level);
  void pause(uint8_t index);
  void resume(uint8_t index);

protected:
  static const uint16_t CLICK_TIME = 20; // 20 ms. debounce time
  static const uint16_t LONGCLICK_TIME = 1000; // 1 sec.
  static const uint16_t DBLCLICK_TIME = 500; // 0.5 sec.

  void cleanup(void *ptr);
  bool match(uint8_t index, const void *t);

  static void _isr(Buttons *_this);
  virtual void onChange(buttonstate_t state, uint8_t button);

  uint32_t _isrtime;
  EventQueue *_events;
};
#endif

#endif

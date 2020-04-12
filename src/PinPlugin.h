#ifndef pinplugin_h
#define pinplugin_h

#include <Arduino.h>
#include <StateMachine.h>

#define PIN_DIGITAL 1
#define PIN_ANALOG 2

class PinPlugin : public Plugin
{
public:
    PinPlugin(const char *, uint8_t, uint8_t direction = INPUT, uint8_t mode = PIN_DIGITAL);
    void initialize(StateMachineController *);

    uint8_t pin;
    uint8_t mode;
    uint8_t direction;

    void read();
    void write();

#if defined(ARDUINO_ARCH_ESP32)
    uint8_t pwmChannel;
#endif
};

#endif
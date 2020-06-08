#include <Arduino.h>
#include "PinPlugin.h"

#if defined(ARDUINO_ARCH_ESP32)
uint8_t _esp32pwmChannelCounter = 0;
#endif

void pinReadAction(Plugin *plugin)
{
    ((PinPlugin *)plugin)->read();
}

void pinWriteAction(Plugin *plugin)
{
    ((PinPlugin *)plugin)->write();
}

PinPlugin::PinPlugin(const char *instanceId, uint8_t pin, uint8_t direction, uint8_t mode) : Plugin(instanceId)
{
    this->pin = pin;
    this->mode = mode;
    this->direction = direction;

    registerAction("read", pinReadAction);
    registerAction("write", pinWriteAction);
}

void PinPlugin::initialize(StateMachineController *sm)
{
    Plugin::initialize(sm);

    if (direction == OUTPUT)
    {
#if defined(ARDUINO_ARCH_ESP32)
        if (mode == PIN_ANALOG)
        {
            pwmChannel = _esp32pwmChannelCounter++;
            ledcAttachPin(pin, pwmChannel);
            ledcSetup(pwmChannel, 5000, 8);
        }
        else
        {
            pinMode(pin, OUTPUT);
        }
#else
        pinMode(pin, OUTPUT);
#endif
    }
    else
    {
        pinMode(pin, direction);
    }
}

void PinPlugin::read()
{
    int value = mode == PIN_DIGITAL ? digitalRead(pin) : analogRead(pin);
    int oldValue = getVarInt("value", -1);
    if (value != oldValue)
        setVar("value", value);
}

void PinPlugin::write()
{
    long int value = actionContext->getParamInt(0);

    if (mode == PIN_DIGITAL)
    {
        digitalWrite(pin, value ? HIGH : LOW);
    }
    else
    {
#if defined(ARDUINO_ARCH_ESP32)
        ledcWrite(pwmChannel, value);
#else
        analogWrite(pin, value);
#endif
    }
}
#include "Arduino.h"
#include "InterruptPlugin.h"

#define MAX_INTERRUPTS 8

// Tracking of Interrupt plugin instances
int _registeredInterrupts = 0;

// Pointers to StateMachine Store variables
uint8_t _slotMap[MAX_INTERRUPTS];
unsigned long _interruptFilters[MAX_INTERRUPTS];
unsigned long _interruptTimeouts[MAX_INTERRUPTS];

VarStruct *_isPressed[MAX_INTERRUPTS] = {nullptr};
volatile bool _wasClicked[MAX_INTERRUPTS] = {false};

#ifdef ESP32
void IRAM_ATTR _pinChange(int num)
#else
void ICACHE_RAM_ATTR _pinChange(int num)
#endif
{
    if (digitalRead(_slotMap[num]))
    {
        if ((micros() - _interruptTimeouts[num]) > _interruptFilters[num])
            _wasClicked[num] = true;
        _isPressed[num]->vInt = 1;
        _isPressed[num]->vFloat = 1.0f;
        _interruptTimeouts[num] = micros();
    }
    else
    {
        _isPressed[num]->vInt = 0;
        _isPressed[num]->vFloat = 0.0f;
    }
}

#ifdef ESP32
#define INT_FN(num) \
    void IRAM_ATTR pinChange##num() { _pinChange(num); }
#else
#define INT_FN(num) \
    void ICACHE_RAM_ATTR pinChange##num() { _pinChange(num); }
#endif

#define ATTACH_FN(pin, fn_num) \
    attachInterrupt(pin, pinChange##fn_num, CHANGE);

// Unique interrupt handler functions

INT_FN(0);
INT_FN(1);
INT_FN(2);
INT_FN(3);
INT_FN(4);
INT_FN(5);
INT_FN(6);
INT_FN(7);

// Attach interrupt action callback

void interruptAttachAction(Plugin *plugin)
{
    ((InterruptPlugin *)plugin)->attach();
}

// Reset clicked flag callback

void interruptReadAction(Plugin *plugin)
{
    ((InterruptPlugin *)plugin)->read();
}

InterruptPlugin::InterruptPlugin(const char *instanceId, uint8_t pin, unsigned long filter)
    : Plugin(instanceId)
{
    this->pin = pin;
    slotNum = _registeredInterrupts++;
    if (slotNum < MAX_INTERRUPTS)
    {
        registerAction("attach", interruptAttachAction);
        registerAction("read", interruptReadAction);
        _interruptFilters[slotNum] = filter;
        _interruptTimeouts[slotNum] = micros();
    }
}

void InterruptPlugin::read()
{
    if (slotNum >= MAX_INTERRUPTS)
        return;

    int oldValue = getVarInt("click", -1);
    int value = _wasClicked[slotNum] ? 1 : 0;
    if (value != oldValue)
        setVar("click", value);
    _wasClicked[slotNum] = false;
}

void InterruptPlugin::attach()
{
    if (slotNum >= MAX_INTERRUPTS)
        return;

    _slotMap[slotNum] = pin;

    // initialize variables (allocate memory for them)
    setVar("click", 0);
    setVar("state", 0);

    // Store pointer to a raw variable value.
    // Variable itself is already created in constructor

    _isPressed[slotNum] = getVarRaw("state");

    // Attach interrupt listener.
    // Listener will update variable directly by writing to memory address

    pinMode(pin, INPUT);
    int mappedPin = digitalPinToInterrupt(pin);
    switch (slotNum)
    {
    case 0:
        ATTACH_FN(mappedPin, 0);
        return;
    case 1:
        ATTACH_FN(mappedPin, 1);
        return;
    case 2:
        ATTACH_FN(mappedPin, 2);
        return;
    case 3:
        ATTACH_FN(mappedPin, 3);
        return;
    case 4:
        ATTACH_FN(mappedPin, 4);
        return;
    case 5:
        ATTACH_FN(mappedPin, 5);
        return;
    case 6:
        ATTACH_FN(mappedPin, 6);
        return;
    case 7:
        ATTACH_FN(mappedPin, 7);
        return;
    }
}

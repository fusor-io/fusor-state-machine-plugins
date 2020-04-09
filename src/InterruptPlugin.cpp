#include "Arduino.h"
#include "InterruptPlugin.h"

#define MAX_INTERRUPTS 8

// Tracking of Interrupt plugin instances
int registeredInterrupts = 0;

// Pointers to StateMachine Store variables
VarStruct *isPressed[MAX_INTERRUPTS] = {nullptr};
volatile bool wasClicked[MAX_INTERRUPTS] = {false};

#define INT_FN(num)                    \
    void IRAM_ATTR pinFall##num()      \
    {                                  \
        isPressed[num]->vInt = 1;      \
        isPressed[num]->vFloat = 1.0f; \
        wasClicked[num] = true;        \
    }                                  \
    void IRAM_ATTR pinRise##num()      \
    {                                  \
        isPressed[num]->vInt = 0;      \
        isPressed[num]->vFloat = 0.0f; \
    }
#define ATTACH_FN(pin, fn_num)                     \
    attachInterrupt(pin, pinRise##fn_num, RISING); \
    attachInterrupt(pin, pinFall##fn_num, FALLING);

// Unique interrupt handler functions

void IRAM_ATTR pinFall0()
{
    VarStruct *p = isPressed[0];
    if (p == nullptr)
    {
        p->vInt = 1;
        p->vFloat = 1.0f;
        wasClicked[0] = true;
    }
    else
    {
        p->vInt = 2;
        p->vFloat = 2.0f;
        wasClicked[0] = true;
    }
}
void IRAM_ATTR pinRise0()
{
    VarStruct *p = isPressed[0];
    p->vInt = 0;
    p->vFloat = 0.0f;
}

// INT_FN(0);
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

InterruptPlugin::InterruptPlugin(const char *instanceId, uint8_t pin)
    : Plugin(instanceId)
{
    this->pin = pin;
    slotNum = registeredInterrupts++;
    if (slotNum < MAX_INTERRUPTS)
    {
        registerAction("attach", interruptAttachAction);
        registerAction("read", interruptReadAction);
    }
}

void InterruptPlugin::read()
{
    if (slotNum >= MAX_INTERRUPTS)
        return;

    setVar("click", wasClicked[slotNum] ? 1 : 0);
    wasClicked[slotNum] = false;
}

void InterruptPlugin::attach()
{
    if (slotNum >= MAX_INTERRUPTS)
        return;

    // initialize variables (allocate memory for them)
    setVar("click", 0);
    setVar("state", 0);

    // Store pointer to a raw variable value.
    // Variable itself is already created in constructor

    isPressed[slotNum] = getVarRaw("state");

    // Attach interrupt listener.
    // Listener will update variable directly by writing to memory address

    pinMode(pin, INPUT_PULLUP);
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

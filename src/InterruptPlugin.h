#ifndef interruptplugin_h
#define interruptplugin_h

#include <StateMachine.h>

typedef void (*InterruptCallback)();

class InterruptPlugin : public Plugin
{
public:
    InterruptPlugin(const char *, uint8_t);

    uint8_t pin;
    int slotNum;

    void attach();
    void read();
};

#endif
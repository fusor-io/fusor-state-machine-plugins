#ifndef interruptplugin_h
#define interruptplugin_h

#include <StateMachine.h>

#define DEFAULT_FILTER_1000US 10000

typedef void (*InterruptCallback)();

class InterruptPlugin : public Plugin
{
public:
    InterruptPlugin(const char *, uint8_t, unsigned long filter = DEFAULT_FILTER_1000US);

    uint8_t pin;
    int slotNum;

    void attach();
    void read();
};

#endif
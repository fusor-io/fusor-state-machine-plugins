#ifndef syslugin_h
#define syslugin_h

#include <Arduino.h>
#include <StateMachine.h>

class SysPlugin : public Plugin
{
public:
    SysPlugin();
    void readMemory();
    void readVcc();
};

#endif
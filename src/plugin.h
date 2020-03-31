#ifndef plugin_h
#define plugin_h

#include <StateMachine.h>

typedef struct PluginAction
{
    const char *name;
    ActionFunction action;
} PLUGIN_ACTION;

class Plugin
{
public:
    Plugin(const char *, StateMachineController *);

    void registerAction(const char *, ActionFunction);

protected:
    const char *_id;
    StateMachineController *_sm;
};

#endif
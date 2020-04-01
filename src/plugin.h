#ifndef plugin_h
#define plugin_h

#include <map>
#include <StateMachine.h>
#include <keycompare.h>

class Plugin;
typedef void (*PluginFunction)(Plugin *);

class Plugin
{
public:
    Plugin(const char *, StateMachineController *);

    StateMachineController *sm;
    const char *id;
    static std::map<const char *, PluginFunction, KeyCompare> actionMap;

    void registerAction(const char *, PluginFunction);
    void setVar(const char *, int);
    void setVar(const char *, float);

private:
    char *withNameSpace(const char *);
};

#endif

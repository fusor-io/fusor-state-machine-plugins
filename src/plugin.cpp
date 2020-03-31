#include <string.h>
#include <StateMachine.h>
#include "plugin.h"

Plugin::Plugin(const char *id, StateMachineController *sm)
{
    _id = id;
    _sm = sm;
}

void Plugin::registerAction(const char *name, ActionFunction action)
{
    char *buff = new char[strlen(name) + strlen(_id) + 2];
    strcpy(buff, _id);
    strcat(buff, ".");
    strcat(buff, name);
    _sm->registerAction((const char *)buff, action);
}
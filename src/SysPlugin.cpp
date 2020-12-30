#include <Arduino.h>
#include "SysPlugin.h"

void readMemoryAction(Plugin *plugin)
{
    ((SysPlugin *)plugin)->readMemory();
}

void readVccAction(Plugin *plugin)
{
    ((SysPlugin *)plugin)->readVcc();
}

void readAll(Plugin *plugin)
{
    ((SysPlugin *)plugin)->readMemory();
    ((SysPlugin *)plugin)->readVcc();
}

SysPlugin::SysPlugin() : Plugin("@sys")
{
    registerAction("read_all", readAll);
    registerAction("read_mem", readMemoryAction);
    registerAction("read_vcc", readVccAction);
}

void SysPlugin::readMemory()
{
#if defined(ARDUINO_ARCH_ESP32)
    setVar("mem", (long int)ESP.getFreeHeap());
#endif
}

void SysPlugin::readVcc()
{
#if defined(ESP8266)
    setVar("vcc", (float)ESP.getVcc() / 1000.0f);
#endif
}

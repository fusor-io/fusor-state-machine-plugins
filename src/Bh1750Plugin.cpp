#include "Bh1750Plugin.h"

/**
 * Light sensor BH1750 plugin, based on hp_BH1750 library by Stefan Armborst
 * @see https://github.com/Starmbi/hp_BH1750
 **/

void bh1750ReadAction(Plugin *plugin)
{
    ((Bh1750Plugin *)plugin)->read();
}

Bh1750Plugin::Bh1750Plugin(const char *instanceId, uint8_t address, TwoWire *i2c, uint8_t filter) : Plugin(instanceId), sensor()
{
    this->address = address;
    this->i2c = i2c;
    this->filter = (float)filter;
    registerAction("read", bh1750ReadAction);
}

void Bh1750Plugin::initialize(StateMachineController *sm)
{
    Plugin::initialize(sm);
    isConnected = sensor.begin(address, i2c);
    if (isConnected)
    {
        sensor.start();
        setVar("lux", 0.0f);
        value = 0.0f;
    }
    else
    {
        setVar("lux", -1.0f);
    }
}

void Bh1750Plugin::read()
{
    if (isConnected && sensor.hasValue())
    {
        value = ((value * filter) + sensor.getLux()) / (filter + 1.0f);
        setVar("lux", value);
        sensor.start();
    }
}
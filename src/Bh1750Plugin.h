#ifndef bh1750plugin_h
#define bh1750plugin_h

#include <Arduino.h>
#include <hp_BH1750.h>
#include <StateMachine.h>

// ADDR pin pulldown
#define BH1750_ADDRESS1 0x23
// ADDR pin pullup
#define BH1750_ADDRESS2 0x5C

class Bh1750Plugin : public Plugin
{
public:
    Bh1750Plugin(const char *, uint8_t address = BH1750_ADDRESS1, TwoWire *i2c = &Wire, uint8_t filter = 1);
    void initialize(StateMachineController *);
    void read();

    hp_BH1750 sensor;
    bool isConnected = false;

    uint8_t address;
    TwoWire *i2c;
    float filter;

    float value;
};

#endif
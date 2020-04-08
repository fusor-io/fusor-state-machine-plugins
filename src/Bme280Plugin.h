#ifndef bme280I2C_h
#define bme280I2C_h

#include <BME280.h>
#include <BME280I2C.h>

#include <StateMachine.h>

class Bme280Plugin : public Plugin
{
public:
    Bme280Plugin(const char *, StateMachineController *, bool useFirstPort = true);

    BME280I2C sensor;
    bool isConnected = false;

    bool init();
    void read();
};

#endif
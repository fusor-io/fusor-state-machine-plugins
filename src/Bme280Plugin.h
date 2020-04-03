#ifndef bme280I2C_h
#define bme280I2C_h

#include <BME280.h>
#include <BME280I2C.h>

#include <StateMachine.h>

class Bme280Plugin : public Plugin
{
public:
    Bme280Plugin(StateMachineController *, int, int, bool useDefaultPort = true);

    BME280I2C sensor;

private:
    int _sdaPin;
    int _sclPin;
};

#endif
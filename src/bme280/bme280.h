#ifndef bme280I2C_h
#define bme280I2C_h

#include <vector>

#include <BME280.h>
#include <BME280I2C.h>

#include <plugin.h>
#include <StateMachine.h>

class bme280 : public Plugin
{
public:
    bme280(StateMachineController *, int, int, bool useDefaultPort = true);

    BME280I2C sensor;

private:
    int _sdaPin;
    int _sclPin;
};

#endif
#ifndef bme280I2C_h
#define bme280I2C_h

#include <BME280.h>
#include <BME280I2C.h>

#include <StateMachine.h>

class Bme280Plugin : public Plugin
{
public:
    Bme280Plugin(const char *, bool useFirstPort = true, int sdaPin = -1, int sclPin = -1);

    BME280I2C sensor;
    bool isConnected = false;

    bool init();
    void read();

private:
    int _sdaPin;
    int _sclPin;
    bool _begin();
};

#endif
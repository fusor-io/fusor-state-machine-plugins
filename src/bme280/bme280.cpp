#include <StateMachine.h>

#include <BME280.h>
#include <BME280I2C.h>

#include "bme280.h"

/**
 * Plugin for BME280 library by Tyler Glenn 
 */

// I2C settings with 16x oversampling for better accuracy

void init(StateMachineController *sm)
{
}

std::vector<PLUGIN_ACTION> bme280::actions = {{"init", init}};

BME280I2C::Settings settings(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    0x76);

BME280I2C a(settings);

bme280::bme280(StateMachineController *sm, int sdaPin, int sclPin, bool useDefaultPort)
    : Plugin("bme280", sm) //,
//      sensor(settings)
{
    _sdaPin = sdaPin;
    _sclPin = sclPin;
}

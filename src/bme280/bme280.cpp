#include <float.h>
#include <StateMachine.h>

#include <BME280.h>
#include <BME280I2C.h>

#include "bme280.h"

/**
 * Plugin for BME280 library by Tyler Glenn 
 */

void init(Plugin *plugin)
{
    BME280I2C bme = ((bme280 *)plugin)->sensor;
    while (!bme.begin())
    {
        // We are allowed to use delay in setup action
        // Using it in other actions is discouraged,
        // use "elapsed" operator inside state machine instead
        // to avoid blocking other machines
        delay(1000);
    }
}

void read(Plugin *plugin)
{
    BME280I2C bme = ((bme280 *)plugin)->sensor;

    float temp(FLT_MIN), hum(FLT_MIN), pres(FLT_MIN);
    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    bme.read(pres, temp, hum, tempUnit, presUnit);

    plugin->setVar("pres_pa", pres);
    plugin->setVar("temp_c", temp);
    plugin->setVar("hum", hum);
}

// I2C settings with 16x oversampling for better accuracy

BME280I2C::Settings i2c_0x76(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    0x76);

BME280I2C::Settings i2c_0x77(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    0x77);

bme280::bme280(StateMachineController *sm, int sdaPin, int sclPin, bool useDefaultPort)
    : Plugin("bme280", sm), sensor(useDefaultPort ? i2c_0x76 : i2c_0x77)
{
    _sdaPin = sdaPin;
    _sclPin = sclPin;

    registerAction("init", init);
    registerAction("read", read);
}

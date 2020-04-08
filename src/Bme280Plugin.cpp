#include <float.h>
#include <math.h>
#include <StateMachine.h>

#include <BME280.h>
#include <BME280I2C.h>

#include "Bme280Plugin.h"

/**
 * Plugin for "BME280 library by Tyler Glenn"
 * @see https://github.com/finitespace/BME280
 * More details about BME settings
 * @see https://github.com/bolderflight/BME280/blob/master/README.md
 */

/**
 *  I2C settings with:
 *    16x oversampling - for better accuracy
 *    2x filter - for readings smoothing
 *    Normal mode - for continuous data collection by chip
 *    Standby time 250ms - time between readings
 */

BME280I2C::Settings i2c_0x76(
    BME280::OSR_X16,
    BME280::OSR_X16,
    BME280::OSR_X16,
    BME280::Mode_Normal,
    BME280::StandbyTime_250ms,
    BME280::Filter_2,
    BME280::SpiEnable_False,
    0x76);

BME280I2C::Settings i2c_0x77(
    BME280::OSR_X16,
    BME280::OSR_X16,
    BME280::OSR_X16,
    BME280::Mode_Normal,
    BME280::StandbyTime_250ms,
    BME280::Filter_2,
    BME280::SpiEnable_False,
    0x77);

void initAction(Plugin *plugin)
{
    ((Bme280Plugin *)plugin)->init();
}

void readAction(Plugin *plugin)
{
    ((Bme280Plugin *)plugin)->read();
}

Bme280Plugin::Bme280Plugin(const char *sensorId, StateMachineController *sm, bool useFirstPort)
    : Plugin(sensorId, sm), sensor(useFirstPort ? i2c_0x76 : i2c_0x77)
{
    registerAction("init", initAction);
    registerAction("read", readAction);
}

bool Bme280Plugin::init()
{
    isConnected = false;

    for (int i = 0; i < 10; i++)
    {
        if (sensor.begin())
        {
            isConnected = true;
            switch (sensor.chipModel())
            {
            case BME280::ChipModel_BME280:
                setVar("type", 2);
                return true;
            case BME280::ChipModel_BMP280:
                setVar("type", 1);
                return true;
            default:
                setVar("type", 0);
            }
            return true;
        }
        delay(500);
    }

    return false;
}

void Bme280Plugin::read()
{
    if (!isConnected && !init())
        return;

    float temp(FLT_MIN), hum(FLT_MIN), pres(FLT_MIN);
    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    sensor.read(pres, temp, hum, tempUnit, presUnit);

    if (temp == 0.0f && hum == 0.0f && pres == NAN)
    {
        if (!init())
            return;
        sensor.read(pres, temp, hum, tempUnit, presUnit);
    }

    setVar("pres_hpa", pres);
    setVar("temp_c", temp);
    setVar("hum", hum);
}
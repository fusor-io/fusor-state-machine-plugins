#include <float.h>
#include <math.h>
#include <StateMachine.h>

#include <Wire.h>
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
 *  Recommended modes (based on Bosch BME280I2C environmental sensor data sheet)
 *
 *    Weather Monitoring :
 *     forced mode, 1 sample/minute
 *     pressure ×1, temperature ×1, humidity ×1, filter off
 *     Current Consumption =  0.16 μA
 *     RMS Noise = 3.3 Pa/30 cm, 0.07 %RH
 *     Data Output Rate 1/60 Hz
 *
 *    Humidity Sensing :
 *     forced mode, 1 sample/second
 *     pressure ×0, temperature ×1, humidity ×1, filter off
 *     Current Consumption = 2.9 μA
 *     RMS Noise = 0.07 %RH
 *     Data Output Rate =  1 Hz
 *
 *    Indoor Navigation :
 *     normal mode, standby time = 0.5ms
 *     pressure ×16, temperature ×2, humidity ×1, filter = x16
 *     Current Consumption = 633 μA
 *     RMS Noise = 0.2 Pa/1.7 cm
 *     Data Output Rate = 25Hz
 *     Filter Bandwidth = 0.53 Hz
 *     Response Time (75%) = 0.9 s
 */

// We default to blended mode for indoor weather station

BME280I2C::Settings i2c_0x76(
    BME280::OSR_X4,  // temperature
    BME280::OSR_X16, // pressure
    BME280::OSR_X4,  // humidity
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_16,
    BME280::SpiEnable_False,
    0x76);

BME280I2C::Settings i2c_0x77(
    BME280::OSR_X4,  // temperature
    BME280::OSR_X16, // pressure
    BME280::OSR_X4,  // humidity
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_16,
    BME280::SpiEnable_False,
    0x77);

void bme280InitAction(Plugin *plugin)
{
    ((Bme280Plugin *)plugin)->init();
}

void bme280ReadAction(Plugin *plugin)
{
    ((Bme280Plugin *)plugin)->read();
}

Bme280Plugin::Bme280Plugin(const char *sensorId, bool useFirstPort, int sdaPin, int sclPin)
    : Plugin(sensorId), sensor(useFirstPort ? i2c_0x76 : i2c_0x77)
{
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    registerAction("init", bme280InitAction);
    registerAction("read", bme280ReadAction);
}

bool Bme280Plugin::init()
{
    isConnected = false;

    for (int i = 0; i < 10; i++)
    {
        if (_begin())
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

    Serial.println(F("Unable to connect to BME280 sensor"));
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

bool Bme280Plugin::_begin()
{
    if (_sdaPin > -1 && _sclPin > -1)
        Wire.begin(_sdaPin, _sclPin);
    return sensor.begin();
}
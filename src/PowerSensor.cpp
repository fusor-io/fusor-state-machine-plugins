/**
 * Plugin for measuring power consumption based on SCT-013 current sensor
 * If device has corrent local system time, this plugin can accumulate
 * power consumption for peak hours and for off-peak hours
 * 
 * Input variables:
 *      "coef_p" - coefficient used to translate AC current amplitude 
 *                 measured as voltage on analog input into Watts
 *      "peak_start" - peak start time in GMT timezone (local device time)
 *      "peak_end" - peak start time in GMT timezone (local device time)
 *      "total" - initial electric metter readings, kw/h
 *      "peak" - initial peak time electric metter readings, kw/h
 *      "off_peak" - initial off-peak time electric metter readings, kw/h
 * 
 * Output variables: 
 *      "w" - momentary power in Watts
 *      "total" - momentary value of electric metter. kw/h
 *      "peak" - momentary value of peak time electric metter, kw/h
 *      "off_peak" - momentary value of off-peak time electric metter, kw/h
 */

#include "PowerSensor.h"

void readSensor(Plugin *plugin)
{
    ((PowerSensor *)plugin)->read();
}

PowerSensor::PowerSensor(const char *instanceId, uint8_t pin) : Plugin(instanceId)
{
    _pin = pin;
    registerAction("read", readSensor);
}

void PowerSensor::read()
{
    float watts = _readPowerSensor();
    unsigned long durationMs = getElapsedTime(_lastPowerCheckPoint);
    _lastPowerCheckPoint = millis();

    // kW/h = 1000 * W / (60 * 60 * 1000 * ms) = W / (3600 * ms)
    float segmentEnergy = (float)watts / ((float)durationMs * 3600.0f);

    setVar(VAR_WATTS, watts);
    setVar(VAR_TOTAL_METTER, getVarFloat(VAR_TOTAL_METTER) + segmentEnergy);

    if (_isPeakTime())
        setVar(VAR_PEAK_METTER, getVarFloat(VAR_PEAK_METTER) + segmentEnergy);
    else
        setVar(VAR_OFF_PEAK_METTER, getVarFloat(VAR_OFF_PEAK_METTER) + segmentEnergy);
}

bool PowerSensor::_isPeakTime()
{
    uint8_t wd = weekday();
    if (wd == 1 || wd == 7)
        return false;

    uint8_t hr = hour();

    uint8_t peakStart = getVarInt(VAR_PEAK_START_HOUR, DEFAULT_PEAK_START_HOUR_GMT);
    uint8_t peakEnd = getVarInt(VAR_PEAK_END_HOUR, DEFAULT_PEAK_END_HOUR_GMT);

    return hr >= peakStart && hr < peakEnd;
}

unsigned long PowerSensor::_getWaveAmplitude()
{
    unsigned long waveMin = ULONG_MAX;
    unsigned long waveMax = 0UL;

    unsigned long startTime = millis();

    // 50 Hz -> 1 wave = 1/50s = 20ms ( and couple of ms, "just in case")
    // It will work for shorter waves as well, eg. 60 hz
    while (getTimeout(startTime) <= 22)
    {
        unsigned long power = (unsigned long)analogRead(_pin);
        if (power > waveMax)
            waveMax = power;
        if (power < waveMin)
            waveMin = power;
    }

    return (waveMax - waveMin);
}

float PowerSensor::_readPowerSensor()
{
    unsigned long averageAmplitude = 0;

    for (int i = 0; i < AVERAGING_CYCLES; i++)
    {
        averageAmplitude += _getWaveAmplitude();
        yield(); // allow MCU to do house keeping, like WiFi
    }

    // convert voltage to Watts
    // P is measured empirically
    float P = getVarFloat(VAR_KOEF_P, DEFAULT_P);

    return ((float)averageAmplitude / (float)AVERAGING_CYCLES) * P;
}

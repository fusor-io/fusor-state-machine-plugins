#ifndef powersensor_h
#define powersensor_h

#include <limits.h>
#include <Time.h>

#include <Arduino.h>

#include <StateMachine.h>
#include <NodeConnector.h>

// Default coefficient for converting ADC voltage readings to watts.
// Value valid for specific sensor should be measured empirically
// and provided through StateMachine store ("keof_p" variable).
#define DEFAULT_P 5.7213503f

// How many measurements to made for averaging sensor readings
// 25 cycles -> 0.5 second (at 50Hz)
#define AVERAGING_CYCLES 25

#define VAR_KOEF_P "coef_p"
#define VAR_WATTS "w"
#define VAR_TOTAL_METTER "total"
#define VAR_PEAK_METTER "peak"
#define VAR_OFF_PEAK_METTER "off_peak"
#define VAR_PEAK_START_HOUR "peak_start"
#define VAR_PEAK_END_HOUR "peak_end"

#define DEFAULT_PEAK_START_HOUR_GMT 5
#define DEFAULT_PEAK_END_HOUR_GMT 21

class PowerSensorPlugin : public Plugin
{
public:
    PowerSensorPlugin(const char *, uint8_t);
    void read();

private:
    uint8_t _pin;

    float _lastPower = 0.0;
    unsigned long _lastPowerCheckPoint = millis();

    bool _isPeakTime();
    unsigned long _getWaveAmplitude();
    float _readPowerSensor();
};

#endif
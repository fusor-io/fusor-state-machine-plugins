#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>

#define NAN __builtin_nan("")

bool ReadRegister(uint8_t, uint8_t *, uint8_t);

class TwoWire
{
public:
    TwoWire();
    void beginTransmission(uint8_t);
    void write(uint8_t);
    void endTransmission();
    void requestFrom(uint8_t, uint8_t);
    bool available();
    uint8_t read();
};

extern TwoWire Wire;

#endif

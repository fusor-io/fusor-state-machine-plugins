#include "Wire.h"

#include <inttypes.h>

bool ReadRegister(uint8_t a, uint8_t *b, uint8_t c) { return true; }

TwoWire::TwoWire() {}
void TwoWire::beginTransmission(uint8_t a) {}
void TwoWire::write(uint8_t a) {}
void TwoWire::endTransmission() {}
void TwoWire::requestFrom(uint8_t a, uint8_t b) {}
bool TwoWire::available() { return true; }
uint8_t TwoWire::read() { return 0; }

TwoWire Wire = TwoWire();
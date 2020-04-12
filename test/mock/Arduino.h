//Mock for running tests

#ifndef Arduino_h
#define Arduino_h

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short signed int int16_t;
typedef long signed int int64_t;
typedef short unsigned int __uint16_t;
typedef __uint16_t uint16_t;
typedef signed int __int32_t;
typedef __int32_t int32_t;
typedef unsigned int __uint32_t;
typedef __uint32_t uint32_t;

inline void delay(unsigned long) {}
inline void attachInterrupt(int, void (*)(), int){};
inline int digitalPinToInterrupt(int v) { return v; };
inline unsigned long micros() { return 0; }

inline void pinMode(uint8_t, uint8_t){};
inline void digitalWrite(uint8_t, uint8_t){};
inline int digitalRead(uint8_t) { return 0; };
inline int analogRead(uint8_t) { return 0; };
inline void analogReference(uint8_t mode){};
inline void analogWrite(uint8_t, int){};

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define HIGH 1
#define LOW 0

#define IRAM_ATTR

#define RISING 0
#define FALLING 1
#define CHANGE 2

#endif
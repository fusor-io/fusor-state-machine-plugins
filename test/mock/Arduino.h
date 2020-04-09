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
inline int digitalPinToInterrupt(int){};

inline void pinMode(int, int){};

#define INPUT_PULLUP 0

#define IRAM_ATTR

#define RISING 0
#define FALLING 0

#endif
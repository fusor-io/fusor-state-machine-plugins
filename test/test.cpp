#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <float.h>
#include <limits.h>

#include <ArduinoJson.h>
#include <StateMachine.h>

#include "../src/bme280/bme280.h"

unsigned long _time = 0;
unsigned long getTime() { return _time; }

TEST(BME280, Init)
{
  StateMachineController sm = StateMachineController("test", NULL, getTime);
  bme280 bme = bme280(&sm, 1, 2, true);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

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

TEST(Plugin, setVar)
{
  StateMachineController sm = StateMachineController("test", NULL, getTime);
  Plugin plugin = Plugin("plugin", &sm);
  plugin.setVar("var1", 42);
  ASSERT_EQ(sm.compute.store.getVarInt("plugin.var1"), 42);

  plugin.setVar("var2", 137.0f);
  ASSERT_FLOAT_EQ(sm.compute.store.getVarFloat("plugin.var2"), 137.0f);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

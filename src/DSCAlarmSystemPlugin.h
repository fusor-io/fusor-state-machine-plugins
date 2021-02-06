#ifndef dscalarmsystem_h
#define dscalarmsystem_h

#include <Arduino.h>

#include <StateMachine.h>
#include <NodeConnector.h>

#define READ_DELAY_MICRO 100
#define DSC_MAX_BITS 74

#define PROP_FIRE "fire"
#define PROP_PROGRAM "prog"
#define PROP_ERROR "err"
#define PROP_BYPASS "byps"
#define PROP_MEMORY "mem"
#define PROP_ARMED_SIGN "asign"
#define PROP_ARMED "armed"
#define PROP_READY "ready"
#define PROP_CLEAN "clean"
#define PROP_ZONE_OPEN "zopen"
#define PROP_PERIMETER_OPEN "popen"
#define PROP_EXIT_DELAY "exit"
#define PROP_BEEPS "beep"

class DSCAlarmSystemPlugin : public Plugin
{
public:
    DSCAlarmSystemPlugin(const char *, uint8_t, uint8_t);

    unsigned long lastDecodingTime = 0;

    uint8_t LoBits[DSC_MAX_BITS];
    uint8_t HiBits[DSC_MAX_BITS];
    uint8_t PrevLoBits[DSC_MAX_BITS];
    uint8_t PrevHiBits[DSC_MAX_BITS];
    uint8_t bytes[10];
    uint8_t zones[32];

    uint8_t cmd;

    bool isBacklight;
    bool isFire;
    bool isProgram;
    bool isError;
    bool isBypass;
    bool isMemory;
    bool isArmedSign;
    bool isReady;
    bool isClean;
    bool isZoneActive;
    bool isPerimeterOpen;
    bool isArmed;
    bool isExitDelay;

    VarStruct *isBacklightVar = nullptr;
    VarStruct *isFireVar = nullptr;
    VarStruct *isProgramVar = nullptr;
    VarStruct *isErrorVar = nullptr;
    VarStruct *isBypassVar = nullptr;
    VarStruct *isMemoryVar = nullptr;
    VarStruct *isArmedSignVar = nullptr;
    VarStruct *isReadyVar = nullptr;
    VarStruct *isCleanVar = nullptr;
    VarStruct *isZoneActiveVar = nullptr;
    VarStruct *isPerimeterOpenVar = nullptr;
    VarStruct *isArmedVar = nullptr;
    VarStruct *isExitDelayVar = nullptr;

    VarStruct *sensorVars[32] = {nullptr};

    VarStruct *beepVar;

    bool isKeypadsQueried;
    bool isCRCOK;

    bool bitsReady;

    uint8_t beep;

    void waitForLowLevel();
    void waitForSyncStart();
    void skipUntilSyncEnd();
    void resync();
    uint8_t readLowBit();
    uint8_t readHighBit();
    void initRegister();
    void readBits();
    uint8_t readByte(uint8_t *);
    void readBytes();
    bool checkCrc();
    bool isNewBits();

    void reset_readings();
    void decode_status();
    void decode_msg();
    void decode_zones(uint8_t);
    char decodeKeyPress();
    void decodePacket();

    void updateVariables();

    void read();

private:
    uint8_t _clockPin;
    uint8_t _signalPin;
};

#endif
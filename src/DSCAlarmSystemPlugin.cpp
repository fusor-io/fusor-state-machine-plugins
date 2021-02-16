#include "DSCAlarmSystemPlugin.h"

/**
 * Plugin for connecting into DSC based alarm system communication wires.
 * It only allows reading system state and events. No write operatins supported.
 *  * 
 * @see
 * 
 * http://www.avrfreaks.net/comment/804380#comment-804380
 * http://electronics.stackexchange.com/questions/11017/dsc-keybus-protocol
 * https://github.com/hvesalai/keybus/blob/master/keybus-protocol.c
 * 
 */

void _readDSC(Plugin *plugin)
{
    ((DSCAlarmSystemPlugin *)plugin)->read();
}

const char *zonesIds[] = {"z1", "z2", "z3", "z4", "z5", "z6", "z7", "z8",
                          "z9", "z10", "z11", "z12", "z13", "z14", "z15", "z16",
                          "z17", "z18", "z19", "z20", "z21", "z22", "z23", "z24",
                          "z25", "z26", "z27", "z28", "z29", "z30", "z31", "z32"};

DSCAlarmSystemPlugin::DSCAlarmSystemPlugin(const char *instanceId, uint8_t clockPin, uint8_t signalPin) : Plugin(instanceId)
{
    _clockPin = clockPin;
    _signalPin = signalPin;

    pinMode(clockPin, INPUT);
    pinMode(signalPin, INPUT);

    memset(LoBits, 0, DSC_MAX_BITS);
    memset(HiBits, 0, DSC_MAX_BITS);
    memset(PrevLoBits, 0, DSC_MAX_BITS);
    memset(PrevHiBits, 0, DSC_MAX_BITS);
    memset(zones, 0, 32);

    bitsReady = false;

    registerAction("read", _readDSC);
}

// wait for low sync level. used for resync (before waitForSync)
void DSCAlarmSystemPlugin::waitForLowLevel()
{
    unsigned int clock = 1;

    while (clock == 1)
    {
        clock = digitalRead(_clockPin);
        delayMicroseconds(READ_DELAY_MICRO / 4); // go in smaller steps to catch the very begining
    }
}

// Wait for the begining of sync pulse (~20ms long)
void DSCAlarmSystemPlugin::waitForSyncStart()
{
    unsigned int count = 0, clock, signal;

    // regular clock pulse is ~5 times of our read cycle;
    // so if we have pulse over 10 cycles, it is sign that 20ms sync pulse is begining

    while (count < 10)
    {
        clock = digitalRead(_clockPin);
        signal = digitalRead(_signalPin); // not necessary, but just for keeping comparable read cycle duration
        if (clock == 1)
        {
            count++;
        }
        else
        {
            count = 0;
        }
        delayMicroseconds(READ_DELAY_MICRO);
    }
}

// Wait until Sunc pulse ends
void DSCAlarmSystemPlugin::skipUntilSyncEnd()
{
    unsigned int clock = 1;
    while (clock == 1)
    {
        clock = digitalRead(_clockPin);
        delayMicroseconds(READ_DELAY_MICRO / 4); // go in smaller steps to catch the very begining
    }
}

void DSCAlarmSystemPlugin::resync()
{
    waitForLowLevel();
    waitForSyncStart();
}

// Read bit while clock level is low
uint8_t DSCAlarmSystemPlugin::readLowBit()
{
    int clock, signal = 0;

    for (int i = 0; i < 3; i++)
    {
        clock = digitalRead(_clockPin);
        if (clock == 1)
        {
            // too early, but let it be
            break;
        }
        signal += digitalRead(_signalPin);
        delayMicroseconds(READ_DELAY_MICRO);
    }

    while (clock == 0)
    {
        clock = digitalRead(_clockPin);
        delayMicroseconds(READ_DELAY_MICRO / 2); // go in smaller steps to catch the very begining of high clock level
    }

    return (signal >= 2) ? 1 : 0;
}

// Read bit while clock level is high
uint8_t DSCAlarmSystemPlugin::readHighBit()
{
    int clock, signal = 0;

    for (int i = 0; i < 3; i++)
    {
        clock = digitalRead(_clockPin);
        if (clock == 0)
        {
            // too early, but let it be
            break;
        }
        signal += digitalRead(_signalPin);
        delayMicroseconds(READ_DELAY_MICRO);
    }

    while (clock == 1)
    {
        clock = digitalRead(_clockPin);
        delayMicroseconds(READ_DELAY_MICRO / 2); // go in smaller steps to catch the very begining of low clock level
    }

    return (signal >= 2) ? 1 : 0;
}

void DSCAlarmSystemPlugin::initRegister()
{
    memset(LoBits, 0, DSC_MAX_BITS);
    memset(HiBits, 0, DSC_MAX_BITS);
    reset_readings();
}

// read bits packet
void DSCAlarmSystemPlugin::readBits()
{
    // there are 74 low clock level bits
    // and 73 high clock level bits

    for (int i = 0; i < DSC_MAX_BITS - 1; i++)
    {
        LoBits[i] = readLowBit();
        HiBits[i] = readHighBit();
    }
    LoBits[DSC_MAX_BITS - 1] = readLowBit();

    bitsReady = true;
}

// compose byte from bits
uint8_t DSCAlarmSystemPlugin::readByte(uint8_t *bits)
{
    uint8_t b = 0;
    for (int i = 0; i < 8; i++)
    {
        b = (b << 1) | bits[i];
    }
    return b;
}

// compose bytes of high interest
void DSCAlarmSystemPlugin::readBytes()
{
    bytes[0] = readByte(&HiBits[0]);  // cmd
                                      // stop bit
    bytes[1] = readByte(&HiBits[9]);  // status
    bytes[2] = readByte(&HiBits[17]); // sensors
    bytes[3] = readByte(&HiBits[25]); // sensors
    bytes[4] = readByte(&HiBits[33]); // sensors
    bytes[5] = readByte(&HiBits[41]); // sensors
    bytes[6] = readByte(&HiBits[49]); // crc

    bytes[7] = readByte(&LoBits[3]);       // keyboard slot
    bytes[8] = readByte(&LoBits[11]) >> 2; // key press
}

bool DSCAlarmSystemPlugin::checkCrc()
{
    uint8_t crc = 0;
    for (int i = 0; i < 6; i++)
        crc += bytes[i];
    return bytes[6] == crc;
}

bool DSCAlarmSystemPlugin::isNewBits()
{
    bool updated;

    updated = memcmp(LoBits, PrevLoBits, DSC_MAX_BITS) != 0 || memcmp(HiBits, PrevHiBits, DSC_MAX_BITS) != 0;
    memcpy(PrevLoBits, LoBits, DSC_MAX_BITS);
    memcpy(PrevHiBits, HiBits, DSC_MAX_BITS);

    return updated;
}

void DSCAlarmSystemPlugin::reset_readings()
{
    isBacklight = isFire = isProgram = isError = isBypass = isMemory = isArmedSign = isReady = false;
    isClean = isZoneActive = isPerimeterOpen = isExitDelay = isArmed = isKeypadsQueried = false;
    isCRCOK = true;
    cmd = beep = 0;
    memset(zones, 0, 32);
}

void DSCAlarmSystemPlugin::decode_status()
{
    uint8_t status = bytes[1];
    isBacklight = (status & 0x80);
    isFire = (status & 0x40);
    isProgram = (status & 0x20);
    isError = (status & 0x10);
    isBypass = (status & 0x08);
    isMemory = (status & 0x04);
    isArmedSign = (status & 0x02);
    isReady = (status & 0x01);
}

void DSCAlarmSystemPlugin::decode_msg()
{
    uint8_t msg = bytes[2];

    isClean = (msg & 0x03) == 0x01;         // XXXXXX01
    isZoneActive = (msg & 0x03) == 0x02;    // XXXXXX10
    isPerimeterOpen = (msg & 0x03) == 0x03; // XXXXXX11
    isExitDelay = (msg & 0x08) == 0x08;     // XXXX1XXX
    isArmed = (msg & 0x10) == 0x10;         // XXX1XXXX
}

void DSCAlarmSystemPlugin::decode_zones(uint8_t cmd)
{
    int delta = 0;
    uint8_t mask = 0x01;
    uint8_t sensors = bytes[5];

    // get sensors group offset
    switch (cmd)
    {
    case 0x2D:
        delta = 8;
        break;
    case 0x34:
        delta = 16;
        break;
    case 0x3E:
        delta = 24;
        break;
    }

    // decode sensor number
    for (int i = delta; i < delta + 8; i++)
    {
        if (sensors & mask)
            zones[i] = 1;
        mask = mask << 1;
    }
}

char DSCAlarmSystemPlugin::decodeKeyPress()
{
    uint8_t key = bytes[8];

    if (key >= 0x3F)
        return -1; // no keys - 11 11 11
    uint8_t row = (key & 0x30) >> 4;
    uint8_t col = (key & 0x0C) >> 2;
    uint8_t crc = key & 0x03;
    if (crc != ((row + col) & 0x03))
        return -2;

    uint8_t code = (key & 0x3C) >> 2;
    if (code < 10)
    {
        return ((char)code + '0');
    }
    else if (code == 10)
    {
        return '*';
    }
    else if (code == 11)
    {
        return '#';
    }
    else
    {
        return '?';
    }
}

void DSCAlarmSystemPlugin::decodePacket()
{
    readBytes();

    // reset what we have from the last decoding
    reset_readings();

    // decode command
    cmd = bytes[0];

    if (cmd == 0x05)
    { // 00000101 = > STATUS 0 (basic info, no crc)
        decode_status();
        decode_msg();
    }

    if (cmd == 0x27 || cmd == 0x2D || cmd == 0x34 || cmd == 0x3E)
    { //00100111 = -> STATUS 1 (extended zone info and crc)
        if (checkCrc())
        {
            isCRCOK = true;
            decode_status();
            decode_msg();
            decode_zones(cmd);
        }
        else
        {
            isCRCOK = false;
        }
    }

    if (cmd == 0x11)
    { // 00010001 -> QUERY keyboards
        isKeypadsQueried = true;
        // we could also decode what slots are used from HiBits
    }

    if (cmd == 0xA5)
    { // 10100101 -> timedate
        // not interested
    }

    if (cmd == 0x64)
    { // beep
        switch (bytes[1])
        {
        case 0x06:
            beep = 1;
            break; // 3 short beeps
        case 0x0A:
            beep = 2;
            break; // 4 short beeps
        case 0x0C:
            beep = 3;
            break; // Long beep
        default:
            beep = 4;
            break; // Unknown beep mode
        }
    }

    // if (decodeKeypadSlot())
    // {
    //     decodeKeyPress(); // TBD - do not transmit
    // }
}

void DSCAlarmSystemPlugin::updateVariables()
{
    if (!isCRCOK)
        return;

    if (cmd == 0x05 || cmd == 0x27 || cmd == 0x2D || cmd == 0x34 || cmd == 0x3E)
    {

        isFireVar = updateVar(isFireVar, PROP_FIRE, (long int)isFire);
        isProgramVar = updateVar(isProgramVar, PROP_PROGRAM, (long int)isProgram);
        isBypassVar = updateVar(isBypassVar, PROP_BYPASS, (long int)isBypass);
        isMemoryVar = updateVar(isMemoryVar, PROP_MEMORY, (long int)isMemory);
        isArmedSignVar = updateVar(isArmedSignVar, PROP_ARMED_SIGN, (long int)isArmedSign);
        isReadyVar = updateVar(isReadyVar, PROP_READY, (long int)isReady);
        isCleanVar = updateVar(isCleanVar, PROP_CLEAN, (long int)isClean);
        isZoneActiveVar = updateVar(isZoneActiveVar, PROP_ZONE_OPEN, (long int)isZoneActive);
        isPerimeterOpenVar = updateVar(isPerimeterOpenVar, PROP_PERIMETER_OPEN, (long int)isPerimeterOpen);
        isArmedVar = updateVar(isArmedVar, PROP_ARMED, (long int)isArmed);
        isExitDelayVar = updateVar(isExitDelayVar, PROP_EXIT_DELAY, (long int)isExitDelay);

        for (uint8_t i = 0; i < 32; i++)
            sensorVars[i] = updateVar(sensorVars[i], zonesIds[i], (long int)zones[i]);

        isExitDelayVar = updateVar(isExitDelayVar, PROP_EXIT_DELAY, (long int)isExitDelay);

        // just reset this as we only getting "on" event
        beepVar = updateVar(beepVar, PROP_BEEPS, 0L);
    }
    else if (cmd == 0x64)
    {
        beepVar = updateVar(beepVar, PROP_BEEPS, (long int)beep);
    }
}

void DSCAlarmSystemPlugin::read()
{
    initRegister(); // prepare registers for new data

    // our idle time is max 20ms, reduce it to 18 to be on a safe size
    // if we are out of this time, we need to resync
    if (getElapsedTime(lastDecodingTime) > 18)
        resync();

    skipUntilSyncEnd();

    readBits();

    lastDecodingTime = millis();

    waitForSyncStart(); // we will spend here ~1ms

    // from this moment we have only 20-1 = 19 ms
    // we have to:
    //  - decode data
    //  - exit state machine loop
    //  - send data over WiFi
    //  - casually read configs by WiFi
    //  - enter new state machine cycle

    // if it is false, probably this is our first run, no info collected yet
    if (bitsReady)
    {
        decodePacket();

        if (isNewBits())
            updateVariables();
    }
}

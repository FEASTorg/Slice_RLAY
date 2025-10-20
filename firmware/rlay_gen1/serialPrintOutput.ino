#include <Arduino.h>

// Uses globals from rlay_gen1.ino:
// RELAY_PINS, ANALOG_PINS, ADC_MAX, g_refVoltage, g_relays, g_streamIntervalMs

static float readVoltage(uint8_t aPin)
{
    const int raw = analogRead(aPin);
    return (float)raw * (g_refVoltage / (float)ADC_MAX);
}

void printHelp()
{
    Serial.println(F("Commands:"));
    Serial.println(F("  HELP"));
    Serial.println(F("  PINS"));
    Serial.println(F("  SET <1-4> <ON|OFF>"));
    Serial.println(F("  GET <1-4|ALL>"));
    Serial.println(F("  READ <1-4|ALL>"));
    Serial.println(F("  REF <volts>"));
    Serial.println(F("  STREAM <ms|OFF>"));
}

void printPins()
{
    Serial.println(F("Relays: 1:D6  2:D9  3:D10  4:D11"));
    Serial.println(F("Analogs: 1:A0  2:A1  3:A2   4:A3"));
}

void printStatus(uint8_t idx)
{
    if (idx < 1 || idx > 4)
    {
        Serial.println(F("ERR Bad index"));
        return;
    }
    const bool st = g_relays[idx - 1];
    Serial.print(F("SW"));
    Serial.print(idx);
    Serial.print(F("="));
    Serial.println(st ? F("ON") : F("OFF"));
}

void printStatusAll()
{
    for (uint8_t i = 1; i <= 4; i++)
        printStatus(i);
}

void printRead(uint8_t idx)
{
    if (idx < 1 || idx > 4)
    {
        Serial.println(F("ERR Bad index"));
        return;
    }
    const float v = readVoltage(ANALOG_PINS[idx - 1]);
    Serial.print(F("A"));
    Serial.print(idx);
    Serial.print(F("="));
    Serial.println(v, 3);
}

void printReadAll()
{
    for (uint8_t i = 1; i <= 4; i++)
        printRead(i);
}

// CSV stream line: v1,v2,v3,v4
void printCSVAnalogLine()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        const float v = readVoltage(ANALOG_PINS[i]);
        Serial.print(v, 3);
        if (i < 3)
            Serial.print(',');
    }
    Serial.println();
}

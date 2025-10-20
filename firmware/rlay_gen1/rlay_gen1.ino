/*
  Relay + Analog Input Test
  Control-only via Serial. Non-blocking. Professional split across 3 files.
  Relays: SW1..SW4 → D6, D9, D10, D11
  Analogs: A_IN1..A_IN4 → A0..A3
  Baud: 115200

  Commands (case-insensitive):
    HELP
    PINS
    SET <1-4> <ON|OFF>
    GET <1-4|ALL>
    READ <1-4|ALL>
    REF <volts>
    STREAM <ms|OFF>
*/

#include <Arduino.h>

// === Pins ===
const uint8_t RELAY_PINS[4] = {6, 9, 10, 11};    // SW1..SW4
const uint8_t ANALOG_PINS[4] = {A0, A1, A2, A3}; // A_IN1..A_IN4

// === Config ===
const uint16_t ADC_MAX = 1023;
float g_refVoltage = 11.90f;          // Adjustable via REF command
unsigned long g_streamIntervalMs = 0; // 0 = OFF
const uint32_t BAUD = 115200;

// === State ===
bool g_relays[4] = {false, false, false, false};
unsigned long g_lastStreamAt = 0;

// === Prototypes (implemented across files) ===
void serialCommandInput();
void printHelp();
void printPins();
void printStatus(uint8_t idx); // 1..4
void printStatusAll();
void printRead(uint8_t idx); // 1..4
void printReadAll();
void printCSVAnalogLine();

// === Setup ===
void setup()
{
    Serial.begin(BAUD);
    for (uint8_t i = 0; i < 4; i++)
    {
        pinMode(RELAY_PINS[i], OUTPUT);
        digitalWrite(RELAY_PINS[i], LOW);
        pinMode(ANALOG_PINS[i], INPUT);
    }
    Serial.println(F("READY"));
    Serial.println(F("Type HELP"));
}

// === Loop (non-blocking) ===
void loop()
{
    serialCommandInput();

    if (g_streamIntervalMs > 0)
    {
        const unsigned long now = millis();
        if (now - g_lastStreamAt >= g_streamIntervalMs)
        {
            printCSVAnalogLine();
            g_lastStreamAt = now;
        }
    }
}

#include <Arduino.h>

// Uses globals from rlay_gen1.ino:
// RELAY_PINS, g_relays, g_refVoltage, g_streamIntervalMs

// === Simple line-buffered parser ===
static const size_t BUF_SZ = 64;
static char lineBuf[BUF_SZ];
static size_t lineLen = 0;

static void setRelay(uint8_t idx, bool on)
{
    if (idx < 1 || idx > 4)
    {
        Serial.println(F("ERR Bad index"));
        return;
    }
    const uint8_t i = idx - 1;
    g_relays[i] = on;
    digitalWrite(RELAY_PINS[i], on ? HIGH : LOW);
    Serial.println(F("OK"));
}

static bool equalsToken(const char *a, const char *b)
{
    // Case-insensitive compare
    while (*a && *b)
    {
        char ca = *a >= 'a' && *a <= 'z' ? *a - 32 : *a;
        char cb = *b >= 'a' && *b <= 'z' ? *b - 32 : *b;
        if (ca != cb)
            return false;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

void serialCommandInput()
{
    while (Serial.available())
    {
        char c = (char)Serial.read();
        if (c == '\r')
            continue;
        if (c != '\n' && lineLen < (BUF_SZ - 1))
        {
            lineBuf[lineLen++] = c;
            continue;
        }

        // End of line or buffer full
        lineBuf[lineLen] = '\0';
        lineLen = 0;

        // Tokenize
        if (lineBuf[0] == '\0')
            continue; // empty line
        char *saveptr = nullptr;
        char *cmd = strtok_r(lineBuf, " \t", &saveptr);
        if (!cmd)
            continue;

        // HELP
        if (equalsToken(cmd, "HELP"))
        {
            printHelp();
            continue;
        }

        // PINS
        if (equalsToken(cmd, "PINS"))
        {
            printPins();
            continue;
        }

        // SET <1-4> <ON|OFF>
        if (equalsToken(cmd, "SET"))
        {
            char *tokIdx = strtok_r(nullptr, " \t", &saveptr);
            char *tokVal = strtok_r(nullptr, " \t", &saveptr);
            if (!tokIdx || !tokVal)
            {
                Serial.println(F("ERR Usage: SET <1-4> <ON|OFF>"));
                continue;
            }
            uint8_t idx = (uint8_t)atoi(tokIdx);
            if (equalsToken(tokVal, "ON"))
            {
                setRelay(idx, true);
            }
            else if (equalsToken(tokVal, "OFF"))
            {
                setRelay(idx, false);
            }
            else
            {
                Serial.println(F("ERR Use ON|OFF"));
            }
            continue;
        }

        // GET <1-4|ALL>
        if (equalsToken(cmd, "GET"))
        {
            char *tok = strtok_r(nullptr, " \t", &saveptr);
            if (!tok)
            {
                Serial.println(F("ERR Usage: GET <1-4|ALL>"));
                continue;
            }
            if (equalsToken(tok, "ALL"))
            {
                printStatusAll();
            }
            else
            {
                uint8_t idx = (uint8_t)atoi(tok);
                printStatus(idx);
            }
            continue;
        }

        // READ <1-4|ALL>
        if (equalsToken(cmd, "READ"))
        {
            char *tok = strtok_r(nullptr, " \t", &saveptr);
            if (!tok)
            {
                Serial.println(F("ERR Usage: READ <1-4|ALL>"));
                continue;
            }
            if (equalsToken(tok, "ALL"))
            {
                printReadAll();
            }
            else
            {
                uint8_t idx = (uint8_t)atoi(tok);
                printRead(idx);
            }
            continue;
        }

        // REF <volts>
        if (equalsToken(cmd, "REF"))
        {
            char *tok = strtok_r(nullptr, " \t", &saveptr);
            if (!tok)
            {
                Serial.println(F("ERR Usage: REF <volts>"));
                continue;
            }
            float v = atof(tok);
            if (v <= 0.0f || v > 50.0f)
            {
                Serial.println(F("ERR Out of range"));
                continue;
            }
            g_refVoltage = v;
            Serial.println(F("OK"));
            continue;
        }

        // STREAM <ms|OFF>
        if (equalsToken(cmd, "STREAM"))
        {
            char *tok = strtok_r(nullptr, " \t", &saveptr);
            if (!tok)
            {
                Serial.println(F("ERR Usage: STREAM <ms|OFF>"));
                continue;
            }
            if (equalsToken(tok, "OFF"))
            {
                g_streamIntervalMs = 0;
                Serial.println(F("OK"));
            }
            else
            {
                long ms = atol(tok);
                if (ms < 10)
                {
                    Serial.println(F("ERR Min 10ms"));
                    continue;
                }
                g_streamIntervalMs = (unsigned long)ms;
                Serial.println(F("OK"));
            }
            continue;
        }

        // Unknown
        Serial.print(F("ERR Unknown: "));
        Serial.println(cmd);
    }
}

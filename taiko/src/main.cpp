#include <Arduino.h>

#define val const auto
#define let auto

/**
 * Since the arduino library doesn't have printf, we have to make our own
 */
void send(const char* format, ...)
{
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.println(buffer);
}

const int NUM_DRUMSTICKS = 2;
const int DRUMSTICK_PINS[] = {A0, A1};
int LAST_VALUES[] = {0, 0};

// Values range from 0 to 1023, but since we reduced the resolution to 8 bits, we need to divide by 8
// After the division, values range from 0 to 127
val threshold = (int) (0.7 * 127);

void setup()
{
    Serial.begin(115200);
    for (val pin: DRUMSTICK_PINS) pinMode(pin, INPUT);

    send("Initialized");
}

void loop()
{
    // Loop through all the pins, indexed
    for (let i = 0; i < NUM_DRUMSTICKS; i++)
    {
        val pin = DRUMSTICK_PINS[i];
        val value = analogRead(pin) / 8;

        // If the value is different from the last value, update it
        if (value != LAST_VALUES[i])
        {
            // If it's less than the threshold
            if (value < threshold)
            {
                // If the last value was greater than the threshold, it means the drumstick was just hit
                if (LAST_VALUES[i] > threshold)
                    send("Hit:+%d", i);
            }
            else
            {
                send(";%d;%d", i, value);

                // If the last value was less than the threshold, it means the drumstick was just released
                if (LAST_VALUES[i] < threshold)
                    send("Hit:-%d", i);
            }
        }
        
        LAST_VALUES[i] = value;
    }
}
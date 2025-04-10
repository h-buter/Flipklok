#include <Arduino.h>
#include <WiFi.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>
#include "secrets.h"

// #include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi credentials

const char *ssid =SECRET_WIFI_SSID;
const char *password =SECRET_WIFI_PASS;


void encodeDCF77UnixEpoch(uint8_t *dcfBits);

void intToBCD(unsigned int number, uint8_t *bitArray, size_t bitArrayLength);
void encodeDCF77(uint8_t *dcfBits, int minute, int hour, int day, int month, int year);

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// DCF77 Output pin
const int DCF77_PIN = 4; //D2
// const int selectPin = 19; //D2

bool fastTiming = false;

unsigned int onTimeOne;
unsigned int onTimeZero;
unsigned int period;

void setup() {
    if(fastTiming == true)
    {
        onTimeOne = 100;
        onTimeZero = 50;
        period = 500;
    }
    else
    {
        onTimeOne = 200;
        onTimeZero = 100;
        period = 1000;
    }
    Serial.begin(115200);
    Serial.println("DCF77 emulator");
    pinMode(DCF77_PIN, OUTPUT);
    digitalWrite(DCF77_PIN, LOW);

    // pinMode(selectPin, INPUT_PULLUP);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    timeClient.begin();

    // Emulate a low bit 58
    digitalWrite(DCF77_PIN, HIGH);
    delay(100);
    digitalWrite(DCF77_PIN, LOW);
    delay(900);

}

void loop() 
{
    unsigned long start = millis();
    int year = 0;
    time_t rawTime;
    struct tm *timeInfo;
    while(year == 0)
    {
        timeClient.update();
        rawTime = timeClient.getEpochTime();
        timeInfo = gmtime(&rawTime);
        if ((timeInfo->tm_year + 1900) < 2025)
        {
            year = 0;
        }
        else
        {
            year = timeInfo->tm_year + 1900;
        }
    }
    int minute = timeInfo->tm_min + 1;
    if (minute >= 60)
    {
        minute = 0;
    }
    int hour = timeInfo->tm_hour + 2;
    int day = timeInfo->tm_mday;
    int month = timeInfo->tm_mon + 1;

    
    Serial.printf("\n Time: %02d:%02d:%02d %02d-%02d-%04d\n", hour, minute, timeInfo->tm_sec, day, month, year);
    
    uint8_t dcfBits[60]; // DCF77 transmits 60 bits per minute

    // if (HIGH == digitalRead(selectPin))
    // {
    //     Serial.println("epoch");
    //     encodeDCF77UnixEpoch(dcfBits);
    // }
    // else
    // {   
        Serial.println("NTP");
        encodeDCF77(dcfBits, minute, hour, day, month, year);
    // }

    
    
    Serial.println("Transmitting DCF77 Signal...");
    unsigned long waitSync = 1800 - (millis() - start);
    Serial.print("waitSync: ");
    Serial.println(waitSync);
    //startup
    if (1 == dcfBits[58]) //59 is always 0 but 58 not and this needs to be taken in to account for the sync time
    {
        delay(waitSync-800);
    }
    else
    {
        delay(waitSync-900);
    }

    // Bit 0:
    digitalWrite(DCF77_PIN, HIGH);
    delay(100);
    digitalWrite(DCF77_PIN, LOW);
    delay(900);

    for (int i = 1; i <= 58; i++) //start with bit 1 because bit 0 is always 0
    {
        Serial.print(i);
        Serial.print(",");

        if (1 == dcfBits[i]) //1 200ms high
        {
            digitalWrite(DCF77_PIN, HIGH);
            delay(onTimeOne);
            digitalWrite(DCF77_PIN, LOW);
            delay(period-onTimeOne);
        }
        else //0 100 ms high
        {
            digitalWrite(DCF77_PIN, HIGH);
            delay(onTimeZero);
            digitalWrite(DCF77_PIN, LOW);
            delay(period - onTimeZero);
        }
    }
}

void encodeDCF77(uint8_t *dcfBits, int minute, int hour, int day, int month, int year) 
{
    memset(dcfBits, 0, 59);

    Serial.printf("minute: %d, hour: %d, day: %d, month: %d, year: %d \n", minute, hour, day, month, year);

    unsigned int pos = 0;
    uint8_t bits[16]; // bits


    dcfBits[0] = 0; // Start of minute marker (always 0)

    // Serial.println("");
    // Serial.print("civil warning: ");
    for (int i = 1; i <= 9; i++ )
    {
        // Serial.printf(" %02d", i);
        dcfBits[i] = 1;
    }
    for (int i = 10; i <= 14; i++ )
    {
        // Serial.printf(" %02d", i);
        dcfBits[i] = 0;
    }

    // Serial.println("");
    // Serial.print("summer time bits: ");
    for (int i = 15; i <= 19; i++ )
    {
        // Serial.printf(" %02d", i);
        dcfBits[i] = 0;
    }
    dcfBits[19] = 0; // No leap second announcement

    // Serial.println("");
    // Serial.printf("minute: %d: ", minute);
    dcfBits[20] = 1; //Start of minute
    intToBCD(minute, bits, sizeof(bits));
    pos = 0;
    for (unsigned int i = 0; i < 7; i++)
    {
        dcfBits[i + 21] = bits[i];
        // Serial.print(bits[i]);
        if (1 == bits[i])
        {
            pos++;
        }
    }
    if (pos % 2) // if odd
    {
        dcfBits[28] = 1; //parity
    }

    // Serial.println("");
    // Serial.printf("hour: %d: ", hour);
    intToBCD(hour, bits, sizeof(bits));
    pos = 0;
    for (unsigned int i = 0; i < 6; i++)
    {
        dcfBits[i + 29] = bits[i];
        // Serial.print(bits[i]);
        if (1 == bits[i])
        {
            pos++;
        }
    }
    if (pos % 2) // if odd
    {
        dcfBits[35] = 1; //parity
    }

    // Serial.println("");
    // Serial.printf("day: %d: ", day);
    intToBCD(day, bits, sizeof(bits));
    for (unsigned int i = 0; i < 6; i++)
    {
        dcfBits[i + 36] = bits[i];
        // Serial.print(bits[i]);
    }

    // Serial.println("");
    // Serial.print("day of week to do: ");  
    //101 : 5
    dcfBits[42] = 1; //1
    dcfBits[43] = 0; //2
    dcfBits[44] = 1; //4
 


    // Serial.println("");
    // Serial.printf("month: %d: ", month); 
    intToBCD(month, bits, sizeof(bits));
    for (unsigned int i = 0; i < 5; i++)
    {
        dcfBits[i + 45] = bits[i];
        // Serial.print(bits[i]);
    }

    // Serial.println("");
    // Serial.printf("year: %d: ", year); 
    unsigned int yearCent = year - 2000;
    // Serial.printf("year of century: %d: ", yearCent); 
    intToBCD(yearCent, bits, sizeof(bits));
    for (unsigned int i = 0; i < 8; i++)
    {
        dcfBits[i + 50] = bits[i];
        // Serial.print(bits[i]);
    }
    // Serial.println("");

    //calculate parity over bits 36-57
    pos = 0;
    for (unsigned int i = 0; i < 21; i++)
    {
        if (1 == dcfBits[i + 36])
        {
            pos++;
        }
    }
    if (pos % 2) // if odd
    {
        dcfBits[58] = 1; //parity over bits 36-57
    }
    
    dcfBits[59] = 0; //minute mark

    for (unsigned int i = 0; i <= 59; i++)
    {
        Serial.print(dcfBits[i]);
    }
    Serial.println("");
}


void intToBCD(unsigned int number, uint8_t *bitArray, size_t bitArrayLength) {
    if (bitArray == nullptr || bitArrayLength == 0) return;

    // Serial.printf("received number: %d \n", number);
    
    // Calculate the maximum number of BCD digits we can store
    size_t maxDigits = bitArrayLength / 4;
    
    // Clear the bit array
    memset(bitArray, 0, bitArrayLength);
    
    // Convert integer to BCD and store bits in the array
    size_t index = 0;
    while (number > 0 && index < maxDigits) {
        uint8_t bcdDigit = number % 10; // Get the last decimal digit
        number /= 10; // Remove the last digit
        
        // Store the BCD digit as 4 bits in the bitArray
        for (uint8_t bit = 0; bit < 4; bit++) {
            bitArray[index * 4 + bit] = (bcdDigit >> bit) & 0x01;
        }
        index++;
    }
}

void encodeDCF77UnixEpoch(uint8_t *dcfBits) {
    memset(dcfBits, 0, 59);

    dcfBits[0] = 0; // Start of minute marker (always 0)
    Serial.print("civil warning: ");
    for (int i = 1; i <= 9; i++ )
    {
        Serial.printf(" %02d", i);
        dcfBits[i] = 1;
    }
    for (int i = 10; i <= 14; i++ )
    {
        Serial.printf(" %02d", i);
        dcfBits[i] = 0;
    }

    Serial.println("");
    Serial.print("summer time bits: ");
    for (int i = 15; i <= 19; i++ )
    {
        Serial.printf(" %02d", i);
        dcfBits[i] = 0;
    }

    Serial.println("");
    Serial.print("minute: ");  
    dcfBits[20] = 1; //Start of minute
    //000 0000 : 00
    dcfBits[21] = 0; //1
    dcfBits[22] = 0; //2
    dcfBits[23] = 0; //4
    dcfBits[24] = 0; //8
    dcfBits[25] = 0; //10
    dcfBits[26] = 0; //20
    dcfBits[27] = 0; //40

    dcfBits[28] = 0; //parity
 
    for (int i = 20; i <= 28; i++ )
    {
        Serial.printf(" %02d", i);
    }

    Serial.println("");
    Serial.print("hour: ");  
    //00 0000 : 00
    dcfBits[29] = 0; //1
    dcfBits[30] = 0; //2
    dcfBits[31] = 0; //4
    dcfBits[32] = 0; //8
    dcfBits[33] = 0; //10
    dcfBits[34] = 0; //20

    dcfBits[35] = 0; //parity
 
    for (int i = 29; i <= 35; i++ )
    {
        Serial.printf(" %02d", i);
    }

    Serial.println("");
    Serial.print("day: ");  
    //00 0000 : 1
    dcfBits[36] = 1; //1
    dcfBits[37] = 0; //2
    dcfBits[38] = 0; //4
    dcfBits[39] = 0; //8
    dcfBits[40] = 0; //10
    dcfBits[41] = 0; //20
 
    for (int i = 36; i <= 41; i++ )
    {
        Serial.printf(" %02d", i);
    }

    Serial.println("");
    Serial.print("day of week: ");  
    //100 : 4
    dcfBits[42] = 0; //1
    dcfBits[43] = 0; //2
    dcfBits[44] = 1; //4
 
    for (int i = 42; i <= 44; i++ )
    {
        Serial.printf(" %02d", i);
    }

    Serial.println("");
    Serial.print("month: ");  
    //0 0001 : 1
    dcfBits[45] = 1; //1
    dcfBits[46] = 0; //2
    dcfBits[47] = 0; //4
    dcfBits[48] = 0; //8
    dcfBits[49] = 0; //10
 
    for (int i = 45; i <= 49; i++ )
    {
        Serial.printf(" %02d", i);
    }

    Serial.println("");
    Serial.print("year: ");  
    // : 70
    dcfBits[50] = 0; //1
    dcfBits[51] = 0; //2
    dcfBits[52] = 0; //4
    dcfBits[53] = 0; //8
    dcfBits[54] = 1; //10
    dcfBits[55] = 1; //20
    dcfBits[56] = 1; //40
    dcfBits[57] = 0; //80

    dcfBits[58] = 0; //parity over bits 36-57
 
    for (int i = 50; i <= 58; i++ )
    {
        Serial.printf(" %02d", i);
    }
    Serial.println("");

    dcfBits[59] = 0; //minute mark


    for (int i = 0; i <= 59; i++ )
    {
        Serial.printf("%d", dcfBits[i]);
    }
    Serial.println("");

    Serial.println("DCF77 encoding updated.");
}
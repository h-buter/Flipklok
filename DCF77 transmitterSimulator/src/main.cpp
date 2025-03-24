#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "secrets.h"

// WiFi credentials

const char* ssid     = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASS;


void encodeDCF77(uint8_t *dcfBits, int minute, int hour, int day, int month, int year);

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// DCF77 Output pin
const int DCF77_PIN = 2;

void setup() {
    Serial.begin(115200);
    pinMode(DCF77_PIN, OUTPUT);
    digitalWrite(DCF77_PIN, HIGH);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    timeClient.begin();
}

void loop() {
    timeClient.update();
    time_t rawTime = timeClient.getEpochTime();
    struct tm *timeInfo = gmtime(&rawTime);
    
    int minute = timeInfo->tm_min;
    int hour = timeInfo->tm_hour;
    int day = timeInfo->tm_mday;
    int month = timeInfo->tm_mon + 1;
    int year = timeInfo->tm_year + 1900;
    
    Serial.printf("Time: %02d:%02d:%02d %02d-%02d-%04d\n", hour, minute, timeInfo->tm_sec, day, month, year);
    
    uint8_t dcfBits[59]; // DCF77 transmits 59 bits per minute
    encodeDCF77(dcfBits, minute, hour, day, month, year);
    
    Serial.println("Transmitting DCF77 Signal...");
    for (int i = 0; i < 59; i++) {
        digitalWrite(DCF77_PIN, LOW);
        delay(dcfBits[i] ? 200 : 100); // 1-bit: 0.2s LOW, 0-bit: 0.1s LOW
        digitalWrite(DCF77_PIN, HIGH);
        delay(800); // Rest of the second remains HIGH
    }
    delay(1000); // 1-second pause before repeating
}

void encodeDCF77(uint8_t *dcfBits, int minute, int hour, int day, int month, int year) {
    memset(dcfBits, 0, 59);
    
    dcfBits[0] = 0; // Start of minute indicator
    
    for (int i = 0; i < 7; i++) dcfBits[21 + i] = (minute >> i) & 1;
    for (int i = 0; i < 6; i++) dcfBits[29 + i] = (hour >> i) & 1;
    for (int i = 0; i < 6; i++) dcfBits[36 + i] = (day >> i) & 1;
    for (int i = 0; i < 4; i++) dcfBits[45 + i] = (month >> i) & 1;
    for (int i = 0; i < 7; i++) dcfBits[50 + i] = ((year - 2000) >> i) & 1;
}
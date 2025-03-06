#include <Arduino.h>

#include <basic_dcf77.h>
#include <DebugProject.h>

#include <WiFi.h>
#include "time.h"
#include "secrets.h"

const char* ssid     = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASS;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


uint8_t bitArray[DCF77_STRING_SIZE]; //Memory location for received DCF77 bit string
TimeStampDCF77 dcfTime;  //Data type for decoded DCF77 string
int ReceiveDCF77;     //DCF77 receiving status

char buffer[40];      //A cache for a pretty and formatted text output

bool getDcfError();
void connectingWiFi();
void printLocalTime();
void getTimeNTP();

void setup()
{
  //If necessary, activate the debug output for DCF77 in DebugProject.h.
	Serial.begin(115200);
  delay(7000);    //Depending on hardware, it may take some dcfTime until the DCF77 module is ready
  connectingWiFi();
  setupDCF77(2); //set MCU digital input Pin 12 for DCF77
}


void loop() 
{
  bool status = getDcfError();
  Serial.print("Status: ");
  Serial.println(status);
}

bool getDcfError()
{
  bool error = 0;
  getTimeNTP();
  ReceiveDCF77 = receiveDCF77(bitArray,DCF77_STRING_SIZE); //Start receiving a DCF77 string
  
  if(ReceiveDCF77==SUCCESS)
  {
    if(decodeDCF77(bitArray,DCF77_STRING_SIZE,&dcfTime)==SUCCESS)
    {
      snprintf(buffer, sizeof(buffer), "It is now %02d:%02d o'clock %s", dcfTime.hour, dcfTime.minute, 
      (dcfTime.CEST) ? "CEST" : "CET");
      Serial.println(buffer);
      snprintf(buffer, sizeof(buffer), "Today is %02d.%02d.20%02d",dcfTime.day, dcfTime.month, dcfTime.year);
      Serial.println(buffer);
      snprintf(buffer, sizeof(buffer), "Weekday: %02d\n", dcfTime.weekday);
      Serial.println(buffer);
      error = 0;
      if(dcfTime.transmitter_fault!=SUCCESS)
      {
        Serial.println("Either their signal is very noisy, or something is wrong in Germany.");
        error = 1;
      }
      else if(dcfTime.A1)
      {
        Serial.println("CET/CEST Time change is coming up.");
      }
    }
    else
    {
      Serial.println("No doubt, signal unstable, please readjust antenna.");
      error = 1;
    } 
  }
  else if(ReceiveDCF77==ERROR_TIMEOUT)
  {
    Serial.println("\nDCF77 signal unstable, please wait or readjust antenna.");
    error = 1;
  }
  return error;
}

void connectingWiFi()
{
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
}

void getTimeNTP()
{
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}
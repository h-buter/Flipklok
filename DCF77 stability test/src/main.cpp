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


bool getDcfFix();
void connectingWiFi();
void printLocalTime(struct tm time);
struct tm getTimeNTP();
bool isTimeInRange();
double printRunningtime();

struct tm startTime;

void setup()
{
  Serial.begin(115200);

  delay(7000);    //Depending on hardware, it may take some time until the DCF77 module is ready
  connectingWiFi();
  Serial.print("StartTime: ");
  startTime = getTimeNTP();
  printLocalTime(startTime);
  
  setupDCF77(2); //set MCU digital input Pin 12 for DCF77
}

long unsigned int receiveErrors = 0;
long unsigned int timeErrors = 0;
long unsigned int timeSuccessful = 0;
long unsigned int totalDcfMessages = 0;

void loop() 
{
  Serial.print("Current time: ");
  printLocalTime(getTimeNTP());
  Serial.print("Total messages received: ");
  Serial.print(totalDcfMessages);
  Serial.print(", of which : ");
  Serial.print(receiveErrors);
  Serial.print(" where errors, resulting in a total of ");
  Serial.print(totalDcfMessages - receiveErrors);
  Serial.print(" complete messages, of which ");
  Serial.print(timeSuccessful);
  Serial.print(" where the time difference was LESS than 2 minutes, and ");
  Serial.print(timeErrors);
  Serial.print(" where the time difference was MORE than 2 minutes, ");
  double runningTime = printRunningtime();
  if (0 != timeSuccessful)
  {
    Serial.print("This running time should result in: ");
    int totalMessagesBasedOnRunningtime = runningTime / 60;
    Serial.print(totalMessagesBasedOnRunningtime);
    Serial.print(" messages, together with the correct recieved messages this results in a error rate of: ");
    Serial.print(100/totalMessagesBasedOnRunningtime*timeSuccessful);
    Serial.println("%");
  }

  bool error = getDcfFix();
  if (error == 1)
  {
    receiveErrors++;
  }
  else
  {
    if (true == isTimeInRange())
    {
      Serial.println("time difference between NTP and DCF is LESS than 2 min");
      timeSuccessful++;
    }
    else
    {
      Serial.println("time difference between NTP and DCF is MORE than 2 min");
      timeErrors++;
    }
  }
  totalDcfMessages++;
}

bool getDcfFix()
{
  bool error = 0;
  ReceiveDCF77 = receiveDCF77(bitArray, DCF77_STRING_SIZE); //Start receiving a DCF77 string

  if (ReceiveDCF77 == SUCCESS)
  {
    if (decodeDCF77(bitArray, DCF77_STRING_SIZE, &dcfTime) == SUCCESS)
    {
      snprintf(buffer, sizeof(buffer), "It is now %02d:%02d o'clock %s", dcfTime.hour, dcfTime.minute, 
      (dcfTime.CEST) ? "CEST" : "CET");
      Serial.println(buffer);
      snprintf(buffer, sizeof(buffer), "Today is %02d.%02d.20%02d", dcfTime.day, dcfTime.month, dcfTime.year);
      Serial.println(buffer);
      snprintf(buffer, sizeof(buffer), "Weekday: %02d\n", dcfTime.weekday);
      Serial.println(buffer);
      error = 0;
      if (dcfTime.transmitter_fault != SUCCESS)
      {
        Serial.println("Either their signal is very noisy, or something is wrong in Germany.");
        error = 1;
      }
      else if (dcfTime.A1)
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
  else if (ReceiveDCF77 == ERROR_TIMEOUT)
  {
    Serial.println("\nDCF77 signal unstable, please wait or readjust antenna.");
    error = 1;
  }
  return error;
}

void connectingWiFi()
{
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

struct tm getTimeNTP()
{
  struct tm time;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  bool succesNtp = 0;
  while (succesNtp != 1)
  {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if(!getLocalTime(&time)){
      Serial.println("Failed to obtain time, retrying");
    }
    else
    {
      succesNtp = 1;
    }
  }
  return time;
}

void printLocalTime(struct tm time){
  
  if(!getLocalTime(&time)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("NTP time: ");
  Serial.println(&time, "%A, %B %d %Y %H:%M:%S");
}

bool isTimeInRange() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain NTP time");
    return false;
  }
  int ntpMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  int dcfMinutes = dcfTime.hour * 60 + dcfTime.minute;
  int timeDifference = abs(ntpMinutes - dcfMinutes);
  Serial.print("Time difference: ");
  Serial.println(timeDifference);

  return timeDifference <= 2;
}

//Calculated and print the running time of the program
double printRunningtime()
{
  struct tm currentTime = getTimeNTP();
  
  // Convert struct tm to time_t for calculation
  time_t currentTimestamp = mktime(&currentTime);
  time_t startTimestamp = mktime(&startTime);
  
  // Calculate time difference in seconds
  double secondsDiff = difftime(currentTimestamp, startTimestamp);
  
  // Convert seconds difference to hours, minutes, and seconds
  int hours = secondsDiff / 3600;
  int minutes = ((int)secondsDiff % 3600) / 60;
  int seconds = (int)secondsDiff % 60;
  
  Serial.print("Running time: ");
  Serial.print(secondsDiff);
  Serial.print(" seconds total, ");
  Serial.print(hours);
  Serial.print(" hours, ");
  Serial.print(minutes);
  Serial.print(" minutes, ");
  Serial.print(seconds);
  Serial.println(" seconds.");

  return secondsDiff;
}
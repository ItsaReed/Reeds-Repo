/* Test sketch for Adafruit PM2.5 sensor with UART or I2C */

// This code uses the code for a test sketch of the PM.25 sensor with UART
// The code is intended to write to PM levels onto a .csv file
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/wdt.h>

// Digital pin 4 for the MicroSD card port.
#define CS 4

SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

const char* fileName = "S7Data.csv";

void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Adafruit PMSA003I Air Quality Sensor");

  // Wait three seconds for sensor to boot up
  delay(3000);

  pmSerial.begin(9600);

  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(3000);
  }

  Serial.println("PM25 found!");

  // Setup for SD Card:
  File f;

  if(!SD.begin(CS)){
    Serial.println("Error initializing SD card!");
    wdt_enable(WDTO_8S); //Restart after 8 seconds
    while(true); //Wait for restart
  }

  f = SD.open(fileName, FILE_WRITE);
  if(!f){
    Serial.println("Error while creating/opening the file!");
    wdt_enable(WDTO_8S);
    while(true);
  }

  f.close(); // Close when data has been written and open again.

}

void loop() {
  PM25_AQI_Data data;

  Serial.println("AQI reading success");

  //Print PM values:

  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (standard)"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (environmental)"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
  Serial.println(F("---------------------------------------"));
  // The PM2.5 sensor had flipped the values, i.e the variable for 0.3um was for 10um, idk why:
  Serial.print(F("Particles > 10um / 0.1L air:")); Serial.println(data.particles_03um);
  Serial.print(F("Particles > 5um / 0.1L air:")); Serial.println(data.particles_05um);
  Serial.print(F("Particles > 2.5um / 0.1L air:")); Serial.println(data.particles_10um);
  Serial.print(F("Particles > 1.0um / 0.1L air:")); Serial.println(data.particles_25um);
  Serial.print(F("Particles > 0.5um / 0.1L air:")); Serial.println(data.particles_50um);
  Serial.print(F("Particles > 0.3 um / 0.1L air:")); Serial.println(data.particles_100um);
  Serial.println(F("---------------------------------------"));
  Serial.println(F("AQI"));
  Serial.print(F("PM2.5 AQI US: ")); Serial.print(data.aqi_pm25_us);
  Serial.print(F("\tPM10  AQI US: ")); Serial.println(data.aqi_pm100_us);
  Serial.print(F("PM2.5 AQI China: ")); Serial.print(data.aqi_pm25_china);
  Serial.println(F("---------------------------------------"));
  Serial.println();

  // Writing to SD Card:
  int pmData0 = data.particles_03um; // Again, values are flipped, so this is actually values for >10um.
  int pmData1 = data.particles_05um;
  int pmData2 = data.particles_10um; // Particles >2.5um, which is the one we were testing for this study.
  int pmData3 = data.particles_25um;
  int pmData4 = data.particles_50um;
  int pmData5 = data.particles_100um;

  // Checking if file can be written to.
  if(writeToFile(fileName, pmData0, pmData1, pmData2, pmData3, pmData4, pmData5))
    Serial.println("Wrote to file!");
  else
    Serial.println("File write failed!");

  // Checking AQI:
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    
    delay(500);  // try again in a bit
    return;
  }

  delay(60000); // Take air quality every minute.
}

bool writeToFile(const char* path, int data1, int data2, int data3, int data4, int data5, int data6){
  File f;
  f = SD.open(fileName, FILE_WRITE);
  // Failsafe if opening and writing to the file fails:
  if(!f){
    f.close();
    f = SD.open(path, FILE_WRITE);
    if(!f){
      f.close();
      return false;
    }
  }
  // Write data onto .csv file
  f.print(data1);
  f.print(",");
  f.print(data2);
  f.print(",");
  f.print(data3);
  f.print(",");
  f.print(data4);
  f.print(",");
  f.print(data5);
  f.print(",");
  f.print(data6);
  f.print('\n');

  f.close();

  return true;
}




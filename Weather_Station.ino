#define BLYNK_TEMPLATE_ID "TMPL3jOBlKwLI"
#define BLYNK_DEVICE_NAME "Weather Station" 

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "Arduino.h"
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>

double temperature; // parameters
double humidity;
double heat;
double P;
double P0;
double A;
double T;

SFE_BMP180 pressure;

#define ALTITUDE 150.0

/////////////////////////////////UPDATE////////////////////////////////////////////////////
char ssid[] = "Airtel_502"; 
char pass[] = "ninetoone";
///////////////////////////////////////////////////////////////////////////////////////////

char auth[] = "NdBM2Z_m8OyLR5pqFDlFurEv5EtPq4wo";

#define DHTPIN 14 // dht sensor is connected to D5
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE); // initialise dht sensor
BlynkTimer timer;

void sendSensor() // function to read sensor values and send them to Blynk
{
  char status;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          P0 = pressure.sealevel(P,ALTITUDE); //relative sea level pressure in mb 
          A = pressure.altitude(P,P0); //computed altitude
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
  
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  heat = dht.computeHeatIndex(temperature, humidity, false);
  
  Blynk.virtualWrite(V0, temperature); // send all the values to their respective virtual pins
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, heat);
  Blynk.virtualWrite(V3, P);
  Blynk.virtualWrite(V4, P0);
  Blynk.virtualWrite(V5, A);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("REBOOT");
  
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }

  dht.begin();
  delay(1000);
  Blynk.begin(auth, ssid, pass);
  delay(1000);
  timer.setInterval(1000L, sendSensor); // sendSensor function will run every 1000 milliseconds
}

void loop()
{
  Blynk.run();
  timer.run();
}

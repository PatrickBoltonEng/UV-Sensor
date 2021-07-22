/*
 * Project UV-Sensor
 * Description:  Seeed Studio Grove UV Sensor https://www.seeedstudio.com/Grove-UV-Sensor.html
 * Author:  PJB
 * Date: 05-15-21
 */

#include "Particle.h"
#include "math.h"
#include "JsonParserGeneratorRK.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;

int UVSensor = A0;


#define UPDATE_INTERVAL 15000  //1 sec = 1000 millis
#define UV_Sample_Size 255

unsigned long UpdateInterval;
int min_last = 1;

void setup() 
{
  Serial.begin(9600);

  pinMode(UVSensor, INPUT);

  UpdateInterval = millis();
}

void loop()
{
  float UVndx=0.0, UVP=0;

  if ((millis() - UpdateInterval) > UPDATE_INTERVAL)
  {
    getUV(UVndx, UVP);
    Log.info("UVIndex: %f", UVndx);
    Log.info("Illumination Intensity(mW/cm²): %f", UVP);
    
    UpdateInterval=millis();
  }
  
  int min_i=Time.minute();

  if((min_i != min_last)&&(min_i==0||min_i==15||min_i==30||min_i==45)) //only update cloud on the quarter hour one time
  {
    createEventPayload(UVndx, UVP);
    min_last = min_i;
    Log.info("Last Update: %d", min_last);
  }

}

 

void createEventPayload(float &UVndx, float &UVP)
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);
    jw.insertKeyValue("UV Index",UVndx);
    jw.insertKeyValue("Illum Int(mW/m²)", UVP);
  }
  Particle.publish("COO-COO-CACHOO", jw.getBuffer(), PRIVATE);
}

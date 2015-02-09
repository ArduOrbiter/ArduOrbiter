#include <SPI.h>
#include <RF22.h>
#include "I2Cdev.h"
#include "Wire.h"

RF22 rf22;

int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
                   //the resolution is 10 mV / degree centigrade with a
                   //500 mV offset to allow for negative temperatures

void setup() 
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

void loop()
{
        int reading = analogRead(sensorPin);  
 
        // converting that reading to voltage, for 3.3v arduino use 3.3
        float voltage = reading * 5.0;
             voltage /= 1024.0; 
 
        // now print out the temperature
        float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                      //to degrees ((voltage - 500mV) times 100)
       
        String idCode;
        String comsTrans;
        
        idCode = "M6IAU, ";

        comsTrans = idCode + temperatureC;
        
        Serial.println(comsTrans);

        Serial.println("Sending Data...");
           
//        uint8_t data[] = idCode + temperatureC;
        rf22.send(comsTrans);
        
        delay(2000);
}

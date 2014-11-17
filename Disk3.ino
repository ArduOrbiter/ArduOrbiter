/* =========================================================================
    This is the code for ArduOrbiter V0.3.1. It isn't as scarey as it looks - 
    I'll try to explain the code below the best I can.
    
    If I have missed anything out, please email me at
    'arduorbiter@gmail.com'.
    
    ***Please note that I did not create ALL of the code.***
 * ========================================================================= */


/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include "I2Cdev.h"
#include "Wire.h"

// Power System

#include "MAX17043.h"
MAX17043 batteryMonitor;

float comsTrans;

int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
                   //the resolution is 10 mV / degree centigrade with a
                   //500 mV offset to allow for negative temperatures

void setup() {

    Serial.begin(9600);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // I'm running it at 9600 because, although it's slower, it uses less power.
    // A too high baud rate can cause issues on the Nano.

}

void loop() {
     
        int reading = analogRead(sensorPin);  
 
        // converting that reading to voltage, for 3.3v arduino use 3.3
        float voltage = reading * 5.0;
             voltage /= 1024.0; 
 
        // now print out the temperature
        float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                      //to degrees ((voltage - 500mV) times 100)

        // Power System
        float cellVoltage = batteryMonitor.getVCell();
        float stateOfCharge = batteryMonitor.getSoC();
        
        // Output
        Serial.print("M6IAU,");
        Serial.print(temperatureC);
        Serial.print(",");
        Serial.print(cellVoltage, 4);
        Serial.print(",");
        Serial.print(stateOfCharge);
        

        delay(5000); // Delay to allow processing time on ground and to preserve power on satellite

    }

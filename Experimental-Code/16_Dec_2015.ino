#include <SPI.h>
#include <RF22.h>
#include <EEPROM.h>

RF22 rf22;

// EEPROM Settings Addresses - the numbers are the EEPROM address
int setRadio = 0; //Radio Active
int LPM = 1; //LPM (Low Power Mode)

// Radio Setup
char radioStatus = 0;
char rData[30];
String command = "";

// Reset Setup
int rstStatus = 0;

int mainSysDelay;

void setup()
{
  Serial.begin(9600); // !!!! REMOVE BEFORE FLIGHT - Remove all Serial.println() !!!!

  pinMode(9, INPUT);    // Just to be clear, as default is INPUT. Not really needed.
  digitalWrite(9, LOW); // Prime it, but does not actually set output.

  if (EEPROM.read(setRadio) == 1) {
    radioStatus = 1;
  } else {
    radioStatus = 2;
  }

  if (EEPROM.read(LPM) == 1) { // If LPM == 1, then Low Power Mode is active
    mainSysDelay = 10000;
  } else {
    mainSysDelay = 1000;
  }

  Serial.println("");
  Serial.println("###SYSTEM START###");

  if (!rf22.init()) {
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  }
}

void loop()
{
  while (radioStatus == 1) // While the radio is allowed to transmit
  {
    delay(mainSysDelay);

    int wADC; // Taken from http://playground.arduino.cc/Main/InternalTemperatureSensor
    double dTempC;
    
    ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
    ADCSRA |= _BV(ADEN);
    
    delay(20);
    
    ADCSRA |= _BV(ADSC);
    
    while (bit_is_set(ADCSRA,ADSC));
    
    wADC = ADCW;
    
    dTempC = (wADC - 324.31 ) / 1.22;

    int tempC = (int) dTempC;

    int rLPM = EEPROM.read(LPM);

    sprintf(rData, "M6IAU.S.C.G!%d@%d#", rLPM, tempC); // Sends radio data
    rf22.send((uint8_t *)rData, sizeof(rData));
    Serial.println(rData);

    rf22.waitPacketSent(); // Wait for downlink to be send

    uint8_t buf[RF22_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf22.waitAvailableTimeout(500))
    {
      if (rf22.recv(buf, &len))
      {

        // Satellite uplink commands
        command = (String((char*)buf)); // Verifies uplink from Earth and executes command

        Serial.println(command);
        

        String strCommand = command;
        strCommand.remove(0, 8);
        int command = strCommand.toInt();

        switch (command) {
          case 1:
            Serial.println("Ground Connected! ID Code '1' - Staying Active!");
            

            break;
          case 2:
            Serial.println("Ground Connected! ID Code '2' - Radio transmissions disabled!");
            

            radioStatus = 2;
            EEPROM.write(setRadio, int(0));

            Serial.println(EEPROM.read(setRadio));
            
            Serial.println(radioStatus);
            

            delay(3000);

            break;
          case 3:
            EEPROM.write(setRadio, int(1));
            radioStatus = 1;

            Serial.println("Ground Connected! ID Code '3' - Reactivated Comms!");
            

            delay(1000);

            break;
          case 4:

            break;
          case 5:
            Serial.println("Ground Connected! ID Code '5' - System ultra-low power mode initialized!");
            

            mainSysDelay = 10000;
            EEPROM.write(LPM, int(1));

            Serial.println("Set time to 10 seconds");
            

            delay(1000);

            break;
          case 6:
            Serial.println("Ground Connected! ID Code '6' - System set to nominal power usage!");
            

            mainSysDelay = 1000;
            EEPROM.write(LPM, int(0));

            Serial.println("Set time to 10 seconds");
            

            delay(1000);

            break;
          default:
            Serial.println("Ground Connected - maybe - Staying Active!");
            

            break;
        }
      }
    }
  }

  while (radioStatus == 2)
  {
    uint8_t buf[RF22_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rstStatus == 1)
    {
      pinMode(9, OUTPUT);
    }

    if (rf22.waitAvailableTimeout(500))
    {
      if (rf22.recv(buf, &len))
      {
        if (String((char*)buf) == "M6IAU.G.3")
        {
          radioStatus = 1;
          EEPROM.write(setRadio, int(1));
        }
      }
    } else {
      Serial.println("Radio Transmission Offline");
      
      Serial.println(EEPROM.read(setRadio));
      
      Serial.println(radioStatus);
      

      delay(2000);
    }
  }
}

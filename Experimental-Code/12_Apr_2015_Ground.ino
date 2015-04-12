#include <SPI.h>
#include <RF22.h>

RF22 rf22;

int value = 49;

void setup()
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop()
{

  if (Serial.available() > 0) {
    value = Serial.read();

    Serial.println(value, DEC);
  }

  //  Serial.println("rf22 avbl");
  if (rf22.available())
  {
    //    Serial.println("rf22 99 avbl");

    uint8_t buf[RF22_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (value == 51)
    {
      uint8_t data[] = "M6IAU.G.3";
      rf22.send(data, sizeof(data));
      rf22.waitPacketSent();
  
      value = 49;
  
      delay(2000);
    }
    

    //    Serial.println("check is recv");
    if (rf22.recv(buf, &len))
    {

      //if (String((char*)buf) == "M6IAU.S.C.G")
      //{

        Serial.println(String((char*)buf));
        
        
        if (value == 49)
        {
          uint8_t data[] = "M6IAU.G.1";
          rf22.send(data, sizeof(data));
          rf22.waitPacketSent();

          delay(2000);
        }

        if (value == 50)
        {
          uint8_t data[] = "M6IAU.G.2";
          rf22.send(data, sizeof(data));
          rf22.waitPacketSent();

          delay(2000);
        }

      //} else {
        //Serial.print("Data Corrupt/Failure! ");
        //Serial.println((char*)buf);
        //Serial.println("");
      //}
    } // End check receive




  } else {
    //   Serial.println("RF22 n/avbl");
  }// End RF22


} // End program

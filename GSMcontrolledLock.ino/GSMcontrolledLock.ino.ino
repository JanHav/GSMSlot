/*
GPRS Loop Handle

This sketch is used to test seeeduino GPRS_Shield's call answering and 
reading SMS function.To make it work, you should insert SIM card
to Seeeduino GPRS Shield,enjoy it!

create on 2015/05/14, version: 1.0
by lawliet.zou(lawliet.zou@gmail.com)
*/
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h>

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600

#define MESSAGE_LENGTH 20

char gprsBuffer[64];
int i = 0;
char *s = NULL;
int inComing = 0;
char phone[16];
char datetime[24];
int iServoPosition;
Servo xservo; //create xservo object

GPRS gprs(PIN_TX,PIN_RX,BAUDRATE);

void setup() {
  gprs.checkPowerUp();
  Serial.begin(9600);
  while(!gprs.init()) {
      Serial.print("init error\r\n");
      delay(1000);
  }
  
  while(!gprs.isNetworkRegistered())
  {
    delay(1000);
    Serial.println("Network has not registered yet!");
  }

  delay(3000);  
  Serial.println("Init Success, please call or send SMS message to me!");
  xservo.attach(9);         //The PWM control of servo 1 is controlled by PIN 9 of the arduino UNO

}

void loop() {
    if(gprs.readable()) {
       inComing = 1;
    }
    else { 
        delay(100);
    }
   
   if(inComing){
      sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
      //Serial.print(gprsBuffer);
      
      if(NULL != strstr(gprsBuffer,"RING")) {
          gprs.answer();
      }else if(NULL != (s = strstr(gprsBuffer,"+CMTI: \"SM\""))) { //SMS: $$+CMTI: "SM",24$$
          char message[MESSAGE_LENGTH];
          int messageIndex = atoi(s+12);
          gprs.readSMS(messageIndex, message,MESSAGE_LENGTH, phone, datetime);
          if (strcmp(phone, "+32471070354") == 0) //With the function strcmp (stringcomparison) we compare the data stored in the variable phone with the desired telephone number
          {
            if (strcmp(message, "OPEN") == 0)
            {
              iServoPosition = 20;
              Serial.print("Door open");
            }
            else if (strcmp(message, "CLOSE") == 0)
            {
              Serial.print("Door close");
              iServoPosition = 180;
            }
            else
            {
              Serial.print("Message data was invalid.");
            }
          }
          /*Serial.print("Recv Message: ");
          Serial.println(message);
          Serial.print("From number: ");
          Serial.println(phone);
          Serial.print("Datetime: ");
          Serial.println(datetime);   */
     }
     sim900_clean_buffer(gprsBuffer,32);  
     inComing = 0;
   }
   //Servomotor control
   xservo.write(iServoPosition);
}

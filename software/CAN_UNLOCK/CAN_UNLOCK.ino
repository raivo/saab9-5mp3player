// SAAB 9-5 stereo project
//
// Sends unlock code
// 
// CAN-BUS Shield, Arduino UNO
// 
// ITT Group
// 20.02.2015
// ver 0.1
//

#include <TimerOne.h>                          
#include <SPI.h>
#include "mcp_can.h"

#define TIMER_US 1000                         // 100mS set timer duration in microseconds 

// DECK PIN setup:
const int Code1 =             A5;  // For old radio
const int Code2 =             A4;  // For new radio

// CAN shield PIN setup:
MCP_CAN CAN(10); // Set CS to pin 10

unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];


unsigned char code_1[8] = {0, 0, 0, 0, 0x90, 0x51, 0x83, 0}; // Old
unsigned char code_2[8] = {0, 0, 0, 0, 0x95, 0x19, 0x12, 0}; // New

int enableState = 0;


void CAN_setup()
{
START_INIT:

    if(CAN_OK == CAN.begin(CAN_47K62BPS))                   // init can bus : baudrate = 47.62k
    {
        Serial.println("CAN BUS init ok!");
    }
    else
    {
        Serial.println("CAN BUS init fail");
        Serial.println("Init CAN BUS again");
        delay(100);
        goto START_INIT;
    }
    
    attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt    

}

void setup() 
{
 
  pinMode(Code1, INPUT); 
  pinMode(Code2, INPUT); 

  Timer1.initialize(TIMER_US);                  // Initialise timer 1
  Timer1.attachInterrupt( timer1Isr );           // attach the ISR routine here
  
  Serial.begin(115200); 
 
  CAN_setup();
}

void loop()
{
  
  if (digitalRead(Code1)) 
  {
         CAN.sendMsgBuf(0x4A0, 0, 8, code_1); 
         Serial.println("Code1");         
  }     
  else 
  {
         CAN.sendMsgBuf(0x4A0, 0, 8, code_2);
         Serial.println("Code2");         
  }  
  delay(1000); 
  
}

void MCP2515_ISR()
{
     Flag_Recv = 1;
}

// --------------------------
// timerIsr() 100 milli second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timer1Isr()
{       
    if(Flag_Recv)                   // check if get data
    {    
        Flag_Recv = 0;                // clear flag
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        if(CAN.getCanId() == 0x290)
        {
        }
        
    }
  
}









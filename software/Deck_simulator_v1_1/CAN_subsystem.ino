//------------------------------------------------------------------------------
// MCP2515 kontrolleril põhinev CAN alamsüsteem. Tegeleb SAABi roolinuppude ja 
// SID paneeli nuppudelt saadavate andmete töötlemisega, SID-i helisignaalide 
// saatmisega ja vajadusel nupuvajutuste tekitamisega.
//
// CAN_message_handler-it tuleb regulaarselt välja kutsuda. Kui CAN_setup 
// funktsiooni pole välja kutsutud, siis tehakse seda automaatselt. Sama tehakse 
// siis, kui on tekkinud mõni viga.
//
// CAN_setup seadistab MCP2515 viigu katkestuse, mis tekib siis, kui on saabunud 
// andmeid. Funktsioon seadistab veel MCP2515 filtri, et vähendada ebaolulist 
// liiklust.
//
// Riistvara: ITT AVR CAN Board
// 
// ITT Group
// 25.05.2015
// ver 1.0
//------------------------------------------------------------------------------

#include <SPI.h>
#include "mcp_can.h"

// CAN subsystem PIN setup:
MCP_CAN CAN(10); // Set CS to pin 10

#define NXT 0x04 //Autopeal
//#define NXT 0x40 //Vana SID
#define SEEK_RIGHT 0x10
#define SEEK_LEFT 0x08

#define SET 0x40 // RPT
#define CLR 0x80 // EQ

//
// CAN Subsystem variables
//
unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];

int sid_message = 0;
int sid_count = 0;
int sends_count = 0;   
int send_text = 0;

int CAN_reset_seek_left_flag;
int Reset_seek_left;
unsigned long CAN_reset_seek_left_time;
int CAN_reset_seek_right_flag;
int Reset_seek_right;
unsigned long CAN_reset_seek_right_time;

unsigned long CAN_Watchdog = 0;

// ISR entry time
volatile uint32_t tIsr = 0;

//
// CAN messages to send
//
unsigned char stmp_seek_right1[8] = {0, 0, 0, 0, SEEK_RIGHT, 0, 0, 0};
unsigned char stmp_seek_right2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char stmp_seek_left1[8] = {0, 0, 0, 0, SEEK_LEFT, 0, 0, 0};
unsigned char stmp_seek_left2[8] = {0, 0, 0, 0, 0, 0, 0, 0};

unsigned char request_beep[8] = {80, 04, 0, 0, 0, 0, 0, 0};
unsigned char request_beep_l[8] = {80, 91, 0, 0, 0, 0, 0, 0};

//
// Setup
//
void CAN_setup()
{
  noInterrupts();
START_INIT:

    if(CAN_OK == CAN.begin(CAN_47K62BPS))                   // init can bus : baudrate = 47.62k
    {
        #ifdef DEBUG
        Serial.println("CAN BUS init ok!");
        #endif
    }
    else
    {
        #ifdef DEBUG
        Serial.println("CAN BUS init fail");
        Serial.println("Init CAN BUS again");
        #endif
        delay(100);
        goto START_INIT;
    }
    
    //attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
    
    /*
     * set mask, set both the mask to 0x3ff
     */
    CAN.init_Mask(0, 0, 0x3ff);                         // there are 2 mask in mcp2515, you need to set both of them
    CAN.init_Mask(1, 0, 0x3ff);
    
    CAN.init_Filt(0, 0, 0x290);
    interrupts();
}  

//
// Interrupt
//
void MCP2515_ISR()
{
    // On AVR this forces compiler to save registers r18-r31.
    CH_IRQ_PROLOGUE();
    /* IRQ handling code, preemptable if the architecture supports it.*/
    
    // Only ISR processing is to save time
    tIsr = micros();
    
    chSysLockFromIsr();
    /* Invocation of some I-Class system APIs, never preemptable.*/
    
    Flag_Recv = 1;
    
    // Perform rescheduling if required.
    CH_IRQ_EPILOGUE();  
}

//
// Handler
//
void CAN_message_handler()
{  
  if(CAN_MSGAVAIL == CAN.checkReceive())                   // check if get data
  {   
        CAN_Watchdog = millis();         
        Flag_Recv = 0;                // clear flag
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        if(CAN.getCanId() == 0x290)
        {
            // Short steering wheel buttons press
            switch (buf[2]) 
            {
              case NXT:                
                break;
              case SEEK_RIGHT:
                {
                  #ifdef DEBUG
                  Serial.println("CAN_SEEK_>>"); 
                  #endif
                  music_player_next();
                  break;
                }
                
              case SEEK_LEFT:
                {
                  #ifdef DEBUG
                  Serial.println("CAN_SEEK_<<");
                  #endif
                  music_player_previous();
                  break;
                }
              default: 
                break;
            }
           
           // Short steering wheel buttons press
            switch (buf[3]) 
            {     
              case CLR:
                {
                  #ifdef DEBUG
                  Serial.println("CLR");
                  #endif
                  music_player_eq();
                  break;
                }
              case SET:
                {
                  #ifdef DEBUG
                  Serial.println("SET");
                  #endif
                  music_player_repeat();                
                  break;
                }
              default: 
                break;
            }
            
            // Long steering wheel buttons press
            if (buf[4] == NXT)
            { 
              music_player_reset_counter(true);       
              if(!mute)
              {                
                mute = 1;
                IR_send_MUTE();
              }  
            } 
            else
            {
              music_player_reset_counter(false);
            }
            
//            // Long steering wheel buttons press
//            if (buf[4] == SEEK_RIGHT || buf[4] == SEEK_LEFT)
//            {
//              //music_player_wind(true);
//            } 
//            else
//            {
//              //music_player_wind(false);
//            }            
        }      
    }
    
    if(CAN_reset_seek_left_flag)
    {
        CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_left1);
        chThdSleepMilliseconds(90);
        CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_left2);
        CAN_reset_seek_left_flag = 0;
    } 
   
    if(CAN_reset_seek_right_flag)
    { 
        CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_right1);
        chThdSleepMilliseconds(90);
        CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_right2);
        CAN_reset_seek_right_flag = 0;
    } 
    
  if( millis()- CAN_Watchdog > 1000)
  {
    CAN_Watchdog = millis();
    //CAN_setup();
  }
}

void CAN_reset_seek_left()
{
  CAN_reset_seek_left_flag = 1;
  //Serial.println("CAN send prev");
  //CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_left1);
  //delay(1000);
  //CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_left2);
}

void CAN_reset_seek_right()
{
  CAN_reset_seek_right_flag = 1;
  //Serial.println("CAN send next");
  //CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_right1);
  //delay(1000);
  //CAN.sendMsgBuf(0x290, 0, 8, stmp_seek_right2);
}

void CAN_request_beep()
{
  CAN.sendMsgBuf(0x430, 0, 8, request_beep); 
}



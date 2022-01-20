// SAAB 9-5 stereo project
//
// Reads the state of the buttons on the steering wheel over CAN-bus and drive MP3 board. Receive data with interrupt mod. Full simulator for cassete system.
// 
// Riistvara: ITT AVR CAN Board
// 
// ITT Group
// 25.05.2015
// ver 1.1
//

#include <ChibiOS_AVR.h>
#include <TimerOne.h>

//------------------------------------------------------------------------------
//Debug mode. Serial is on
//#define DEBUG 
//------------------------------------------------------------------------------

#define TIMER_US 1000                         // 100mS set timer duration in microseconds 

#define NEW_DECK 1
#define OLD_DECK 0

//------------------------------------------------------------------------------
// Music player subsystem global variables:
//------------------------------------------------------------------------------
unsigned long last_nxt_press_time = 0;
int nxt_flag = 0;
int track_number;
int mute = 0;

//------------------------------------------------------------------------------
// DECK simulator subsystem global variables:
//------------------------------------------------------------------------------
int enableState = 0;

//------------------------------------------------------------------------------
// Version selection switch pin setup
//------------------------------------------------------------------------------
const int ver_switch_supply_pin = A0;  // 5V
const int ver_switch_pin =        A1;  // Signal
const int ver_switch_gnd_pin =    A2;  // GND

int deck_version = 0;

//------------------------------------------------------------------------------
// WorkerThread - run all the system
// 100 byte stack beyond task switch and interrupt needs
//------------------------------------------------------------------------------
static WORKING_AREA(waWorkerThread, 100);

static msg_t WorkerThread(void *arg) 
{
  while (1) 
  {
    //chThdSleepMilliseconds(1); 
    CAN_message_handler();
    mp3_handler();
    SID_CAN_text_handler();    
  }
}
//------------------------------------------------------------------------------
// DeckSimulatorThread - high priority for all deck logic
//------------------------------------------------------------------------------
static WORKING_AREA(waDeckSimulatorThread, 100);

static msg_t DeckSimulatorThread(void *arg) 
{ 
  while (1) 
  {
    if(deck_version)
    {
      new_deck_simulator_handler();
    }  
    else
    {
      old_deck_simulator_handler();
    } 
        
    ir_handler();  
  }
  return 0;
}

//------------------------------------------------------------------------------
void setup() 
{   
  Serial.begin(115200);

  Serial.println("START12");

  // Set version switch pins
  pinMode(ver_switch_supply_pin, OUTPUT); 
  pinMode(ver_switch_gnd_pin, OUTPUT); 
  pinMode(ver_switch_pin, INPUT);  
    
  digitalWrite(ver_switch_supply_pin, HIGH); 
  digitalWrite(ver_switch_gnd_pin, LOW); 

  if(digitalRead(ver_switch_pin))
  {
    Serial.println("Selected NEW deck system");
    deck_version = 1;
    new_deck_simulator_setup();
  }
  else
  {
    Serial.println("Selected OLD deck system");
    deck_version = 0;
    old_deck_simulator_setup();
  }  
  
  ir_subsystem_setup();
   
  music_player_setup();
  
  Timer1.initialize(TIMER_US);                  // Initialise timer 1
  Timer1.attachInterrupt( timer1Isr );          // attach the ISR routine here
  CAN_setup();
  chBegin(mainThread);   
  
  // chBegin never returns, main thread continues with mainThread()
  while(1) {}
}
//------------------------------------------------------------------------------
// main thread runs at NORMALPRIO
//------------------------------------------------------------------------------
void mainThread() 
{
  // start blink thread
  chThdCreateStatic(waDeckSimulatorThread, sizeof(waDeckSimulatorThread),
                          NORMALPRIO + 2, DeckSimulatorThread, NULL);

  // start print thread
  chThdCreateStatic(waWorkerThread, sizeof(waWorkerThread),
                          NORMALPRIO + 1, WorkerThread, NULL);
                          

  while (1) 
  {
    // Do nothing
  }
}
//------------------------------------------------------------------------------
void loop() 
{
 // not used
}

//------------------------------------------------------------------------------
// timer1Isr - PWM signals generation for deck simulator
//------------------------------------------------------------------------------
void timer1Isr()
{   
  if(deck_version)
  {
    new_deck_simulator_PWM_handler();
  }
  else
  {
    old_deck_simulator_PWM_handler();
  }
      
}





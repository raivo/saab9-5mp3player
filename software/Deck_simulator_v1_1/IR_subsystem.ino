//------------------------------------------------------------------------------
// IR_subsystem alasüsteem. Simuleerib MP3 mängija IR puldi klahvivajutusi, et 
// lugusid valida ja muid toiminguid teha. Suudab valida kuni kolmekohalisi 
// lugude numbreid.
//
// ir_handler-it tuleb regulaarselt välja kutsuda, et pakkettide 
// koostamine töötaks vastavalt vabale ajale.
//
// Riistvara: ITT AVR CAN Board
// 
// ITT Group
// 25.05.2015
// ver 1.0
//------------------------------------------------------------------------------

#include <IRremote.h>

IRsend irsend;

//
// IR simulator subsystem variables:
//
#define BUTTON_POWER        0xFFA25D      // 1
#define BUTTON_MODE         0xFF629D      // 2
#define BUTTON_MUTE         0xFFE21D      // 3
#define BUTTON_PLAY_STOP    0xFF22DD      // 4
#define BUTTON_PREVIOUS     0xFF02FD      // 5
#define BUTTON_NEXT         0xFFC23D      // 6
#define BUTTON_EQ           0xFFE01F      // 7
#define BUTTON_VOL_DOWN     0xFFA857      // 8
#define BUTTON_VOL_UP       0xFF906F      // 9
#define BUTTON_0            0xFF6897     // 10
#define BUTTON_RPT          0xFF9867     // 11
#define BUTTON_CLOCK        0xFFB04F     // 12
#define BUTTON_1            0xFF30CF     // 13
#define BUTTON_2            0xFF18E7     // 14
#define BUTTON_3            0xFF7A85     // 15
#define BUTTON_4            0xFF10EF     // 16
#define BUTTON_5            0xFF38C7     // 17
#define BUTTON_6            0xFF5AA5     // 18
#define BUTTON_7            0xFF42BD     // 19
#define BUTTON_8            0xFF4AB5     // 20
#define BUTTON_9            0xFF52AD     // 21

//
// IR Subsystem variables
//
unsigned int num=0;
unsigned int dig;
int digits = 0;
int ir_flag_first = 0;
int ir_digit_flag_first = 0;
int track_on_ir = 0;
int digits_buf[3];
int nxt_flag_2 = 0;
int track_on_ir_last = 0;
unsigned long last_IR_packet_time;

int IR_send_EQ_flag = 0;
int IR_send_RPT_flag = 0;
int IR_send_NEXT_flag = 0;
int IR_send_PREVIOUS_flag = 0;
int IR_send_MUTE_flag = 0;

//
// Setup
//
void ir_subsystem_setup()
{
  pinMode(A3, INPUT);
}

// Handler. Should be called every 100 ms. Run IR subsysem thread.
// Main function: Dial digits 1 to 999
void ir_handler()
{ 
  if(IR_send_EQ_flag)
  {
    IR_send_EQ_flag = 0;
    noInterrupts();
    irsend.sendNEC(BUTTON_EQ, 32);
    interrupts();
  }
  
  if(IR_send_RPT_flag)
  {
    IR_send_RPT_flag = 0;
    noInterrupts();
    irsend.sendNEC(BUTTON_RPT, 32);
    interrupts();
  }
  
  if(IR_send_NEXT_flag)
  {
    IR_send_NEXT_flag = 0;
    noInterrupts();
    irsend.sendNEC(BUTTON_NEXT, 32);
    interrupts();
  }
  
  if(IR_send_PREVIOUS_flag)
  {
    IR_send_PREVIOUS_flag = 0;
    noInterrupts();
    irsend.sendNEC(BUTTON_PREVIOUS, 32);
    interrupts();
  }
  
  if(IR_send_MUTE_flag)
  {
    IR_send_MUTE_flag = 0;
    //irsend.sendNEC(BUTTON_MUTE, 32);
  }
  
  if(nxt_flag && ((millis() - last_nxt_press_time) > 800))
  { 
    nxt_flag = 0;   
    nxt_flag_2 = 0;
    Serial.println("NXT vabaks");
  }
  else
  {
    nxt_flag_2 = 1;
  }
  
  if(!ir_flag_first && (track_on_ir_last != track_number)&& !nxt_flag_2)
  {
    track_on_ir = track_number;
  }
  
  if (track_on_ir)
  {       
    if(ir_flag_first == 0)
    {             
      ir_flag_first = 1;
      dig = countDigits(track_on_ir);
      num = track_on_ir;
      int arr[dig];
      
      if(dig <= 3)
      {                   
        digits = dig;
        //int arr[dig];
        while (dig--) 
        {
          digits_buf[dig] = num % 10;
          num /= 10;
        }
        
      }      
      else
      {
        #ifdef DEBUG
        Serial.println("ERR: digits count < 3");
        #endif        
      }
    }
    
    if(ir_flag_first == 1 && ir_digit_flag_first == 0 && digits >= 1)
    {                
      ir_digit_flag_first = 1;
      send_digit(digits_buf[0]);
      last_IR_packet_time = millis();      
    }    
    if(ir_flag_first == 1 && ir_digit_flag_first == 1 && digits >= 2 && (millis() - last_IR_packet_time) > 50)
    {
      ir_digit_flag_first = 2;
      send_digit(digits_buf[1]);
      last_IR_packet_time = millis();
    }        
    if(ir_flag_first == 1 && ir_digit_flag_first == 2 && digits >= 3 && (millis() - last_IR_packet_time) > 50)
    {
      ir_digit_flag_first = 3;
      send_digit(digits_buf[2]);   
      last_IR_packet_time = millis();
    }
    
    //Pikk viide on vajalik, et oodata, kuniks MP3 mängija oma loo valimise protsessi lõpuni viib.
    if(ir_digit_flag_first == digits && (millis() - last_IR_packet_time) > 2500)
    {      
      ir_flag_first = 0;
      ir_digit_flag_first = 0;
      track_on_ir_last = track_on_ir;
      track_on_ir = 0;
      //chThdSleepMilliseconds(10);       
      
      //chThdSleepMilliseconds(10);       
      send_SID_CAN_song(digits_buf[0], digits_buf[1], digits_buf[2], digits);
      
      IR_send_MUTE();
      
      mute = 0;      
    }   
  }    
}

// Counts the digits in an integer
int countDigits(int num)
{
  int count=0;
  while(num){
    num=num/10;
    count++;
  }
  return count;
}

// Send IR digit
void send_digit(int digit)
{
  noInterrupts();
  switch (digit) 
  {
    case 0:
      irsend.sendNEC(BUTTON_0, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_0");
      #endif
      break;
    case 1:
      irsend.sendNEC(BUTTON_1, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_1");
      #endif
      break;
    case 2:
      irsend.sendNEC(BUTTON_2, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_2");
      #endif
      break;
    case 3:
      irsend.sendNEC(BUTTON_3, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_3");
      #endif
      break;
    case 4:
      irsend.sendNEC(BUTTON_4, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_4");
      #endif
      break;
    case 5:
      irsend.sendNEC(BUTTON_5, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_5");
      #endif
      break;
    case 6:
      irsend.sendNEC(BUTTON_6, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_6");
      #endif
      break;
    case 7:
      irsend.sendNEC(BUTTON_7, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_7");
      #endif
      break;
    case 8:
      irsend.sendNEC(BUTTON_8, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_8");
      #endif
      break;
    case 9:
      irsend.sendNEC(BUTTON_9, 32);
      #ifdef DEBUG
      Serial.println("BUTTON_9");
      #endif
      break;      
    default:
      #ifdef DEBUG
      Serial.println("ERR: Wrong digit");
      #endif
      break;
  }
  interrupts();
}

void IR_send_next()
{
  IR_send_NEXT_flag = 1;  
}

void IR_send_previous()
{
  IR_send_PREVIOUS_flag = 1;  
}

void IR_send_repeat()
{
  //irsend.sendNEC_Repeat();
}

void IR_send_EQ()
{
  IR_send_EQ_flag = 1;
  
}

void IR_send_RPT()
{
  IR_send_RPT_flag = 1;

}

void IR_send_MUTE()
{
  noInterrupts();
  chThdSleepMilliseconds(10);
  irsend.sendNEC(BUTTON_MUTE, 32);
  chThdSleepMilliseconds(10);
  interrupts();  
}

#define CAN_OFF_DELAY   600

// Music player PIN setup:
#define next_pin A0
#define prev_pin A1
#define player_power_pin A0

//
// CAN Subsystem variables
//
int tr = 1;
unsigned long seek_firstTime = 0;
int seek_first_flag = 0;
int previous = 0;
unsigned long firstTime;   // how long since the button was first pressed 
int nex_pin_off_state = 1;
int prev_pin_off_state = 1;
int prev_pin_can_time = 0;
int nex_pin_can_time = 0;
int next_pin_state = 0;
int prev_pin_state = 0;
int seek_long = 0;
int src_long = 0;
int repeat_state = 0;
int eq_state = 1;

//
// Setup
//
void music_player_setup() 
{ 
  pinMode(player_power_pin, OUTPUT);  
  digitalWrite(player_power_pin, HIGH);  
}

//
// Handler
//
void mp3_handler()
{
  if(next_pin_state)
  {     
    IR_send_next();       
    
    chThdSleepMilliseconds(400);    
    next_pin_state = 0;      
    CAN_reset_seek_right();     
  }
  
  if(prev_pin_state)
  { 
    IR_send_previous();    
    chThdSleepMilliseconds(400);   
    prev_pin_state = 0;      
    CAN_reset_seek_left();   
  }
  
  
  if (src_long && previous == 0 && millis()- firstTime > 500)
  {
    firstTime = millis();    // if the buttons becomes press remember the time     
  }
  
//  if (seek_long)
//  {
//     if(!seek_first_flag)
//     {
//       seek_firstTime = millis();
//       seek_first_flag = 1;
//     }      
//     else if((millis() - seek_firstTime) > 80)
//     {          
//         //IR_send_repeat();
//         seek_first_flag = 0;     
//     }     
//  }
//  else
//  {
//    seek_firstTime = 0;
//    seek_first_flag = 0;
//  }
  
  
  if (src_long == 0 && previous == 1)
  {
    if((millis() - firstTime) > 1000)
    {
      #ifdef DEBUG
      Serial.println("Algusesse");
      #endif
      mute = 0;
      track_number = tr = 1;
      nxt_flag = 1;
      
      last_nxt_press_time = millis();
      CAN_request_beep();    
    }  
    else if((millis() - firstTime) > 2)
    {
      #ifdef DEBUG
      Serial.println("CAN_NXT"); 
      #endif     
      
      nxt_flag = 1;
      if(tr == 1)
      {
        track_number = tr = 10;                  
      }
      else
      {
        track_number = tr;                  
      }
      tr +=10;
      
      //Serial.println("CAN_NXT_SHORT"); // Do nothing. Button not used.
      // Serial.println(track_number); // Do nothing. Button not used. 
      last_nxt_press_time = millis();
      //Serial.println(last_nxt_press_time);
      nxt_flag = 1;           
    }
    
  }                                

  previous = src_long;
}

void music_player_ON()
{
  digitalWrite(player_power_pin, LOW);
  //send_SID_CAN_text(2);
}

void music_player_OFF()
{
  digitalWrite(player_power_pin, HIGH);
  eq_state = 1;
  repeat_state = 0;
}

void music_player_next()
{
  if(enableState)
  {               
    next_pin_state = 1;
  }
}

void music_player_previous()
{
  if(enableState)
  {                
    prev_pin_state = 1; 
  }
}

void music_player_eq()
{
  if(enableState)
  {                 
     CAN.sendMsgBuf(0x430, 0, 8, request_beep);
     
     
     if(eq_state == 1)
     {
       eq_state = 2;
       send_SID_CAN_text(5);
     }
     else if (eq_state == 2)
     {
       eq_state = 3;
       send_SID_CAN_text(6);
     }
     else if (eq_state == 3)
     {
       eq_state = 4;
       send_SID_CAN_text(7);
     }
     else if (eq_state == 4)
     {
       eq_state = 1;
       send_SID_CAN_text(8);
     }
     
     IR_send_EQ();
  }   
}

void music_player_repeat()
{
  if(enableState)
  {     
     CAN.sendMsgBuf(0x430, 0, 8, request_beep);
     if(!repeat_state)
     {
       repeat_state = 1;
       send_SID_CAN_text(4);
     }
     else
     {
       repeat_state = 0;
       send_SID_CAN_text(3);
     }
     
     IR_send_RPT(); 
  }   
}

void music_player_reset_counter(boolean state)
{
  if(enableState)
  {
    if (state)
    {
      //Serial.println("enableState 1");
      src_long = 1;   
    } 
    else
    {
      src_long = 0;
      //Serial.println("enableState 0");
    }
  }
}

void music_player_wind(boolean state)
{
  if (state)
  {
    seek_long = 1;   
  } 
  else
  {
    seek_long = 0;
  }
}





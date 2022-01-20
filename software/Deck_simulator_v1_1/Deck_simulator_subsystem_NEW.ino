// DECK PIN setup:
const int N_EnablePin =             3;  // PIN 2  Enable signal
const int N_Signal1 =               4;  // PIN 3  Drive motor signal

const int N_Signal2 =               5;  // PIN 4 System status motor signal 1
const int N_Signal3 =               6;  // PIN 5 System status motor signal 2

const int N_pwm1 =                  7;  // PIN 6 Drive motor encoder 1
//const int N_pwm2 =                7;  // PIN 7 Drive motor encoder 2
const int N_Signal4 =               A5;  // PIN 8 System status motor optocoupler state
const int N_tape_inserted_status =  A4;  // PIN 10 Tape switch. IN or OUT

//
// DECK simulator subsystem variables:
//
// PWM Generator subsystem variables
int N_pwm_A_timer = 0;
int N_pwm_B_timer = 0;
int N_pwm_A_speed_timer = 0;
int N_pwm_B_speed_timer = 0;

// Enable variables
int N_power_on = 0;
int N_power_off = 0;

// Deck status variables
int N_pwm_state = 0;
int N_state_wheel_position = 1;


void new_deck_simulator_setup()
{
  pinMode(N_pwm1, OUTPUT);  
  //pinMode(N_pwm2, OUTPUT);  
  
  pinMode(N_EnablePin, INPUT);  
  pinMode(N_Signal1, INPUT); 
  pinMode(N_Signal2, INPUT);
  pinMode(N_Signal3, INPUT);
  pinMode(N_Signal4, OUTPUT);
  pinMode(N_tape_inserted_status, OUTPUT);
  
  digitalWrite(N_pwm1, HIGH); 
  //digitalWrite(N_pwm2, HIGH);
  
  digitalWrite(N_Signal4, HIGH);
  digitalWrite(N_tape_inserted_status, HIGH);
}


// Deck simulator handler. Should be called main loop.
void new_deck_simulator_handler()
{
  enableState = digitalRead(N_EnablePin);

  if (enableState == HIGH) 
  {
    N_power_off = 0;  
    
    if(!N_power_on)
    {
       N_power_on = 1;
       digitalWrite(N_pwm1, LOW); 
       //digitalWrite(N_pwm2, LOW);
       music_player_ON();
       if(N_state_wheel_position)
       {
         digitalWrite(N_Signal4, LOW); 
       }
       #ifdef DEBUG
       Serial.println("Tape Power on");
       #endif
       N_pwm_state = 2;      
    }    
    
    chThdSleepMilliseconds(100);
    //State machine++  //////////////////////
    if (digitalRead(N_Signal2))
    {
      chThdSleepMilliseconds(30);
      while(digitalRead(N_Signal2))
      {  
        // Tape out to STOP       
        if(N_state_wheel_position == 0)
        {
          #ifdef DEBUG
          Serial.println("Tape out to STOP"); 
          #endif 
          chThdSleepMilliseconds(725);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(100);        
          N_state_wheel_position++;          
        }
        // Stop to A       
        else if(N_state_wheel_position == 1)
        {
          if(digitalRead(N_Signal3))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println("Stop to A"); 
          #endif
          N_pwm_state = 5; 
          chThdSleepMilliseconds(40);  
          digitalWrite(N_Signal4, HIGH);
          chThdSleepMilliseconds(370);
          digitalWrite(N_Signal4, LOW);    
          N_state_wheel_position++; 
          chThdSleepMilliseconds(100);         
          
        }
        // A to <<
        else if(N_state_wheel_position == 2)
        {
          if(digitalRead(N_Signal3))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println("A to <<");
          #endif
          N_pwm_state = 3;
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(114);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(117);
          N_state_wheel_position++; 
        } 
        // << to >>  
        else if(N_state_wheel_position == 3)
        {
          if(digitalRead(N_Signal3))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println(" << to >>");
          #endif
          N_pwm_state = 4;
          chThdSleepMilliseconds(90);          
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(180);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(90);  
          N_state_wheel_position++;          
        } 
        // >> to B 
        else if(N_state_wheel_position == 4)
        {
          if(digitalRead(N_Signal3))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println(" >> to B");
          #endif
          N_pwm_state = 6;  
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(114);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(117);
          N_state_wheel_position++;
          break; 
        } 
      }
    }    
    //State machine-- //////////////////////
    else if (digitalRead(N_Signal3))
    {
      chThdSleepMilliseconds(30);
      while(digitalRead(N_Signal3))
      { 
        // B to >>  
        if(N_state_wheel_position == 5)
        {
          #ifdef DEBUG
          Serial.println("B to >>");
          #endif
          N_pwm_state = 4;
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(114);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(117);
          N_state_wheel_position--;         
        }
        // >> to <<
        else if(N_state_wheel_position == 4)
        {
          if(digitalRead(N_Signal2))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println(" >> to <<");
          #endif
          N_pwm_state = 3;
          chThdSleepMilliseconds(90);
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(180);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(117); 
          N_state_wheel_position--;          
        } 
         // <<  to A
        else if(N_state_wheel_position == 3)
        {
          if(digitalRead(N_Signal2))
          {
            break;
          }  
          #ifdef DEBUG        
          Serial.println(" <<  to A");
          #endif
          N_pwm_state = 5;          
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(114);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(117);        
          N_state_wheel_position--; 
        }      
        // A side to STOP
        else if(N_state_wheel_position == 2)
        {
          if(digitalRead(N_Signal2))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println("A side to STOP");
          #endif
          chThdSleepMilliseconds(40);  
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(370);
          digitalWrite(N_Signal4, LOW);  
          chThdSleepMilliseconds(117);        
          N_state_wheel_position--; 
          N_pwm_state = 0;
          break;   

        }
        // STOP to tape out
        else if(N_state_wheel_position == 1)
        {          
          if(digitalRead(N_Signal2))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println("STOP to tape out");
          #endif
          N_pwm_state = 0;
          chThdSleepMilliseconds(40);  
          digitalWrite(N_Signal4, HIGH);   
          chThdSleepMilliseconds(766);
          chThdSleepMilliseconds(40);    
          digitalWrite(N_tape_inserted_status, LOW);   
          chThdSleepMilliseconds(90);
          digitalWrite(N_tape_inserted_status, HIGH);    
          N_state_wheel_position--;
         
          break;  
        }
      }
    }        
  }    
  else 
  {  
     N_power_on = 0;
     N_pwm_state = 0;
     if(!N_power_off)
     {
       N_power_off = 1;
       digitalWrite(N_Signal4, HIGH);
       music_player_OFF();
       #ifdef DEBUG
       Serial.println("Tape Power off");
       #endif
       if(N_state_wheel_position)
       {
              N_state_wheel_position = 1; 
       } 
     }
  }   
}

void new_deck_simulator_PWM_handler()
{
  if(N_pwm_state == 5)
    {
      //if (digitalRead(Signal1))
      {
        pwm_A();
      }       
    }
    else if(N_pwm_state == 6)
    {
      //if (digitalRead(Signal1))
      {
        pwm_B();
      }       
    }
    else if(N_pwm_state == 3)
    {
      if (digitalRead(N_Signal1))
      {
        pwm_B_speed(); 
      }      
    }
    else if(N_pwm_state == 4)
    {
      if (digitalRead(N_Signal1))
      {
        pwm_A_speed(); 
      }       
    }
    else if(N_pwm_state == 0)
    {
      digitalWrite(N_pwm1, HIGH);
    }
    else if(N_pwm_state == 2)
    {
       digitalWrite(N_pwm1, LOW); 
       //digitalWrite(N_pwm2, LOW);
       if (digitalRead(N_Signal1))
       {
        N_pwm_state = 0; 
       }
    }
}

void pwm_A()
{
    N_pwm_A_timer++;
    
    if(N_pwm_A_timer == 387)
    {
      digitalWrite( N_pwm1, HIGH );      
    }
    else if(N_pwm_A_timer == 796)
    {
      digitalWrite( N_pwm1, LOW);       
    } 
    else if(N_pwm_A_timer == 916)
    {
      digitalWrite( N_pwm1, HIGH );       
    }
    else if(N_pwm_A_timer == 1046)
    {
      digitalWrite( N_pwm1, LOW); 
      N_pwm_A_timer = 0;
    }
}

void pwm_B()
{
    N_pwm_B_timer++;
    
    if(N_pwm_B_timer == 192)
    {
      digitalWrite( N_pwm1, HIGH );      
    }
    else if(N_pwm_B_timer == 253)
    {
      digitalWrite( N_pwm1, LOW);       
    } 
    else if(N_pwm_B_timer == 308)
    {
      digitalWrite( N_pwm1, HIGH );       
    }
    else if(N_pwm_B_timer == 498)
    {
      digitalWrite( N_pwm1, LOW); 
      N_pwm_B_timer = 0;
    }
}

void pwm_A_speed()
{
    N_pwm_A_speed_timer++;
    
    if(N_pwm_A_speed_timer == 14)
    {
      digitalWrite( N_pwm1, HIGH );      
    }
    else if(N_pwm_A_speed_timer == 28)
    {
      digitalWrite( N_pwm1, LOW);       
    } 
    else if(N_pwm_A_speed_timer == 32)
    {
      digitalWrite( N_pwm1, HIGH );       
    }
    else if(N_pwm_A_speed_timer == 36)
    {
      digitalWrite( N_pwm1, LOW); 
      N_pwm_A_speed_timer = 0;
    }
}

void pwm_B_speed()
{
    N_pwm_B_speed_timer++;
    
    if(N_pwm_B_speed_timer == 11)
    {
      digitalWrite( N_pwm1, HIGH );      
    }
    else if(N_pwm_B_speed_timer == 15)
    {
      digitalWrite( N_pwm1, LOW);       
    } 
    else if(N_pwm_B_speed_timer == 19)
    {
      digitalWrite( N_pwm1, HIGH );       
    }
    else if(N_pwm_B_speed_timer == 32)
    {
      digitalWrite( N_pwm1, LOW); 
      N_pwm_B_speed_timer = 0;
    }
}

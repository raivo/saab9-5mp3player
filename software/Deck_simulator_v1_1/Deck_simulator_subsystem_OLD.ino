// DECK PIN setup:
const int O_EnablePin =             3;  // PIN 2  Enable signal
const int O_Signal1 =               4;  // PIN 3  Drive motor signal

const int O_Signal2 =               5;  // PIN 4 System status motor signal 1
const int O_Signal3 =               6;  // PIN 5 System status motor signal 2

const int O_pwm1 =                  7;  // PIN 6 Drive motor encoder 1
const int O_pwm2 =                  9;  // PIN 7 Drive motor encoder 2
const int O_Signal4 =               A5;  // PIN 8 System status motor optocoupler state
const int O_tape_inserted_status =  A4;  // PIN 10 Tape switch. IN or OUT

//
// DECK simulator subsystem variables:
//
// PWM Generator subsystem variables
int O_pwm_A_timer = 0;
int O_pwm_B_timer = 0;
int O_pwm_A_speed_timer = 0;
int O_pwm_B_speed_timer = 0;

// Enable variables
int O_power_on = 0;
int O_power_off = 0;

// Deck status variables SHOULD BE 0
int O_pwm_state = 1;
int O_state_wheel_position = 1;

int O_pwm1_timer = 0;
int O_pwm2_timer = 0;


void old_deck_simulator_setup()
{
  pinMode(O_pwm1, OUTPUT);  
  pinMode(O_pwm2, OUTPUT);  
  
  pinMode(O_EnablePin, INPUT);  
  pinMode(O_Signal1, INPUT); 
  pinMode(O_Signal2, INPUT);
  pinMode(O_Signal3, INPUT);
  pinMode(O_Signal4, OUTPUT);
  pinMode(O_tape_inserted_status, OUTPUT);
  
  digitalWrite(O_pwm1, HIGH); 
  digitalWrite(O_pwm2, HIGH);
  
  digitalWrite(O_Signal4, HIGH);
  digitalWrite(O_tape_inserted_status, LOW);
}

// Deck simulator handler. Should be called main loop.
void old_deck_simulator_handler()
{
  chThdSleepMilliseconds(3); 
  enableState = digitalRead(O_EnablePin);

  if (enableState == HIGH) 
  {
    O_power_off = 0;  
    
    if(!O_power_on)
    {
       O_power_on = 1;
       digitalWrite(O_pwm1, LOW); 
       //digitalWrite(pwm2, LOW);
       music_player_ON();
       
       #ifdef DEBUG
       Serial.println("Tape Power on");
       #endif
       O_pwm_state = 2;      
    }    
    
    //delay(120);
    //State machine++  //////////////////////
    if (digitalRead(O_Signal2))
    {
      chThdSleepMilliseconds(30);
      while(digitalRead(O_Signal2))
      {  
        // Tape out to STOP       
        if(O_state_wheel_position == 0)
        {
          #ifdef DEBUG
          Serial.println("Tape out to STOP");  
          #endif
          chThdSleepMilliseconds(90);  
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(1000);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(50);          
          O_state_wheel_position++;          
        }
        // Stop to A       
        else if(O_state_wheel_position == 1)
        {
          if(digitalRead(O_Signal3))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println("Stop to A");  
          #endif          
          chThdSleepMilliseconds(90); 
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(450);
          digitalWrite(O_Signal4, HIGH);     
          O_state_wheel_position++; 
          chThdSleepMilliseconds(50);;         
          
        }
        // A to <<
        else if(O_state_wheel_position == 2)
        {
          if(digitalRead(O_Signal3))
          {
            O_pwm_state = 1;
            break;
          }
          #ifdef DEBUG
          Serial.println("A to <<");
          #endif
          
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(80);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(117); 
          O_state_wheel_position++; 
          O_pwm_state = 1;
        } 
        // << to >>  
        else if(O_state_wheel_position == 3)
        {
          if(digitalRead(O_Signal3))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println(" << to >>");
          #endif
          O_pwm_state = 1;
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(80);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(117);  
          O_state_wheel_position++;          
        } 
        // >> to B 
        else if(O_state_wheel_position == 4)
        {
          if(digitalRead(O_Signal3))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println(" >> to B");
          #endif
          O_pwm_state = 1;  
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(80);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(117);
          O_state_wheel_position++;
          break; 
        } 
      }
    }    
    //State machine-- //////////////////////
    else if (digitalRead(O_Signal3))
    {
      chThdSleepMilliseconds(30);
      while(digitalRead(O_Signal3))
      { 
        // B to >>  
        if(O_state_wheel_position == 5)
        {
          #ifdef DEBUG
          Serial.println("B to >>");
          #endif
          O_pwm_state = 1;
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(80);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(117);
          O_state_wheel_position--;         
        }
        // >> to <<
        else if(O_state_wheel_position == 4)
        {
          if(digitalRead(O_Signal2))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println(" >> to <<");
          #endif
          O_pwm_state = 1;
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(80);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(117);
          O_state_wheel_position--;          
        } 
         // <<  to A
        else if(O_state_wheel_position == 3)
        {
          if(digitalRead(O_Signal2))
          {
            break;
          }  
          #ifdef DEBUG        
          Serial.println(" <<  to A");
          #endif
          O_pwm_state = 1;          
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(80);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(117);
          O_state_wheel_position--; 
        }      
        // A side to STOP
        else if(O_state_wheel_position == 2)
        {
          if(digitalRead(O_Signal2))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println("A side to STOP");
          #endif
          chThdSleepMilliseconds(40);  
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(450);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(117);
          O_state_wheel_position--; 
          O_pwm_state = 0;
          break;   

        }
        // STOP to tape out
        else if(O_state_wheel_position == 1)
        { 
          chThdSleepMilliseconds(40);
          if(digitalRead(O_Signal2))
          {
            break;
          }
          #ifdef DEBUG
          Serial.println("STOP to tape out");
          #endif
          O_pwm_state = 0;
          chThdSleepMilliseconds(40);
          digitalWrite(O_Signal4, LOW);   
          chThdSleepMilliseconds(1000);
          digitalWrite(O_Signal4, HIGH);  
          chThdSleepMilliseconds(40);
          digitalWrite(O_tape_inserted_status, HIGH);   
          chThdSleepMilliseconds(90);
          digitalWrite(O_tape_inserted_status, LOW);   
          O_state_wheel_position--;
         
          break;  
        }
      }
    }        
  }    
  else 
  {  
     O_power_on = 0;
     O_pwm_state = 0;
     if(!O_power_off)
     {
       O_power_off = 1;
       digitalWrite(O_Signal4, HIGH);
       music_player_OFF();
       #ifdef DEBUG
       Serial.println("Tape Power off");
       #endif
       if(O_state_wheel_position)
       {
              O_state_wheel_position = 1; 
       } 
     }
  }   
}

//void deck_simulator_pwm_handler()
//{    
//    while (1) 
//    {        
//      if(pwm_state == 1)
//      {
//        digitalWrite( pwm2, HIGH );      
//        chThdSleepMilliseconds(15);
//        digitalWrite( pwm1, HIGH );
//        chThdSleepMilliseconds(11);
//        digitalWrite( pwm2, LOW);
//        chThdSleepMilliseconds(21);   
//        digitalWrite( pwm1, LOW);
//        chThdSleepMilliseconds(54);
//      }
//      else if(pwm_state == 0)
//      {
//         digitalWrite(pwm1, HIGH); 
//         digitalWrite(pwm2, HIGH);
//         chThdSleepMilliseconds(10); 
//      }
//      else if(pwm_state == 2)
//      {
//         digitalWrite(pwm1, LOW); 
//         digitalWrite(pwm2, LOW);
//         chThdSleepMilliseconds(10); 
//      }
//    } 
//}

void old_deck_simulator_PWM_handler()
{
  if(O_pwm_state == 1)
    {
      pwm1Isr();
      pwm2Isr();    
    }
    else if(O_pwm_state == 0)
    {
       digitalWrite(O_pwm1, HIGH); 
       digitalWrite(O_pwm2, HIGH);
    }
    else if(O_pwm_state == 2)
    {
       digitalWrite(O_pwm1, LOW); 
       digitalWrite(O_pwm2, LOW);
    }
}

void pwm1Isr()
{
    volatile long i;
    O_pwm1_timer++;
    
    if(O_pwm1_timer == 55)
    {
      digitalWrite(O_pwm2, HIGH );      
    }
    else if(O_pwm1_timer == 81)
    {
      digitalWrite(O_pwm2, LOW); 
    }
    else if(O_pwm1_timer == 106)
    {       
      O_pwm1_timer = 0;
    }
}

void pwm2Isr()
{
    //volatile long i;
    O_pwm2_timer++;
    
    if(O_pwm2_timer == 70)
    {
      digitalWrite(O_pwm1, HIGH ); 
      
    }
    else if(O_pwm2_timer == 106)
    {
      digitalWrite(O_pwm1, LOW); 
      O_pwm2_timer = 0;
    } 
}

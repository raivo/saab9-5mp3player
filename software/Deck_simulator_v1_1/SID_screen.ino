//------------------------------------------------------------------------------
// Laiendab CAN_subsystem moodulit, et kuvada SID ekraanile tekste. Tekstid on 
// defineeritud CAN-i pakettidena. Paketid tuleb saata grupina, ehk esimene ja 
// teine SID ekraani rida on koostatud kolmest paketist.Neile tuleb lisaks saata 
// kaks seadistuspaketti.
//
// SID_CAN_text_handler-it tuleb regulaarselt välja kutsuda, et pakkettide 
// koostamine töötaks vastavalt vabale ajale.
//
// Riistvara: ITT AVR CAN Board
// 
// ITT Group
// 25.05.2015
// ver 1.0
//------------------------------------------------------------------------------

#define GREETING1    1         
#define GREETING2    2
#define REPEAT_OFF   3
#define REPEAT_ON    4
#define EQ1          5
#define EQ2          6
#define EQ3          7
#define EQ4          8
#define SONG         9

//                                           D1     D2      D3      D4      D5      D6      D7      D8
unsigned char s_text_1_eq1[8] =           {0x45,  0x96,   0x01,   'E',    'Q',    ' 1',    ' ',    0x20};
unsigned char s_text_1_eq2[8] =           {0x45,  0x96,   0x01,   'E',    'Q',    ' 2',    ' ',    0x20};
unsigned char s_text_1_eq3[8] =           {0x45,  0x96,   0x01,   'E',    'Q',    ' 3',    ' ',    0x20};
unsigned char s_text_1_eq4[8] =           {0x45,  0x96,   0x01,   'E',    'Q',    ' 4',    ' ',    0x20};

unsigned char s_text_1_song[8] =           {0x45,  0x96,   0x01,   'S',    'O',    'N',    'G',    0x20};
unsigned char s_text_2_song[8] =           {0x04,  0x96,   0x01,   0x20,    0x20,   0x20,  0x20,   0x20};

unsigned char s_text_1_repeat_on[8] =      {0x45,  0x96,   0x01,   'R',    'E',    'P',    'E',    'A'};
unsigned char s_text_2_repeat_on[8] =      {0x04,  0x96,   0x01,   'T',    0x20,   'O',    'N',    0x20};

unsigned char s_text_1_repeat_off[8] =     {0x45,  0x96,   0x01,   'R',    'E',    'P',    'E',    'A'};
unsigned char s_text_2_repeat_off[8] =     {0x04,  0x96,   0x01,   'T',    0x20,   'O',    'F',    'F'};

unsigned char s_text_1_creeting_1[8] =     {0x45,  0x96,   0x01,   'H',    'E',    'L',    'L',    'O'};
unsigned char s_text_2_creeting_1[8] =     {0x04,  0x96,   0x01,   ' ',    'R',    'A',    'I',    'V'};
unsigned char s_text_3_creeting_1[8] =     {0x03,  0x96,   0x01,   'O',   0x20,   0x20,   0x20,   0x20};

unsigned char s_text_1_creeting_2[8] =     {0x45,  0x96,   0x01,   'M',    'P',    '3',    ' ',    'P'};
unsigned char s_text_2_creeting_2[8] =     {0x04,  0x96,   0x01,   'L',    'A',    'Y',    'E',    'R'};
unsigned char s_text_3_creeting_2[8] =     {0x03,  0x96,   0x01,   0x20,   0x20,   0x20,   0x20,   0x20};

unsigned char s_text_2[8] =                {0x04,  0x96,   0x01,   0x20,   0x20,   0x20,   0x20,   0x20};
unsigned char s_text_3[8] =                {0x03,  0x96,   0x01,   0x20,   0x20,   0x20,   0x20,   0x20};

unsigned char s_text_4[8] =                {0x02,  0x96,   0x02,   0x20,   0x20,   0x20,   0x20,   0x20};
unsigned char s_text_5[8] =                {0x01,  0x96,   0x02,   0x20,   0x20,   0x20,   0x20,   0x20};
unsigned char s_text_6[8] =                {0x00,  0x96,   0x02,   0x20,   0x20,   0x20,   0x20,   0x20};

unsigned char s_text_cont1[8] =            {0x1B,  0x00,   0x04,   0x2D,   0x00,   0x00,   0x00,   0x00};
unsigned char s_text_cont2[8] =            {0x1B,  0x00,   0x05,   0x2D,   0x00,   0x00,   0x00,   0x00};

void send_SID_CAN_text(int s_sid_message)
{
    //Serial.println("MESSAGE: send_SID_CAN_text");
    sid_message = s_sid_message;
    send_text = 1;  
}

void send_SID_CAN_song(int digit1, int digit2, int digit3, int digit_count)
{     
    if(digit_count >= 1)
    {
      s_text_2_song[4] =   (char) digit1 + 48;
    }
    else
    {
      s_text_2_song[4] = ' ';
    }
    
    if(digit_count >= 2)
    {
      s_text_2_song[5] =   (char) digit2 + 48;
    }
    else
    {
      s_text_2_song[5] = ' ';
    }
    
    if(digit_count >= 3)
    {
      s_text_2_song[6] =   (char) digit3 + 48;
    }
    else
    {
      s_text_2_song[6] = ' ';
    } 
    
    send_text = 1;
    sid_message = 9;   
}

int SID_completed()
{
 return send_text;
}

void SID_CAN_text_handler()
{
  if(send_text)
  {    
    if(sends_count <= 2)
    {  
       sends_count++;  
       switch (sid_message) 
       {
          case GREETING1:
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_creeting_1);                   
            chThdSleepMilliseconds(10); 
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2_creeting_1);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3_creeting_1);
            break;
          case GREETING2:             
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_creeting_2);                   
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2_creeting_2);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3_creeting_2); 
            break;
          case REPEAT_ON:                         
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_repeat_on);                   
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2_repeat_on);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3);
            break;
          case REPEAT_OFF:           
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_repeat_off);                   
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2_repeat_off);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3);
            break;
          case EQ1:     
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_eq1);                   
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3);  
            break;
          case EQ2:                      
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_eq2);                   
            chThdSleepMilliseconds(10); 
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3);
            break;
          case EQ3:                       
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_eq3);                   
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3);
            break; 
          case EQ4:                         
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_eq4);                   
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3); 
            break; 
          case SONG:    
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_1_song);                   
            chThdSleepMilliseconds(10); 
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_2_song);              
            chThdSleepMilliseconds(10);
            CAN.sendMsgBuf(0x32F, 0, 8, s_text_3); 
            break;           
          default:
            #ifdef DEBUG
            Serial.println("ERR: Wrong SID message: ");
            Serial.print(sid_message);
            Serial.println("-");
            Serial.print(send_text);
            Serial.println(" ");
            send_text = 0;
            sid_message = 0;
            #endif
            break;
       } 
       
      chThdSleepMilliseconds(10);          
      CAN.sendMsgBuf(0x32F, 0, 8, s_text_4);                   
      chThdSleepMilliseconds(10); 
      CAN.sendMsgBuf(0x32F, 0, 8, s_text_5);              
      chThdSleepMilliseconds(10);
      CAN.sendMsgBuf(0x32F, 0, 8, s_text_6);           
      
      chThdSleepMilliseconds(10);            
      CAN.sendMsgBuf(0x34F, 0, 8, s_text_cont1);                   
      chThdSleepMilliseconds(10); 
      CAN.sendMsgBuf(0x34F, 0, 8, s_text_cont2); 
    } 
    else
    {
      send_text = 0;
      sends_count = 0;    
      
      #ifdef DEBUG
      //
      Serial.println("SID message sent");
      #endif
   
      #ifdef DEBUG
    //Serial.println("SID  null");
    #endif
    }  
  } 
}



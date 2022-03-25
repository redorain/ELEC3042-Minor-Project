#define d_a 2  //Definition of the pin connected to the nixie tube
#define d_b 3
#define d_c 4
#define d_d 5
#define d_e 6
#define d_f 7
#define d_g 8
#define d_h 9

#define COM1 10   //Define the nixie tube serial port connected to each pin
#define COM2 11
#define COM3 12
#define COM4 13
// Define the analog pin connected to the button and LED lamp
#define ledpin_Red A1     // Red LED
#define ledpin_Yellow A3  // Yellow LED
#define inpin_Red A0      // Red button
#define inpin_Yellow A2   // Yellow button
#define inpin_Green A4    // Green button
#define buzzer A5         // Buzzer

int ms = 0, s = 55, m = 0, h = 12, Clock_State = 0, Alarm_State = 0, Clock_Type = 0;
// When Alarm_State is 0, the alarm is closed
int h_Alarm = h, m_Alarm = m, Alarm_Switch = 0;   // hour, minute and switch for the alarm

//Global variables used by multiple functions are defined respectively. H for hours, m for minutes, s for seconds

unsigned char num[17][8] =
{
 //a  b  c  d  e  f  g  h 
  {0, 0, 0, 0, 0, 0, 1, 1},     //0
  {1, 0, 0, 1, 1, 1, 1, 1},     //1
  {0, 0, 1, 0, 0, 1, 0, 1},     //2
  {0, 0, 0, 0, 1, 1, 0, 1},     //3
  {1, 0, 0, 1, 1, 0, 0, 1},     //4 ,
  {0, 1, 0, 0, 1, 0, 0, 1},     //5
  {0, 1, 0, 0, 0, 0, 0, 1},     //6
  {0, 0, 0, 1, 1, 1, 1, 1},     //7
  {0, 0, 0, 0, 0, 0, 0, 1},     //8
  {0, 0, 0, 0, 1, 0, 0, 1},     //9
  {1, 1, 1, 1, 1, 1, 1, 1},     //.
};

void setup()
{  //Set as output pin
  pinMode(d_a, OUTPUT);    
  pinMode(d_b, OUTPUT);
  pinMode(d_c, OUTPUT);
  pinMode(d_d, OUTPUT);
  pinMode(d_e, OUTPUT);
  pinMode(d_f, OUTPUT);
  pinMode(d_g, OUTPUT);
  pinMode(d_h, OUTPUT);

  pinMode(COM1, OUTPUT);
  pinMode(COM2, OUTPUT);
  pinMode(COM3, OUTPUT);
  pinMode(COM4, OUTPUT); // 4-digit 7 Segment Display
  
  pinMode(ledpin_Red, OUTPUT);    // LED red D1
  pinMode(ledpin_Yellow, OUTPUT); // LED Yellow D2
  pinMode(buzzer, OUTPUT);        // Buzzer
  //Set as input pin
  pinMode(inpin_Red, INPUT);     // Button red S1
  pinMode(inpin_Yellow, INPUT);  // Button yellow S2
  pinMode(inpin_Green, INPUT);   // Button `green S3
}

void loop() {

  if(Clock_State == 0)
    Clock_Type = 0, HH_MM_12();  // Display 12-hour HH_MM Clock
  else if(Clock_State == 1)  
    Clock_Type = 1, HH_MM_24();  // Display 24-hour HH_MM Clock
  else if(Clock_State == 2)
    MM_SS();                     // Display MM_SS Clock
  else if(Clock_State == 3)      
    Timeset_HH();                // Time set for hour
  else if(Clock_State == 4)
    Timeset_MM();                // Time set for minute
  else if(Clock_State == 5)
    Alarmset_HH();               // Time set for alarm hour
  else if(Clock_State == 6) 
    Alarmset_MM();               // Time set for alarm minute
  else if(Clock_State == 7)
    Alarm_HH_MM();               // Display Alarm HH_MM
  else
    sleep();                     // Clock sleep
}

void HH_MM_12(){
    while(1){
      while(m < 60){
        for(int ms = 125 * s; ms < 60 * 125; ms++){    // 125 * 8 = 1000 ms
          int times = 2;                               // 1000 ms * 60 = 1 min
          if(digitalRead(inpin_Yellow) == HIGH) {  // When press the yellow button S2
            bool flag = 1;
            for(int q = 0; q <= 125; q++) {
              if(digitalRead(inpin_Yellow) == LOW) {  
              flag = 0;
              break;
              }         // Judge whether holding the button down over 2 seconds or not
              delay(8);
            }
          if(flag == 1) {
            s += 3;
            Clock_State = 8;  // Change the clock status to sleep
            delay(1000);
            return;
          }
            Alarm_Switch ^= 1;  // if just press the button once
            delay(400);         // turn on or turn off the alarm
            ms += 50;
          }
          if(Alarm_Switch == 1) {
              digitalWrite(ledpin_Red, HIGH);     // if the alarm is opend, the LED D1 will turn on.
              if(h == h_Alarm && m == m_Alarm) {     // If it's the time for alarm, the buzzer will ring.
                times = 1;
                digitalWrite(buzzer, HIGH);    
                delay(1);
                digitalWrite(buzzer, LOW);  // Ring for the buzzer
                delay(1);
              }
           }
          else digitalWrite(ledpin_Red, LOW);  // If the alarm is closed, the LED d1 will turn off.
          if(digitalRead(inpin_Red) == HIGH) {  // When press the red button S1
            Clock_State = 3;
            bool Alarm_flag = 1;
            for(int q = 0; q < 250; q++) {
              if(digitalRead(inpin_Red) == LOW) {
                Alarm_flag = 0;
                break;        // Judge whether holding the button down over 2 seconds or not
              }
              if(Clock_Type == 0 && h > 12) 
                Blink(h - 12, m, 1, 1, 1);
              else
                Blink(h, m, 1, 1, 0);  // Display the time
            }
            if(Alarm_flag == 1) {
              Clock_State = 5;
              delay(400);
              ms += 50;
              return;         // Change the clock status to set alarm time
            }
            delay(400);
            return;     // Change the clock status to set clock time
          }
          s = ms / 125;   
          if(h < 10) {
              if(ms / 125 % 2 == 0)
                Display(2, h, 1);
              else
                Display(2, h, 0);     // Display the time for hour
              delay(4);
            if(m < 10) {
              Display(3, 0, 0);   // Display the time for minute
              delay(times);
              Display(4, m, 0);   // If minute less than 10.
              delay(times);
            }
            else {
              Display(3, m / 10, 0);
              delay(times);
              Display(4, m % 10, 0);  // if the minute over 10
              delay(times);
            }
         }
         else {
            if(((h - 12) > 9) || ((h - 12) <= 0 && h > 9))
              Display(1, 1, 0);         // Display hour for one situation
              delay(2);
            if(ms / 125 % 2 == 0) {
              if(h - 12)
                Display(2, (h - 12) % 10, 1);
              else
                Display(2, h % 10, 1);
            }
              else {
               if(h - 12)
                Display(2, (h - 12) % 10, 0);
              else
                Display(2, h % 10, 0);  // Display hour on the 4-digit 7 Segment Display
              }
              delay(2);
            if(m < 10) {
              Display(3, 0, 0);
              delay(times);
              if(h - 12)
                Display(4, m, 1);
              else
                Display(4, m, 0);   // Display minute.
              delay(times);
            }
            else {
              Display(3, m / 10, 0);
              delay(times);
              if(h - 12)
                Display(4, m % 10, 1);
              else
                Display(4, m % 10, 0);
              delay(times);
            }
         }
        if(digitalRead(inpin_Green) == HIGH) { // When press the green button S3
            bool Type_flag = 1;
            for(int q = 0; q < 250; q++) {
              if(digitalRead(inpin_Green) == LOW) {
                Type_flag = 0;
                break;
              }
              if(Clock_Type == 0 && h > 12) 
                Blink(h - 12, m, 1, 1, 1);// Display time when holding the button down
              else
                Blink(h, m, 1, 1, 0);
            }
            if(Type_flag == 1) {
              Clock_State = 1; // Judge whether holding the button down over 2 seconds or not
              delay(400);
              return;   // Change the clock status to display 24-hour HH_MM
            }
            Clock_State = 2;
            ms += 3 * 125;
            delay(800); // Change the clock status to display MM_SS
            return;
        }
      }
      s = 0;
      m++;
      }
    m = 0, h++;       // Set hour, minute and second in a region
    if(h > 23)  h = 0, m = 0, s = 0;
    }
}

void HH_MM_24(){
    while(1){
      while(m < 60){
        for(int ms = 125 * s; ms < 60 * 125; ms++){
          int times = 2;
          if(digitalRead(inpin_Yellow) == HIGH) {
            bool flag = 1;
            for(int q = 0; q <= 125; q++) {
              if(digitalRead(inpin_Yellow) == LOW) { // When press the yellow button
              flag = 0;
              break;
              }
              delay(8);
            }
          if(flag == 1) {
            s += 3;
            Clock_State = 8;  // Change the clock state
            delay(1000);
            return;
          }
            Alarm_Switch ^= 1; // Turn the alarm on or off
            delay(400);
            ms += 50;
          }
          if(Alarm_Switch == 1) {   // If it's the time for alarm 
              digitalWrite(ledpin_Red, HIGH);   // It will turn the buzzer on
              if(h == h_Alarm && m == m_Alarm) {
                times = 1;
                digitalWrite(buzzer, HIGH);
                delay(1);
                digitalWrite(buzzer, LOW);
                delay(1);
              }
           }
           else digitalWrite(ledpin_Red, LOW);  // Turn the LED on
          if(digitalRead(inpin_Red) == HIGH) {  //If the red button is pressed
            Clock_State = 3;
            bool Alarm_flag = 1;
            for(int q = 0; q < 250; q++) {
              if(digitalRead(inpin_Red) == LOW) {  // Just press once
                Alarm_flag = 0;
                break;
              }
              if(Clock_Type == 0 && h > 12)  
                Blink(h - 12, m, 1, 1, 1);  // Display the time for the clock
              else
                Blink(h, m, 1, 1, 0);
            }
            if(Alarm_flag == 1) {
              Clock_State = 5;
              s += 1;
              delay(1000);  // Change the clock status.
              return;
            }
            delay(400);
            return;
          }
           s = ms / 125;
           if(h < 10) {
                if(ms / 125 % 2 == 0)   // Make the middle decimal point blink
                  Display(2, h, 1);
                else
                  Display(2, h, 0);
                delay(4);
              if(m < 10) {
                Display(3, 0, 0);  // Display the time in different situation
                delay(times);
                Display(4, m, 0);
                delay(times);
              }
              else {
                Display(3, m / 10, 0);
                delay(times);
                Display(4, m % 10, 0);
                delay(times);
              }
           }
           else {
              Display(1, h / 10, 0);
              delay(2);
              if(ms / 125 % 2 == 0)
                  Display(2, h % 10, 1);
                else
                  Display(2, h % 10, 0);  // Display the time
              delay(2);
              if(m < 10) {
                Display(3, 0, 0);
                delay(times);
                Display(4, m, 0);
                delay(times);
              }
              else {
                Display(3, m / 10, 0);
                delay(times);
                Display(4, m % 10, 0);
                delay(times);
              }
           }
        if(digitalRead(inpin_Green) == HIGH) { // If the button S3 is pressed
            bool Type_flag = 1;
            for(int q = 0; q < 250; q++) {
              if(digitalRead(inpin_Green) == LOW) {
                Type_flag = 0;
                break;
              }
                Blink(h, m, 1, 1, 0);
            }
            if(Type_flag == 1) {  //Judge whether the button si holded for 2 seconds or not
              Clock_State = 0;
              ms += 3 * 125;
              delay(800);
              return;
            }
            Clock_State = 2; // Change the clock staus
            ms += 50;
            delay(400);
            return;
        }
      }
      s = 0;
      m++;
      }
      m = 0, h++;
      if(h > 23) h = 0, m = 0, s = 0; // Let the h, m, s in the legal region
    }
}

void MM_SS() {
  while(1){
    while(m < 60){
      for(int ms = 125 * s; ms < 60 * 125; ms++){
        int times = 2;
          if(digitalRead(inpin_Yellow) == HIGH) { // If yellow button is pressed 
            Alarm_Switch ^= 1;
            delay(400);               // Turn the alarm on or off
            ms += 50;
          }
          if(digitalRead(inpin_Green) == HIGH) {  // If the Green button is pressed
            Clock_State = 7;
            delay(400);            // Display the alarm time.
            return;
          }
        if(Alarm_Switch == 1) {
            digitalWrite(ledpin_Red, HIGH);
            if(h == h_Alarm && m == m_Alarm) {  // If It's the time for alarm
              times = 1;
              digitalWrite(buzzer, HIGH);       // Turn the buzzer on
              delay(1);
              digitalWrite(buzzer, LOW);
              delay(1);
            }
         }
        else digitalWrite(ledpin_Red, LOW);   // Turn the LED off
        s = ms / 125;
        if(m < 10) {
          Display(1, 0, 0);
          delay(2);
          Display(2, m, 1);
          delay(2);
          if(ms / 125 < 10) {
            Display(3, 0, 0); 
            delay(times);
            Display(4, ms / 125, 0);
            delay(times);
          }
          else {
            Display(3, ms / 125 / 10, 0);  // Display time for 24-hour
            delay(times);
            Display(4, ms / 125 % 10, 0);
            delay(times);
          }
        }
        else {
          Display(1, m / 10, 0);
          delay(2);
          Display(2, m % 10, 1);
          delay(2);
          if(ms < 10) {
            Display(3, 0, 0);
            delay(times);
            Display(4, ms / 125, 0);  // Display time in different way
            delay(times);
          }
          else {
            Display(3, ms / 125 / 10, 0);
            delay(times);
            Display(4, ms / 125 % 10, 0);
            delay(times);
          }
        }
      }
      s = 0;
      m++;
    }
    m = 0, h++;      // Let the h, m, s in the leagal way
    if(h > 23)  h = 0, m = 0, s = 0;
    }
}


void Timeset_HH() {
  
  normal :
               // When press button s1 and there is no operation
  while(1) {
    for(int q = 0; q < 125; q++) {
      if(digitalRead(inpin_Red) == HIGH) {
        Clock_State = 4;
        delay(400);      // if Press the button S1, then set the minute.
        return;
      }
      if(digitalRead(inpin_Yellow) == HIGH) goto increase; // Increase module to increase hour
      if(digitalRead(inpin_Green) == HIGH) goto decrease;  // Decrease module to decrease hour
      
      if(q / 62 % 2 == 0) {
        if(h > 12 && Clock_Type == 0)
          Blink(h - 12, m, 1, 1, 1);   // Let the hour part blink 
        else
          Blink(h, m, 1, 1, 0);
      }
      else {
        if(h > 12 && Clock_Type == 0)  // When hour over 12 and it's 12-hour display
          Blink(h - 12, m, 0, 1, 1);
        else
          Blink(h, m, 0, 1, 0);
      }
    }
  }
  
  increase :
  
    for(int q = 0; q < 500; q++) {
      if(digitalRead(inpin_Yellow) == LOW) {  
        h++;
        if(h > 23) h = 0;
        goto normal;    // Judge whether holding the button down or not.
      }
      delay(4);    
    }
    while(1) {
      for(int q = 0; q < 125; q++) {
        if(digitalRead(inpin_Yellow) == LOW) goto normal; 
        if(h > 12 && Clock_Type == 0)   // If Released the button, go back to normal
          Blink(h - 12, m, 1, 1, 1);  
        else
          Blink(h, m, 1, 1, 0);  // h increase 1 per second
      }
      h++;
      if(h > 23) h = 0;  // Let h during a legal region
    }

  decrease :
  
    for(int q = 0; q < 500; q++) {
      if(digitalRead(inpin_Green) == LOW) {
        h--;
        if(h < 0) h = 23;
        goto normal;
      }
      delay(4);    
    }
    while(1) {
      for(int q = 0; q < 125; q++) {
        if(digitalRead(inpin_Green) == LOW) goto normal;
        if(h > 12 && Clock_Type == 0)// If Released the button, go back to normal
          Blink(h - 12, m, 1, 1, 1);
        else
          Blink(h, m, 1, 1, 0); // h decrease 1 per second
        }
      h--;
      if(h < 0) h = 23;  // Let h during a legal region
    }
}


void Timeset_MM() {
  
  normal :

  while(1) {
    for(int q = 0; q < 125; q++) {
      if(digitalRead(inpin_Red) == HIGH) {  // If pressed the red button s1
        Clock_State = Clock_Type;
        delay(400);
        return;
      }
      if(digitalRead(inpin_Yellow) == HIGH) goto increase;
      if(digitalRead(inpin_Green) == HIGH) goto decrease;  
      
      if(q / 62 % 2 == 0) {
        if(h > 12 && Clock_Type == 0)
          Blink(h - 12, m, 1, 1, 1);  // Display the time in different way
        else
          Blink(h, m, 1, 1, 0);
      }
      else {
        if(h > 12 && Clock_Type == 0)
          Blink(h - 12, m, 1, 0, 1);  // Let the time blink
        else
          Blink(h, m, 1, 0, 0);
      }
    }
  }
  
  increase :
  
    for(int q = 0; q < 500; q++) {
      if(digitalRead(inpin_Yellow) == LOW) { // When the yellow button is pressed
        m++;
        if(m > 60) m = 0;
        goto normal; // turn the status to the normal
      }
      delay(4);    
    }
    while(1) {
      for(int q = 0; q < 125; q++) {
        if(digitalRead(inpin_Yellow) == LOW) goto normal;
        if(h > 12 && Clock_Type == 0)
          Blink(h - 12, m, 1, 1, 1);  // Display the time
        else
          Blink(h, m, 1, 1, 0);
      }
      m++;
      if(m > 60) m = 0;
    }

  decrease :
  
    for(int q = 0; q < 500; q++) {
      if(digitalRead(inpin_Green) == LOW) { // When the green button is pressed
        m--;
        if(m < 0) m = 59;
        goto normal;
      }
      delay(4);    
    }
    while(1) {
      for(int q = 0; q < 125; q++) {
        if(digitalRead(inpin_Green) == LOW) goto normal;
        if(h > 12 && Clock_Type == 0)
          Blink(h - 12, m, 1, 1, 1);  // Display the time
        else
          Blink(h, m, 1, 1, 0);
        }
      m--;
      if(m < 0) m = 59;
    }
}


void Alarmset_HH() {
  int h_temp = h, m_temp = m;
  normal :

  while(1) {
    for(int q = 0; q < 250; q++) {
      if(q < 125) digitalWrite(ledpin_Red, HIGH); // Trun the LED light on
      else digitalWrite(ledpin_Red, LOW);
      if(digitalRead(inpin_Red) == HIGH) { // When press the red button 
        h_Alarm = h_temp;
        Clock_State = 6;
        digitalWrite(ledpin_Red, LOW);
        delay(400);    // Jude the holding time
        return;
      }
      if(digitalRead(inpin_Yellow) == HIGH) goto increase;
      if(digitalRead(inpin_Green) == HIGH) goto decrease;
      
      if(q / 62 % 2 == 0) {
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 1, 1, 1);
        else
          Blink(h_temp, m_temp, 1, 1, 0);   // Blink the hour value in the clock
      }
      else {
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 0, 1, 1);
        else
          Blink(h_temp, m_temp, 0, 1, 0);  // Blink the hour value in the clock
      }
    }
  }
  
  increase :
  
    for(int q = 0; q < 500; q++) {
      if(q < 125) digitalWrite(ledpin_Red, LOW); 
      else digitalWrite(ledpin_Red, HIGH);
      if(digitalRead(inpin_Yellow) == LOW) {
        h_temp++;
        if(h > 23) h_temp = 0; // Judge the press time
        goto normal;
      }
      delay(4);    
    }
    while(1) {
      for(int q = 0; q < 250; q++) {
        if(q < 125) digitalWrite(ledpin_Red, HIGH);
        else digitalWrite(ledpin_Red, LOW);
        if(digitalRead(inpin_Yellow) == LOW) goto normal; // Go back to normal
        if(q == 250) h_temp++;
        if(h_temp > 23) h_temp = 0;
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 1, 1, 1);
        else
          Blink(h_temp, m_temp, 1, 1, 0); // Blink the time
      }
      h_temp++;
      if(h_temp > 23) h_temp = 0;
    }

  decrease :
  
    for(int q = 0; q < 500; q++) {
      if(q < 125) digitalWrite(ledpin_Red, LOW);
      else digitalWrite(ledpin_Red, HIGH);
      if(digitalRead(inpin_Green) == LOW) { 
        h_temp--;
        if(h_temp < 0) h_temp = 23;
        goto normal;  // Judge the time of holding button S3
      }
      delay(4);    
    }
    while(1) {
      for(int q = 0; q < 250; q++) {
        if(q < 125) digitalWrite(ledpin_Red, HIGH);
        else digitalWrite(ledpin_Red, LOW);
        if(digitalRead(inpin_Green) == LOW) goto normal;  // Go back to normal
        if(q == 250) h_temp--;
        if(h_temp < 0) h_temp = 23;
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 1, 1, 1); // Blink the value of time
        else
          Blink(h_temp, m_temp, 1, 1, 0);
        }
      h_temp--;
      if(h_temp < 0) h_temp = 23;
    }
}

void Alarmset_MM() {
  int h_temp = h_Alarm, m_temp = m;
  normal :

  while(1) {
    for(int q = 0; q < 250; q++) {
      if(q < 125) digitalWrite(ledpin_Red, HIGH); // Judge the pressing time of holding button
      else digitalWrite(ledpin_Red, LOW); 
      if(digitalRead(inpin_Red) == HIGH) { // If the button S1 is pressed
        Clock_State = Clock_Type;
        Alarm_Switch = 1;
        m_Alarm = m_temp;
        digitalWrite(ledpin_Red, LOW); // Change the clock status
        delay(400);
        return;
      }
      if(digitalRead(inpin_Yellow) == HIGH) goto increase;
      if(digitalRead(inpin_Green) == HIGH) goto decrease; // change the status of Clock
      
      if(q / 62 % 2 == 0) {
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 1, 1, 1); // Blink the value of minute
        else
          Blink(h_temp, m_temp, 1, 1, 0);
      }
      else {
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 1, 0, 1); // Blink the value of minute
        else
          Blink(h_temp, m_temp, 1, 0, 0);
      }
    }
  }
  
  increase :
  
    for(int q = 0; q < 500; q++) {
      if(q < 250) digitalWrite(ledpin_Red, LOW); // Judge the pressing time 
      else digitalWrite(ledpin_Red, HIGH);
      if(digitalRead(inpin_Yellow) == LOW) {
        m_temp++;
        if(m_temp > 60) m_temp = 0; // If less than 2 seconds, just increase 1
        goto normal; 
      }
      delay(4);    
    }
    while(1) {
      for(int q = 0; q < 250; q++) {
        if(q < 125) digitalWrite(ledpin_Red, HIGH);
        else digitalWrite(ledpin_Red, LOW);  
        if(digitalRead(inpin_Yellow) == LOW) goto normal;
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 1, 1, 1);
        else
          Blink(h_temp, m_temp, 1, 1, 0);
      }  // If holding the button down for long time, the value will increase 1 per second
      m_temp++;
      if(m_temp > 60) m_temp = 0;
    }

  decrease :
  
    for(int q = 0; q < 500; q++) {
      if(q < 250) digitalWrite(ledpin_Red, LOW); // Judge the pressing time 
      else digitalWrite(ledpin_Red, HIGH);
      if(digitalRead(inpin_Green) == LOW) {
        m_temp--;
        if(m_temp < 0) m_temp = 59;
        goto normal;
      }
      delay(4);    // If less than 2 seconds, just increase 1
    }
    while(1) {
      for(int q = 0; q < 250; q++) {
        if(q < 125) digitalWrite(ledpin_Red, HIGH);
        else digitalWrite(ledpin_Red, LOW);
        if(digitalRead(inpin_Green) == LOW) goto normal;
        if(h_temp > 12 && Clock_Type == 0)
          Blink(h_temp - 12, m_temp, 1, 1, 1);
        else
          Blink(h_temp, m_temp, 1, 1, 0);
        } // If holding the button down for long time, the value will increase 1 per second
      m_temp--;
      if(m_temp < 0) m_temp = 59;
    }
}

void Alarm_HH_MM() { // Display the time of alarm 
    while(1){
      while(m < 60){
        for(int ms = 125 * s; ms < 60 * 125; ms++){
          int times = 2;
          digitalWrite(ledpin_Yellow, HIGH);
          if(digitalRead(inpin_Yellow) == HIGH) { // If yellow button S1 is pressed
            Alarm_Switch ^= 1;
            delay(400);
            ms += 50;
          }
          if(digitalRead(inpin_Green) == HIGH) { //   If green button S3 is pressed
            Clock_State = Clock_Type;
            digitalWrite(ledpin_Yellow, LOW);
            delay(400);
            return;
          }
          if(Alarm_Switch == 1) {  // If alarm is opend
              digitalWrite(ledpin_Red, HIGH);
              if(h == h_Alarm && m == m_Alarm) {
                times = 1;
                digitalWrite(buzzer, HIGH);  // Turn the buzzer on
                delay(1);
                digitalWrite(buzzer, LOW);
                delay(1);
              }
           }
          else digitalWrite(ledpin_Red, LOW); // Turn the LED off
          if(digitalRead(inpin_Red) == HIGH) {
            Clock_State = 3;
            bool Alarm_flag = 1;
            for(int q = 0; q < 250; q++) {
              if(digitalRead(inpin_Red) == LOW) { // Judge the pressing time
                Alarm_flag = 0;
                break;
              }
              if(Clock_Type == 0 && h > 12)  // Blink for the time
                Blink(h - 12, m, 1, 1, 1);
              else
                Blink(h, m, 1, 1, 0);
            }
            if(Alarm_flag == 1) { // Change the clock type
              Clock_State = 5;
              delay(400);
              return;
            }
            delay(400);
            return;
          }
          s = ms / 125;
          if(h_Alarm < 10 || Clock_Type == 1) {
              Display(1, h_Alarm / 10, 0);
              delay(2);
              Display(2, h_Alarm % 10, 1); // Display the alarm time
              delay(2);
            if(m_Alarm < 10) {
              Display(3, 0, 0);
              delay(times);
              Display(4, m_Alarm, 0);
              delay(times);
            }
            else {
              Display(3, m_Alarm / 10, 0); // Applying for different situation
              delay(times);
              Display(4, m_Alarm % 10, 0);
              delay(times);
            }
         }
         else {
            if(((h_Alarm - 12) > 9) || ((h_Alarm - 12) <= 0 && h_Alarm > 9))
              Display(1, 1, 0);
            else
              Display(1, 0, 0);
              delay(2);
            if(ms / 125 % 2 == 0) {  // Display the time for the alarm
              if(h_Alarm - 12)
                Display(2, (h_Alarm - 12) % 10, 1);
              else
                Display(2, h_Alarm % 10, 1);
            }
            else {
               if(h_Alarm - 12) // Display the time
                Display(2, (h_Alarm - 12) % 10, 0);
              else
                Display(2, h_Alarm % 10, 0);
            }
            delay(2);
            Display(3, m_Alarm / 10, 0);
            delay(times);
            if(h_Alarm - 12)
              Display(4, m_Alarm, 1); // Display the alarm time
            else
              Display(4, m_Alarm, 0);
            delay(times);
         }
      }
      s = 0;
      m++;
      }
    m = 0, h++; // Let the h, m, s in a legal region
    if(h > 23)  h = 0, m = 0, s = 0; 
    }
}

void sleep() {
  while(1){
      if(s >= 60) s -= 60, m +=1 ;
      if(m >= 60) m -= 60, h += 1;
      if(h >= 24) h -= 24;    // make m, h, s in a legal region
      if(digitalRead(inpin_Yellow) == HIGH) {
        bool flag = 1;   // Press the yellow button S2
        for(int q = 0; q <= 125; q++) {
          if(digitalRead(inpin_Yellow) == LOW) {
            flag = 0;
            break;
          }       
          delay(8); // If holding the button S2 for 2 seconds
        }           // the clock will turn it on.
        if(flag == 1) {
          if(h > 12 && Clock_Type == 0)
            Blink(h - 12, m, 1, 1, 1); //Display the Clock
          else
            Blink(h, m, 1, 1, 0);
          s += 3;
          Clock_State = Clock_Type; // Return to Display time
          delay(1000);
          return;
        }
      }
      for(int q = 0; q <= 125; q++) {
        digitalWrite(COM1, LOW);
        digitalWrite(COM2, LOW);
        digitalWrite(COM3, LOW);
        digitalWrite(COM4, LOW);  // Sleep for the 4-digit 7 Segment Display
        delay(8);
      }
      s+=1;
  }
}
// Make the value on the 4-digit 7 Segment Display blink
void Blink(int h_temp, int m_temp, int Mode1, int Mode2, unsigned char Point) {
  int temp_time = 2;
  if(Mode1 ^ Mode2 == 1) temp_time = 4;  // Make the delay time same
  if(Mode1 == 1) {
    if(h_temp < 10) {
      Display(1, 0, 0); // If mode1 == 0, it will not display hour
      delay(temp_time);
      Display(2, h_temp, 1); // Display the time
      delay(temp_time);
    }
    else {
      Display(1, h_temp / 10, 0);
      delay(temp_time);
      Display(2, h_temp % 10, 1);
      delay(temp_time);
    }
  }
  if(Mode2 == 1) {
    if(m_temp < 10) { // If mode2 == 0, it will not display minute
      Display(3, 0, 0);
      delay(temp_time);
      if(Point == 0)
        Display(4, m_temp, 0);
      else
        Display(4, m_temp, 1);
      delay(temp_time);
    }
    else {
      Display(3, m_temp / 10, 0); // Display the number on the 4-digit 7 Segment Display
      delay(temp_time);
      if(Point == 0)
        Display(4, m_temp % 10, 0);
      else
        Display(4, m_temp % 10, 1);
      delay(temp_time);
    }
  }
}

void Display(unsigned char com, unsigned char n, unsigned char Point)
{
  digitalWrite(d_a, LOW);      //delete the light before
  digitalWrite(d_b, LOW);
  digitalWrite(d_c, LOW);
  digitalWrite(d_d, LOW);
  digitalWrite(d_e, LOW);
  digitalWrite(d_f, LOW);
  digitalWrite(d_g, LOW);
  digitalWrite(d_h, LOW);
 
  switch(com)           //Gate bit
  {
    case 1:
      digitalWrite(COM1, HIGH);   //Select bit 1
      digitalWrite(COM2, LOW);
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, LOW);
      break;
    case 2:
      digitalWrite(COM1, LOW);
      digitalWrite(COM2, HIGH);   //Select bit 2
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, LOW);
      break;
    case 3:
      digitalWrite(COM1, LOW);
      digitalWrite(COM2, LOW);
      digitalWrite(COM3, HIGH);   //Select bit 3
      digitalWrite(COM4, LOW);
      break;
    case 4:
      digitalWrite(COM1, LOW);
      digitalWrite(COM2, LOW);
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, HIGH);   //Select bit 4
      break;
    default:break;
  }
 
  digitalWrite(d_a, num[n][0]);      //Query code value table
  digitalWrite(d_b, num[n][1]);
  digitalWrite(d_c, num[n][2]);
  digitalWrite(d_d, num[n][3]);
  digitalWrite(d_e, num[n][4]);
  digitalWrite(d_f, num[n][5]);
  digitalWrite(d_g, num[n][6]);
  if(Point == 0)
    digitalWrite(d_h, num[n][7]);
  else
    digitalWrite(d_h, 0);
}

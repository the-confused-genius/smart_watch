#include<smartwatch.h>

void setup() {
  oled.init();
  oled.clear(); 
  oled.setPower(0);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,OUTPUT);
  attachInterrupt(0,handleInt,FALLING);
  setSQW();
  pox.begin();
  pox.shutdown();
}



void handleInt() {
  c = 1;
}
 
void setSQW() {
  Wire.beginTransmission(0x68);
  Wire.write(7);
  Wire.write(0x10);
  Wire.endTransmission();
}

uint8_t item;

void loop() {
 if(digitalRead(3)==LOW && digitalRead(4)==LOW)
 {  item = displayMenu();
    if(item==0)
    {   delay(200);
        displayAlarm();      
    }
    if(item==1)
    {
      delay(200);
      optiontd();
    }
    if(item==2)
    {
      displaysettings();
    }
    if(item==4)
    {
      heartrate();
    }
    oled.clear();
 }
 else if(digitalRead(3)==LOW)
 {  
    if(c)
    {
       u.utime();
       c=0;
    }  
    oled.setPower(1);
    u.displaytime();
    delay(700);
 }
 else if(digitalRead(4)==LOW)
 { 
    u.udate();
    oled.setPower(1);
    u.displaydate();
    delay(700);
 }
 else if((digitalRead(5)==LOW && gyscreen) || alwayson)
 {
  if(c)
    {
       u.utime();
       c=0;
    }  
    oled.setPower(1);
    u.displaytime();
    delay(700);
 }
 else
    oled.setPower(0);

  checkalarm();
  
 delay(200); 
}

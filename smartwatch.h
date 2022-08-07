#include <Wire.h>
#include <GyverOLED.h>
#include <DS1307RTC.h>
#include "MAX30100_PulseOximeter.h"

GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;
char dayofweek[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

void formattodisplay(uint8_t n);
//void(*resetFunc) (void)=0;

class updatetd
{
	public:
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint8_t day;
  uint8_t mon;
  uint8_t year;
  uint8_t dow;
  char m[3] = "AM";

  void zeller()
  {
     uint8_t i;
     uint8_t y = year;
     if(mon > 2)
        i = mon;    
     else {
        i = 12+mon;    
        y--; 
     }
     int w = (day + floor((13*(i+1))/5) + y + floor(y/4) + 104);
     dow = w % 7;
  }
	  void utime()
	  {
	    tmElements_t tm;
	    RTC.read(tm);
	    hour = tm.Hour;
	    min = tm.Minute;
	    sec = tm.Second;    
		m[0] = (hour<12) ? 'A' : 'P';
		if(hour>12)
	    	hour = hour-12;
	  }
	  void udate()
	  {
	    tmElements_t tm;
	    RTC.read(tm);
	    day = tm.Day;
	    mon = tm.Month;
	    year = (tmYearToCalendar(tm.Year)) % 100;
	    zeller();
	  }
	
		void displaytime()
		{   
		  oled.setScale(3);
		  oled.setCursor(16, 1);
		  formattodisplay(hour);  
		  oled.print(':');
		  formattodisplay(min);
		  oled.setCursor(110, 1); 
		  oled.setScale(1);
		  oled.print(':');
		  formattodisplay(sec);  
		  oled.setCursor(110, 3);
		  oled.print(' '); 
		  oled.print(m);
		  oled.update();
		}
		
		void displaydate()
		{  
		  oled.setScale(3);
		  oled.setCursor(16, 1);
		  formattodisplay(day); 
		  oled.print('/');
		  formattodisplay(mon);
		  oled.setScale(1);
		  oled.setCursor(110, 1); 
		  oled.print(' ');
		  formattodisplay(year);  
		  oled.setCursor(110, 3); 
		  oled.print(dayofweek[dow]);
		  oled.update();
		}
			
};

void formattodisplay(uint8_t n)
{
  if(n<10)
    oled.print(0);
  oled.print(n);
}

class alarm
{
	public:
	uint8_t hour;
	uint8_t min;
	char m;
	bool check;
	
	alarm()
	{
		hour = 0;
		min = 0;
		m = 'A';
		check = false;
	}
};

class updatetd u;
class alarm a[6];
PulseOximeter pox;
bool motoron = true;
bool gyscreen = true;
bool alwayson = false;
bool c = 1;

void setAlarm(int item)
{
	int c = 0,k = 0;
	int hour = a[item].hour;
	int min = a[item].min;
	char m = a[item].m;
	delay(200);
	oled.clear();
	while(true){    
		oled.setScale(2);
		oled.setCursor(0,0);
		formattodisplay(hour);
		oled.print(':');
		formattodisplay(min);
		oled.print(' ');
		oled.print(m);
		oled.print('M');
		oled.setScale(1);
		oled.setCursor(8,3);
		oled.print("Set");
		oled.setCursor(104,3);
		oled.print("Back");
		oled.update();
		
		
		if (digitalRead(3)==LOW)
	    {   c = (c+1)%5; 
	        delay(300);
	        oled.clear();
	    }
	    
	    if(c==4)
		{   oled.setCursor(97,3);
		    oled.print('>');
		}
		else if(c==3)
		{   oled.setCursor(0,3);
		    oled.print('>');
		}
		else if(c==2)
		  oled.fastLineH(16,72,94);
		else if(c==1)
		  oled.fastLineH(16,36,58);
		else 
		  oled.fastLineH(16,0,22);
	    
		if(digitalRead(4)==LOW)
		{
			if(c==0)
			{   hour=(hour+1)%12;
			    delay(500);
			}
			else if(c==1)
			{   min=(min+1)%60;
			    delay(200);
			}
			else if(c==2)
			{   k = (k+1)%2;
			    m = k ? 'A' : 'P';
			    delay(300);
			}			
			
			else if(c==3)
			{
				a[item].hour = hour;
				a[item].min = min;
				a[item].m = m;
				a[item].check = true;
				delay(300);
				oled.clear();
				return ;
			}
			else if(c==4)
			{
				a[item].hour = 0;
				a[item].min = 0;
				a[item].m = 'A';
				a[item].check = false;
				delay(300);
				oled.clear();
				return ;
			}
		}
	}
}


void displayAlarm()
{
	uint8_t item = 0;
	uint8_t x=0,y=0;
	oled.setPower(1);
	oled.clear();
	oled.setScale(1);
	while(true)
	{
		if(digitalRead(3)==LOW)
		{  	item = (item + 1)%7 ;
		 	oled.clear();
		}
		switch(item)
		{
		case 0: x=0;y=1;
		        break;
		case 1: x=0;y=2;
		        break;
		case 2: x=0;y=3;
		        break;
		case 3: x=91; y=1;
		        break;
		case 4: x=91; y=2;
		        break;
		case 5: x=91; y=3;
		        break;
		case 6: x=48; y=3;
		        break;
		}
		oled.setCursor(52,0);
		oled.print("Alarm");
		oled.setCursor(8,1);
		formattodisplay(a[0].hour);
		oled.print(':');
		formattodisplay(a[0].min);
		
		oled.setCursor(8,2);
		formattodisplay(a[1].hour);
		oled.print(':');
		formattodisplay(a[1].min);
		
		oled.setCursor(8,3);
		formattodisplay(a[2].hour);
		oled.print(':');
		formattodisplay(a[2].min);
		
		oled.setCursor(99,1);
		formattodisplay(a[3].hour);
		oled.print(':');
		formattodisplay(a[3].min);
		
		oled.setCursor(99,2);
		formattodisplay(a[4].hour);
		oled.print(':');
		formattodisplay(a[4].min);
		
		oled.setCursor(99,3);
		formattodisplay(a[5].hour);
		oled.print(':');
		formattodisplay(a[5].min);
		
		oled.setCursor(55,3);
		oled.print("Exit");
		oled.setCursor(x,y);
		oled.print(">");
		oled.update();
		if(item==6 && digitalRead(4)==LOW)
			return ;
		else if(digitalRead(4)==LOW)
		  setAlarm(item);
		delay(100);
	}
}


void settime()
{
	tmElements_t tm;
	RTC.read(tm);
	int c = 0,k=0;
	int hour = tm.Hour;
	int min = tm.Minute;
	char m;
	m = (hour<12) ? 'A' : 'P';
	if(hour>=12)
	    hour = hour-12;
	delay(200);
	oled.clear();
	while(true){    
		oled.setScale(2);
		oled.setCursor(0,0);
		formattodisplay(hour);
		oled.print(':');
		formattodisplay(min);
		oled.print(' ');
		oled.print(m);
		oled.print('M');
		oled.setScale(1);
		oled.setCursor(8,3);
		oled.print("Set");
		oled.setCursor(104,3);
		oled.print("Back");
		oled.update();
		
		
		if (digitalRead(3)==LOW)
	    {   c = (c+1)%5; 
	        delay(300);
	        oled.clear();
	    }
	    
	    if(c==4)
		{   oled.setCursor(97,3);
		    oled.print('>');
		}
		else if(c==3)
		{   oled.setCursor(0,3);
		    oled.print('>');
		}
		else if(c==2)
		  oled.fastLineH(16,72,94);
		else if(c==1)
		  oled.fastLineH(16,36,58);
		else 
		  oled.fastLineH(16,0,22);
	    
		if(digitalRead(4)==LOW)
		{
			if(c==0)
			{   hour=(hour+1)%12;
				delay(300);
			}
			else if(c==1)
			{   min=(min+1)%60;
			    delay(200);
			}
			else if(c==2)
			{   k = (k+1)%2;
			    m = k ? 'A' : 'P';
			    delay(300);
			}			
			
			else if(c==3)
			{
				tmElements_t tm;
				RTC.read(tm);
				int day = tm.Day;
				int mon = tm.Month;
				int year = tm.Year;
				if(m=='P' )
					hour = hour + 12;
				
				tm.Hour = hour;
				tm.Minute = min;
				tm.Second = 0;
				tm.Day = day;
				tm.Month = mon;
				tm.Year = year;				
        		RTC.write(tm);
				delay(300);
				oled.clear();
				return ;
			}
			else if(c==4)
			{
				oled.clear();
				return ;
			}
		}
	}
}


void setdate()
{
	tmElements_t tm;
	RTC.read(tm);
	int c = 0;
	int day = tm.Day;
	int mon = tm.Month;
	int year = (tmYearToCalendar(tm.Year)) % 100;
	delay(200);
	oled.clear();
	while(true){   
		oled.setScale(2);
		oled.setCursor(0,0);
		formattodisplay(day);
		oled.print('-');
		formattodisplay(mon);
		oled.print('-');
		formattodisplay(year);
		oled.setScale(1);
		oled.setCursor(8,3);
		oled.print("Set");
		oled.setCursor(104,3);
		oled.print("Back");
		oled.update();
		
		
		if (digitalRead(3)==LOW)
	    {   c = (c+1)%5; 
	        delay(300);
	        oled.clear();
	    }
	    
	    if(c==4)
		{   oled.setCursor(97,3);
		    oled.print('>');
		}
		else if(c==3)
		{   oled.setCursor(0,3);
		    oled.print('>');
		}
		else if(c==2)
		  oled.fastLineH(16,72,94);
		else if(c==1)
		  oled.fastLineH(16,36,58);
		else 
		  oled.fastLineH(16,0,22);
	    
		if(digitalRead(4)==LOW)
		{
			if(c==0)
			{   day = day % 31;
				day = day + 1;
			    delay(400);
			}
			else if(c==1)
			{   mon = mon % 12;
				mon = mon + 1;
			    delay(400);
			}
			else if(c==2)
			{   year = (year + 1) % 50;
			    delay(500);
			}			
			
			else if(c==3)
			{
				tmElements_t tm;
				RTC.read(tm);
				int hour = tm.Hour;
  				int min = tm.Minute;
  				int sec = tm.Second;
				tm.Hour = hour;
       			tm.Minute = min;
        		tm.Second = sec;
				tm.Day = day;
				tm.Month = mon;
				tm.Year = CalendarYrToTm(year-48);				
        		RTC.write(tm);
				delay(300);
				oled.clear();
				return ;
			}
			else if(c==4)
			{
				oled.clear();
				return ;
			}
		}
	}
}


void optiontd()
{
	int item = 0,x=0,y=0,z=2;
	oled.clear();
	while(true)
	{
		if(digitalRead(3)==LOW)
		{  	item = (item + 1)%3 ;
		 	oled.clear();
		}
		switch(item)
		{
			case 0: x=0;y=0;z=2;
			        break;
			case 1: x=0;y=2;z=2;
					break;
			case 2: x=97;y=3;z=1;
					break;
		}
		    
		oled.setScale(2);
		oled.setCursor(14,0);
		oled.print("Time");
		oled.setCursor(14,2);
		oled.print("Date");
		oled.setScale(1);
		oled.setCursor(104,3);
		oled.print("Exit");
		oled.setScale(z);
		oled.setCursor(x,y);
		oled.print(">");
		oled.update();
		
		if(digitalRead(4)==LOW && item==2)
			return ;
		else if(digitalRead(4)==LOW && item==1)
		    setdate();
		else if(digitalRead(4)==LOW && item==0)
			settime();
		delay(100);
	}
}


void displaysettings()
{
	int item = 0;
	uint8_t x=0,y=0;
	oled.clear();
	delay(200);
	oled.setScale(1);
	int a=0,b=0,c=0;
	while(true)
	{
	if(digitalRead(3)==LOW)
	{   item = (item + 1)%4 ;
	  oled.clear();
	}
	switch(item)
	{
	case 0: x=0;y=1;
	        break;
	case 1: x=0;y=2;
	        break;
	case 2: x=0;y=3;
	        break;
	case 3: x=98; y=3;
	        break;
	}
	oled.setCursor(24,0);
	oled.print("Settings");
	oled.setCursor(8,1);
	oled.print("Motor ");
	if(motoron)
	  oled.print("ON ");
	else 
	  oled.print("OFF");
	oled.setCursor(8,2);
	oled.print("Gesture ");
	if(gyscreen)
	  oled.print("ON ");
	else 
	  oled.print("OFF");
	oled.setCursor(8,3);
	oled.print("Alwayson ");
	if(alwayson)
	  oled.print("ON ");
	else 
	  oled.print("OFF");
	oled.setCursor(104,3);
	oled.print("Exit");
	oled.setCursor(x,y);
	oled.print(">");
	oled.update();
	if(digitalRead(4)==LOW && item==0)
	{
	  a = (a+1)%2;
	  motoron = a ? false:true;
	  delay(200);
	}
	else if(digitalRead(4)==LOW && item==1)
	{
	  b = (b+1)%2;
	  gyscreen = b ? false:true;
	  delay(200);
	}
	else if(digitalRead(4)==LOW && item==2)
	{
	  c = (c+1)%2;
	  alwayson = c ? false:true;
	  delay(100);
	  //resetFunc();
	}
	else if(digitalRead(4)==LOW && item==3)
	  return ;
	delay(100);
	}
}



int displayMenu()
{
	oled.setPower(1);
	int item = 0;
	uint8_t x=0,y=0;
	oled.clear();
	oled.setScale(1);
	int c =0;
	while(true)
	{
		if(digitalRead(3)==LOW && c>4)
		{  	item = (item + 1)%6 ;
		 	oled.clear();
		}
		switch(item)
		{
		case 0: x=0;y=1;
		        break;
		case 1: x=0;y=2;
		        break;
		case 2: x=0;y=3;
		        break;
		case 3: x=80; y=1;
		        break;
		case 4: x=80; y=2;
		        break;
		case 5: x=80; y=3;
		        break;
		}
		oled.setCursor(52,0);
		oled.print("Menu");
		oled.setCursor(8,1);
		oled.print("Alarm");
		oled.setCursor(8,2);
		oled.print("Set T/D");
		oled.setCursor(8,3);
		oled.print("Settings");
		oled.setCursor(87,1);
		oled.print("NA");
		oled.setCursor(87,2);
		oled.print("BMP/SPO");
		oled.setCursor(87,3);
		oled.print("Exit");
		oled.setCursor(x,y);
		oled.print(">");
		oled.update();
		if(digitalRead(4)==LOW && c>8)
		  return item;
		c++;
		delay(100);
	}
}


void buzz(int item)
{
  oled.setPower(1);
  oled.clear();
  oled.setScale(2);
  oled.setCursor(24, 1);
  oled.print("ALARM ");
  oled.print(item);
  oled.update();
  if(motoron)
  	digitalWrite(6,HIGH);
  delay(300);
  digitalWrite(6,LOW);
  oled.clear();
  delay(300);
}


void snoozealarm(int item)
{
	int h = a[item].hour;
	int m = a[item].min;
	a[item].min = (m + 5)%60;
	if(m > a[item].min)
		a[item].hour = (h + 1)%12;
	if(h > a[item].hour)
	{
		if(a[item].m=='A')
			a[item].m = 'P';
		else 
			a[item].m = 'A';
	}	
		
}

void checkalarm()
{
	if(c)
	{
		u.utime();
		c=0;
	}
	for(int item=0;item<6;item++)
	{
		if(a[item].check==true)
		{
			if(a[item].hour==u.hour && a[item].min==u.min && a[item].m==u.m[0])
			{
				while(true)
				{
					buzz(item+1);
					if(c)
					{
						u.utime();
						c=0;
					}
					if(digitalRead(3)==LOW)
					{
						a[item].check = false;
						oled.setPower(0);
						return ;
					}
					else if(digitalRead(4)==LOW || a[item].min!=u.min)
					{
						snoozealarm(item);
					}
				}
			}
		}
	}
}


void displayheartrate(int x,int y)
{
	oled.setScale(2);
	oled.setCursor(0,0);
	oled.print("BMP : ");
	if(x>35 && x<=120)
		oled.print(x);
	else
		oled.print("0  ");
	oled.setCursor(0,2);
	oled.print("SPO : ");
	if(y>85 && y<=100)
		oled.print(y);
	else 
		oled.print("0  ");
}

void heartrate()
{
	oled.clear();
	delay(500);
	pox.resume();
	int x=0,y=0;
	while(true)
	{
		pox.update();
		x= pox.getHeartRate();
      	y =pox.getSpO2();
      	displayheartrate(x,y);
      	if(digitalRead(3)==LOW ||digitalRead(4)==LOW)
      	{
      		pox.shutdown();
      		return ;
		}
	}
} 

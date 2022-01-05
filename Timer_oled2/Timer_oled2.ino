#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//2 menit dalam milisecond
#define COUNTDOWN_TIME  3600000ul

char
    szString[20];
byte
    jam, mins;
        
unsigned long
    timeTemp,
    timeNow,
    timeStart,
    timeElapsed,
    timeLeft;    

int frequency=1000; //Specified in Hz
int buzzPin=D4; //PIN
int timeOn=1000; //specified in milliseconds
int timeOff=1000; //specified in millisecods

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() 
{
    // initialize and clear display
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);

    Serial.begin(9600);
    timeStart = millis();
    jam = 1;
    mins = 1;
  
}//setup

void DoCountdown1()
{
    #define COUNTDOWN_TIME  3600000ul
    static unsigned long
        lastTimeNow = 0;
    static byte
        lastmins = 1;
        
    timeNow = millis();
    timeElapsed = timeNow - timeStart;
    
    if(jam == 0 && mins == 0 )
        return;
        
    timeLeft = COUNTDOWN_TIME - timeElapsed;
    Serial.println(timeLeft);
//    
//    jam = (byte)(timeLeft / 3600000ul);
//    timeTemp = timeLeft - (jam * 3600000ul);
    jam = (byte)(timeLeft / 3600000ul);
    timeTemp = timeLeft - (jam * 3600000ul);
    mins = (byte)(timeTemp / 60000ul);
    timeTemp = timeTemp - (mins * 60000ul);

    if( jam == 0 && mins == 0 )
    {
        display.clearDisplay();
        sprintf( szString, "**DONE**" );
        Serial.println( szString );
        display.setCursor( 10, 10 );
        display.print( szString );
        display.display();
         
    }//if
    else if( mins != lastmins )
    {
        lastmins = mins;
        display.clearDisplay();
        sprintf( szString, "%02d:%02d", jam, mins );
        Serial.println( szString );
        //
        display.setCursor( 10, 10 );
        display.print( szString );
        display.display();
    
    }//if
    
}//DoCountdown

void DoCountdown2()
{
    #define COUNTDOWN_TIME  7200000ul
    static unsigned long
        lastTimeNow = 0;
    static byte
        lastmins = 1;
        
    timeNow = millis();
    timeElapsed = timeNow - timeStart;
    
    if(jam == 0 && mins == 0 )
        return;
        
    timeLeft = COUNTDOWN_TIME - timeElapsed;
    Serial.println(timeLeft);
//    
//    jam = (byte)(timeLeft / 3600000ul);
//    timeTemp = timeLeft - (jam * 3600000ul);
    jam = (byte)(timeLeft / 3600000ul);
    timeTemp = timeLeft - (jam * 3600000ul);
    mins = (byte)(timeTemp / 60000ul);
    timeTemp = timeTemp - (mins * 60000ul);

    if( jam == 0 && mins == 0 )
    {
        display.clearDisplay();
        sprintf( szString, "**DONE**" );
        Serial.println( szString );
        display.setCursor( 10, 10 );
        display.print( szString );
        display.display();

        //buzzer
        tone(buzzPin, frequency);
        delay(timeOn);
        noTone(buzzPin);
        delay(timeOff);
         
    }//if
    else if( mins != lastmins )
    {
        lastmins = mins;
        display.clearDisplay();
        sprintf( szString, "%02d:%02d", jam, mins );
        Serial.println( szString );
        //
        display.setCursor( 10, 10 );
        display.print( szString );
        display.display();
    
    }//if
    
}//DoCountdown

void loop() 
{
    //DoCountdown1();
    DoCountdown2();
            
}//loop

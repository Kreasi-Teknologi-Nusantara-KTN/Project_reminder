#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "KTN";
const char* password = "ktnmantab03";
const char* mqtt_server = "test.mosquitto.org";

int c;

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


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  delay(1000);

  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    c = 1 ;
  } else if ((char)payload[0] == '2'){
    c = 2 ;
  } else{
    Serial.print("unknown");
    }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("tes1/pub", "hello world");
      // ... and resubscribe
      client.subscribe("tes1/sub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() 
{
     Serial.begin(115200);
    // initialize and clear display
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    
    timeStart = millis();
    jam = 1;
    mins = 1;

  
}//setup

void DoCountdown1()
{
    int COUNTDOWN_TIME = 3600000ul;
    static unsigned long
        lastTimeNow = 0;
    static byte
        lastmins = 1;
        
    timeNow = millis();
    timeElapsed = timeNow - timeStart;
    
    if(jam == 0 && mins == 0 )
        return;
        
    timeLeft = COUNTDOWN_TIME - timeElapsed;

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

void DoCountdown2()
{
    int COUNTDOWN_TIME = 7200000ul;
    static unsigned long
        lastTimeNow = 0;
    static byte
        lastmins = 1;
        
    timeNow = millis();
    timeElapsed = timeNow - timeStart;
    
    if(jam == 0 && mins == 0 )
        return;
        
    timeLeft = COUNTDOWN_TIME - timeElapsed;

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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (c == 1){
  DoCountdown1();
//  Serial.println("1");
  }else if (c == 2){
  DoCountdown2();
//  Serial.println("2");
  }else{
        display.clearDisplay();
        display.setCursor( 10, 10 );
        display.print("Terhubung");
        display.display();
    }
            
}//loop

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
// Update these with values suitable for your network.

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char szString[20];

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const char* ssid = "KTN";
const char* password = "ktnmantab03";
const char* mqtt_server = "broker.emqx.io";

int buzzPin = D4;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

//battery indicator
int analogInPin  = A0;    // Analog input pin
int sensorValue; 
float calibration = 0.36; // Check Battery voltage using multimeter & add/subtract the value
String batt_str;
char batt[20];

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
  display.clearDisplay();
  display.setCursor( 10, 10 );
  display.print("Terhubung");
  display.display();
  Serial.println(WiFi.localIP());
  digitalWrite(BUILTIN_LED, LOW);
  digitalWrite(buzzPin, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String response;


  for (int i = 0; i < length; i++) {
       response += (char)payload[i];
       
//lcd oled
        display.clearDisplay();
//        sprintf( szString, "%02d", response );
        Serial.println( szString );
        display.setCursor( 10, 10 );
        display.print( response );
        Serial.println(response);
        display.display();
  }


//  notifikasi led
  if ( response == "timeout") {
          digitalWrite(D5, HIGH);
          digitalWrite(D6, LOW);
          digitalWrite(D7, LOW);
          tone(buzzPin, 200);
    } 
    else if ( response == "start" ) {
          digitalWrite(D5, LOW);
          digitalWrite(D6, LOW);
          digitalWrite(D7, HIGH);
          
    }
    else if(response == "half"){
          digitalWrite(D5, LOW);
          digitalWrite(D6, HIGH);
          digitalWrite(D7, LOW);
    }else if (response == "selesai"){
          digitalWrite(buzzPin, LOW);
    }else{}


//buzzer warning  
if ( response == "warning") {
     tone(buzzPin, 200);
    } else if ( response == "off" ) {
          digitalWrite(buzzPin, LOW);
    }else{}
 
  Serial.println();
 
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
      display.clearDisplay();
      display.setCursor( 10, 10 );
      display.print("Ready");
      display.display();
//      // Once connected, publish an announcement...
//      client.publish("devreminder/node6/sub", "hello world");
      // ... and resubscribe
      client.subscribe("devreminder/node6/pub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void batteryIndicator(){
  sensorValue = analogRead(analogInPin);
  voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor
 
  bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
 
  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }

//  Serial.print("Analog Value = ");
//  Serial.print(sensorValue);
//  Serial.print("\t Output Voltage = ");
//  Serial.print(voltage);
//  Serial.print("\t Battery Percentage = ");
//  Serial.println(bat_percentage);
batt_str = String(bat_percentage);
batt_str.toCharArray(batt, batt_str.length()+1);
client.publish("devreminder/node6/sub", batt);
//  delay(1000);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void setup() {
  digitalWrite(buzzPin, LOW);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setCursor( 10, 10 );
  display.display();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  pinMode(buzzPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  batteryIndicator()
}

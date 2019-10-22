
// Include Libraries
#include "Arduino.h"
#include "NewPing.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//max distance to trigger echo(in cm)
#define MAX_DISTANCE 500
//min distance to count as a presence(in cm), preventing false readings when opening the door
#define MIN_DISTANCE 12
//minimum distance from floor(set on initialization)
#define MIN_DISTANCE_FROM_FLOOR 20

//timeout to clear count on PIR
#define PIR_COUNT_TIMEOUT 900000


// NodeMCU pin definitions, and some of their quirks
#define D0 16 //no interrrupt/PWM, high at boot, deepsleep restore
#define D1 5 
#define D2 4
#define D3 0  //enters programming mode when low on boot, external pullup
#define D4 2  //tries to boot from SD card when low on boot, external pullup?
#define D5 14 
#define D6 12
#define D7 13 
#define D8 15 //external pull-down, fails boot if high
#define RX 3  //needed for serial input, high on boot?
#define TX 1  //outputs debug data on boot, needed for serial output, fails boot if low
#define D9 9  //requires DIO/Dout mode to use, freezes flash when down (!), pulled up? 
#define D10 10//requires DIO/Dout mode to use, protects flash from writes when down?

#define INSIDE_SR04_TRIG	D5
#define OUTSIDE_SR04_TRIG	D6

#define INSIDE_SR04_ECHO	D0
#define OUTSIDE_SR04_ECHO  D1

#define INSIDE_PIR_OUT D2

#define BUZZER D8

#define DISABLE_BUZZ_ON_COUNT_CHANGE_JUMPER D7 //connect to ground on failure

// Replace with your network credentials
const char* ssid = "UFPI";
const char* password = "";
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

String page = "";
char text[1000];


// Global variables and defines

// objects
NewPing Inside_SR04(INSIDE_SR04_TRIG, INSIDE_SR04_ECHO, MAX_DISTANCE);
NewPing Outside_SR04(OUTSIDE_SR04_TRIG, OUTSIDE_SR04_ECHO, MAX_DISTANCE);


void setup(){
 pinMode(INSIDE_PIR_OUT, INPUT);
 pinMode(BUZZER, OUTPUT);
 digitalWrite(BUZZER,HIGH);
 delay(50);
 digitalWrite(BUZZER,LOW);
 delay(100);
 Serial.begin(74880);

//these already are set on instance declaration
// pinMode(HCSR04_1_PIN_TRIG, OUTPUT);
// pinMode(HCSR04_1_PIN_ECHO, INPUT);
// pinMode(HCSR04_2_PIN_TRIG, OUTPUT);
// pinMode(HCSR04_2_PIN_ECHO, INPUT);
 connectWifi();
 digitalWrite(BUZZER,HIGH);
 delay(50);
 digitalWrite(BUZZER,LOW);
 delay(100);

 connectServe();

 digitalWrite(BUZZER,HIGH);
 delay(50);
 digitalWrite(BUZZER,LOW);
 delay(100);
}


//scheduler info
//sense: a task reading from sensors.
/* FINISH LATER
//buzzer: a task updating buzzer playback.

typedef struct{
  struct buzz* next;
  bool output;
  uint16_t duration;//ms
  }

struct buzz* CurrentBuzz = {
  next = 0;
  output = 0;
  duration = 0; 
}
*/
int64_t nextsense=0;
int64_t nextpirreadout=0;
int InsideDistance;
int OutsideDistance;
int64_t LastPIRDetection;



void loop(){
bool taskran = false;
if ((int64_t)(nextsense-millis())<0){
  taskran = true;
  nextsense = millis()+50;
   InsideDistance = Inside_SR04.ping_cm();
  delay(16);
   OutsideDistance = Outside_SR04.ping_cm();
  Serial.print("Inside sensor dist(cm):");
  Serial.println(InsideDistance);
  Serial.print("Outside sensor dist(cm):");
  Serial.println(OutsideDistance);
  if (OutsideDistance == NO_ECHO){
    digitalWrite(BUZZER,HIGH);
    delay(200);
    digitalWrite(BUZZER,LOW);
    delay(200);
    }
  } 
  if ((int64_t)(nextpirreadout-millis())<0){
    taskran=true;
    nextpirreadout = millis()+1000;
    if (digitalRead(INSIDE_PIR_OUT)==HIGH){
      LastPIRDetection = millis();
      }
    }
  server.handleClient();
if (!taskran){delay(50);};//ideally we'd use deep sleep, but this does require constant monitoring - at least with an opened door.

}

void connectWifi(){
 WiFi.begin(ssid, password); //begin WiFi connection
 Serial.println("");
 
 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 
 Serial.println("");
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
}

void connectServe(){
  server.on("/data.txt", [](){
   sprintf(text, "<p>Inside SR04: %d cm<p>Outside SR04: %d cm<p>Last PIR detection: %d s ago", InsideDistance, OutsideDistance, ((int64_t)(LastPIRDetection-millis())/1000));
   server.send(200, "text/html", text);
 });
 server.on("/", [](){
   page = "<h1>PIR Sensor to NodeMCU/h1><h1>Data:</h1> <h1 id=\"data\">""</h1>\r\n";
   page += "<script>\r\n";
   page += "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, 1000);\r\n";
   page += "function loadData(url, callback){\r\n";
   page += "var xhttp = new XMLHttpRequest();\r\n";
   page += "xhttp.onreadystatechange = function(){\r\n";
   page += " if(this.readyState == 4 && this.status == 200){\r\n";
   page += " callback.apply(xhttp);\r\n";
   page += " }\r\n";
   page += "};\r\n";
   page += "xhttp.open(\"GET\", url, true);\r\n";
   page += "xhttp.send();\r\n";
   page += "}\r\n";
   page += "function updateData(){\r\n";
   page += " document.getElementById(\"data\").innerHTML = this.responseText;\r\n";
   page += "}\r\n";
   page += "</script>\r\n";
   server.send(200, "text/html", page);
});
 
 server.begin();
 Serial.println("Web server started!");
}

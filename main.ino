
// Include Libraries
#include "Arduino.h"
#include "NewPing.h"
#include "EventLog.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//max distance to trigger echo(in cm)
#define MAX_DISTANCE 500
//min distance to count as a presence(in cm), preventing false readings when opening the door
#define MIN_DISTANCE 12
//minimum distance from floor(set on initialization)
#define MIN_DISTANCE_FROM_FLOOR 20

//timeout to clear count on PIR in milliseconds
#define PIR_COUNT_TIMEOUT 900*1000


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

#define INSIDE_SR04_TRIG  D5
#define OUTSIDE_SR04_TRIG D6

#define INSIDE_SR04_ECHO  D0
#define OUTSIDE_SR04_ECHO  D1

#define INSIDE_PIR_OUT D2

#define BUZZER D8

#define DISABLE_BUZZ_ON_COUNT_CHANGE_JUMPER D7 //connect to ground on failure

// Replace with your network credentials
const char* ssid = "UFPI";
const char* password = "";
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
EventLog EventLogger;

String page = "";
char text[1000];
int count = 0;
int in = 0;
int out = 0;
int inP = 0;
int outP = 0;
int inL = 0;
int outL = 0;
int pIn = 0;
int pOut = 0;

int currentState = 0;
int previousState = 0;
int currentState2 = 0;
int previousState2 = 0;


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
  pinMode(INSIDE_SR04_TRIG, OUTPUT);
  pinMode(INSIDE_SR04_ECHO, INPUT);
  pinMode(OUTSIDE_SR04_TRIG, OUTPUT);
  pinMode(OUTSIDE_SR04_ECHO, INPUT);
  connectWifi();
  digitalWrite(BUZZER,HIGH);
  delay(50);
  digitalWrite(BUZZER,LOW);
  delay(100);

  connectServe();

  int i = 0;
  int mn = 0;
  int mx = 0;
  int midIn = 0;
  int midOut = 0;
  //in = 160;
  //out = 160;
  in = Inside_SR04.ping_cm();
  out = Outside_SR04.ping_cm();
//  while(i < 1000){
//    midIn += Inside_SR04.ping_cm();
//    midOut += Outside_SR04.ping_cm();
//    if(
//    i +=1;
//    yield();
//  }
  inP = in + 5;
  outP = out + 5;
  inL = in - 5;
  outL = out - 5;
  
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
  delay(100);
  long distance;
  distance = Inside_SR04.ping_cm();
  delay(100);
  long distance2;
  distance2 = Outside_SR04.ping_cm();
 // object entering in the system

//  Serial.print("In:");
//  Serial.println(in);
//  Serial.print("out:");
//  Serial.println(out);
//
  Serial.print("Sensor in - now:");
  Serial.println(distance);
  Serial.print("Sensor out - now:");
  Serial.println(distance2);
  long newDistance = 0;
  long newDistance2 = 0;

  if (distance < in && distance < inP && distance < inL){
    currentState = 1;
  }
  else {
    currentState = 0;
  }
  if(currentState != previousState){
    while(currentState == 1) {
      delay(50);
      distance2 = Outside_SR04.ping_cm();
      Serial.println(distance2);
      if (distance2 < out && distance2 < outP && distance2 < outL){
        currentState2 = 1;
        distance2 = Outside_SR04.ping_cm();
        //wait until lost detection
//        while(distance2 < out && distance2 < outP && distance2 < outL){
//          delay(50);
//          distance2 = Outside_SR04.ping_cm();
//          yield();
//        }
      }
      else {
        currentState2 = 0;
      }
      if(currentState2 != previousState2){
        if(currentState2 == 1) {
          while(distance2 < out && distance2 < outP && distance2 < outL){
            delay(50);
            distance2 = Outside_SR04.ping_cm();
            yield();
          }
          while(distance < in && distance < inP && distance < inL){
            delay(50);
            distance = Inside_SR04.ping_cm();
            yield();
          }
          count += 1;
          pIn += 1;
          digitalWrite(BUZZER,HIGH);
          delay(50);
          digitalWrite(BUZZER,LOW);
          Serial.print("count:");
          Serial.println(count);
          delay(1000); // pause for 1/2 second
         }
        server.handleClient();
        return;
       }
      delay(50);
      distance = Inside_SR04.ping_cm();
      if (distance < in && distance < inP && distance < inL){
        currentState = 1;
      }
      else{
        currentState = 0;
      }       
      yield();
    }
  }

  if (distance2 < out && distance2 < outP && distance2 < outL){
    currentState2 = 1;
  }
  else {
    currentState2 = 0;
  }
  if(currentState2 != previousState2){
    while (currentState2 == 1) {
      delay(50);
      distance = Inside_SR04.ping_cm();
      Serial.println(distance);
      if (distance < in && distance < inP && distance < inL){
        currentState = 1;
//        distance = Inside_SR04.ping_cm();
//        while(distance < in && distance < inP && distance < inL){
//          delay(50);
//          distance = Inside_SR04.ping_cm();
          Serial.println("Esperando in");
//          yield();
        //}
      }
      else {
        currentState = 0;
      }
      if(currentState != previousState){
        if(currentState == 1) {
          while(distance < in && distance < inP && distance < inL){
            delay(50);
            distance = Inside_SR04.ping_cm();
            yield();
          }          
          while(distance2 < out && distance2 < outP && distance2 < outL){
            delay(50);
            distance2 = Outside_SR04.ping_cm();
            yield();
          }
          count -= 1;
          pOut += 1;
          digitalWrite(BUZZER,HIGH);
          delay(50);
          digitalWrite(BUZZER,LOW);
          Serial.print("count:");
          Serial.println(count);
          delay(1000); // pause for 1/2 second
           }
        server.handleClient();         
        return;
      }
      delay(50);
      distance2 = Outside_SR04.ping_cm();
      if (distance2 < out && distance2 < outP && distance2 < outL){
        currentState2 = 1;
      }
      else{
        currentState2 = 0;
      }           
      yield();
    }
  }
  server.handleClient();
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
    sprintf(text, "<p>Número de pessoas: %d Pessoas<p>Entraram: %d pessoas<p>Sairam: %d pessoas<p>Last PIR detection: %d s ago", count, pIn, pOut, ((int64_t)(LastPIRDetection-millis())/1000));
    server.send(200, "text/html", text);
  });

  //std::bind(func* a, args) creates a 0-arg function equivalent to a(args)
  server.on(F("/logs/"), std::bind(&EventLog::SendLogs, &EventLogger, &server));
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

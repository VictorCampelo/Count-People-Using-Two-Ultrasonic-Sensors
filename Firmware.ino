
// Include Libraries
#include "Arduino.h"
#include "NewPing.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Pin Definitions
#define HCSR04_1_PIN_TRIG	4
#define HCSR04_1_PIN_ECHO	5
#define HCSR04_2_PIN_TRIG	2
#define HCSR04_2_PIN_ECHO	0

// Replace with your network credentials
const char* ssid = "<Your-WiFi-SSID>";
const char* password = "<Your-WiFi-Password>";
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

String page = "";
String text = "";


// Global variables and defines

// object initialization
NewPing hcsr04_1(HCSR04_1_PIN_TRIG,HCSR04_1_PIN_ECHO);
NewPing hcsr04_2(HCSR04_2_PIN_TRIG,HCSR04_2_PIN_ECHO);


void setup(void){
 Serial.begin(115200);
 pinMode(HCSR04_1_PIN_TRIG, OUTPUT);
 pinMode(HCSR04_1_PIN_ECHO, INPUT);
 pinMode(HCSR04_2_PIN_TRIG, OUTPUT);
 pinMode(HCSR04_2_PIN_ECHO, INPUT);
 connectWifi();
 connectServe();
}
 
void loop(void){
 // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  
  //sensor ultrasonic 1
  digitalWrite(HCSR04_1_PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(HCSR04_1_PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HCSR04_1_PIN_TRIG, LOW);
  //sensor ultrasonic 2
  digitalWrite(HCSR04_2_PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(HCSR04_2_PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HCSR04_2_PIN_TRIG, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  duration1 = pulseIn(HCSR04_1_PIN_ECHO, HIGH);
  duration2 = pulseIn(HCSR04_2_PIN_ECHO, HIGH);

  // convert the time into a distance
  inches1 = microsecondsToInches(duration1);
  cm1 = microsecondsToCentimeters(duration1);
  // convert the time into a distance
  inches2 = microsecondsToInches(duration2);
  cm2 = microsecondsToCentimeters(duration2);

  //here we've to define the distances value for the inferences wheter people throuput the door 

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  delay(100);

  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second

  //send the number of people in the room
  //add the appropriates calculations for this approach

  sendHeight(cm);
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
   if(isTriggered){
     text = "Person Detected!";
   }else{
     text = "";
   }
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
long microsecondsToInches(long microseconds) 
{ // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}
long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

 void sendHeight(float cm)
{  
  WiFiClient tclient;//not to be confused with "client" in PubSub{}, and wclient for mqtt
   if (tclient.connect(serverTS, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   //Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(cm);
   postStr += "\r\n\r\n";
   
   tclient.print("POST /update HTTP/1.1\n");
   tclient.print("Host: api.thingspeak.com\n");
   tclient.print("Connection: close\n");
   tclient.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   tclient.print("Content-Type: application/x-www-form-urlencoded\n");
   tclient.print("Content-Length: ");
   tclient.print(postStr.length());
   tclient.print("\n\n");
   tclient.print(postStr);
   delay(1000);
   
   }//end if

 tclient.stop();
}//end send to ts

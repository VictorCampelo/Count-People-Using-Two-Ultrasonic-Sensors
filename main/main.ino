
// Include Libraries
#include "Arduino.h"
#include "NewPing.h"
#include "EventLog.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h> //lib para conectar o wifi do ESP201
#include <ESP8266WiFiMulti.h>//lib para as funções addAP e  run
#include <SPI.h>  //protocolo síncrono de dados serial

//max dist_in to trigger echo(in cm)
#define MAX_DISTANCE 500
//min dist_in to count as a presence(in cm), preventing false readings when opening the door
#define MIN_DISTANCE 12
//minimum dist_in from floor(set on initialization)
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
//porta 5000 do protocolo TCP, deve ser a mesma utilizada pelo servidor
const uint16_t port = 9999;
//endereço ip, deve ser o mesmo utilizado pelo servidor
const char * host = "10.94.15.69";
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

int state_in = 0;
int flag = 0;
int state_out = 0;

ESP8266WiFiMulti WiFiMulti; //cria uma instância da classe ESP8266WiFiMulti
IPAddress local_IP(192, 168, 10, 110); //Cria uma instância da classe IPAddress e define o ip do servidor

// Global variables and defines

// objects
NewPing Inside_SR04(INSIDE_SR04_TRIG, INSIDE_SR04_ECHO, MAX_DISTANCE);
NewPing Outside_SR04(OUTSIDE_SR04_TRIG, OUTSIDE_SR04_ECHO, MAX_DISTANCE);


void setup() {
  pinMode(INSIDE_PIR_OUT, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  delay(50);
  digitalWrite(BUZZER, LOW);
  delay(100);
  Serial.begin(74880);

  //these already are set on instance declaration
  pinMode(INSIDE_SR04_TRIG, OUTPUT);
  pinMode(INSIDE_SR04_ECHO, INPUT);
  pinMode(OUTSIDE_SR04_TRIG, OUTPUT);
  pinMode(OUTSIDE_SR04_ECHO, INPUT);
  //connectWifi();
  wifiMultiConecte();
  digitalWrite(BUZZER, HIGH);
  delay(50);
  digitalWrite(BUZZER, LOW);
  delay(100);

  connectServe();

  int i = 0;
  int mn = 0;
  int mx = 0;
  int midIn = 0;
  int midOut = 0;
  in = 90;
  out = 90;

  //this code above is a test to create erros ration values

  //in = Inside_SR04.ping_cm();
  //out = Outside_SR04.ping_cm();
  //  while(i < 1000){
  //    midIn += Inside_SR04.ping_cm();
  //    midOut += Outside_SR04.ping_cm();
  //    if(
  //    i +=1;
  //    yield();
  //  }
  inP = in + 10;
  outP = out + 10;
  inL = in - 10;
  outL = out - 10;

  digitalWrite(BUZZER, HIGH);
  delay(50);
  digitalWrite(BUZZER, LOW);
  delay(100);
}

int64_t nextsense = 0;
int64_t nextpirreadout = 0;
int InsideDistance;
int OutsideDistance;
int64_t LastPIRDetection;

void loop() {

  bool taskran = false;
  long dist_in;
  //delay(20);
  dist_in = Inside_SR04.ping_cm();
  long dist_out;
  //delay(20);
  dist_out = Outside_SR04.ping_cm();

  Serial.print("Sensor in:");
  Serial.println(dist_in);
  Serial.print("Sensor out:");
  Serial.println(dist_out);
  //this fuction above not is more necessary yet
  //server.handleClient();

  if (dist_in < in && dist_in < inP && dist_in < inL) {
    state_in = 1;
    while (state_in == 1) {
      delay(20);
      dist_out = Outside_SR04.ping_cm();
      if (dist_out < out && dist_out < outP && dist_out < outL) {
        state_out = 1;
        Serial.println("Esperando out");
      }
      else {
        state_out = 0;
      }
      if (state_out != flag) {
        if (state_out == 1) {
          while (dist_out < out && dist_out < outP && dist_out < outL) {
            delay(20);
            dist_out = Outside_SR04.ping_cm();
            yield();
          }
          while (dist_in < in && dist_in < inP && dist_in < inL) {
            delay(20);
            dist_in = Inside_SR04.ping_cm();
            yield();
          }
          count += 1;
          pIn += 1;
          digitalWrite(BUZZER, HIGH);
          delay(50);
          digitalWrite(BUZZER, LOW);
          Serial.print("count:");
          Serial.println(count);
          sendDataToDatabase(count);
          delay(1000); // pause for 1/2 second
        }
        return;
      }
      delay(20);
      dist_in = Inside_SR04.ping_cm();
      if (dist_in < in && dist_in < inP && dist_in < inL) {
        state_in = 1;
      }
      else {
        return;
      }
      yield();
    }
  }

  else if (dist_out < out && dist_out < outP && dist_out < outL) {
    state_out = 1;
    while (state_out == 1) {
      delay(20);
      dist_in = Inside_SR04.ping_cm();
      if (dist_in < in && dist_in < inP && dist_in < inL) {
        state_in = 1;
        Serial.println("Esperando in");
      }
      else {
        state_in = 0;
      }
      if (state_in != flag) {
        if (state_in == 1) {
          while (dist_in < in && dist_in < inP && dist_in < inL) {
            delay(20);
            dist_in = Inside_SR04.ping_cm();
            yield();
          }
          while (dist_out < out && dist_out < outP && dist_out < outL) {
            delay(20);
            dist_out = Outside_SR04.ping_cm();
            yield();
          }
          count -= 1;
          pOut += 1;
          digitalWrite(BUZZER, HIGH);
          delay(50);
          digitalWrite(BUZZER, LOW);
          Serial.print("count:");
          Serial.println(count);
          sendDataToDatabase(count);
          delay(1000); // pause for 1/2 second
        }
        return;
      }
      dist_out = Outside_SR04.ping_cm();
      if (dist_out < out && dist_out < outP && dist_out < outL) {
        state_out = 1;
      }
      else {
        return;
      }
      yield();
    }
  }
}

void sendDataToDatabase(int count){
  //inicializa a lib do cliente
  WiFiClient client;
  if (client.connect(host, port)){    //Envioda temperatura atual
      client.println(" ");
      client.println("1"); //id
      client.println(count); //NumPeople
      client.println("fim");
   }
}

void wifiMultiConnect(){
  //configura modo como estação
  WiFi.mode(WIFI_STA);
  
  //parametros: WiFi.softAP(nomeDoAccessPoint, senhaRede)
  //redeVisivel: a rede pode ou não aparecer para outros serviços 
  WiFiMulti.addAP("UFPI", "");
  
  //enquanto o cliente não estiver conectado, escreve "."
  while (WiFiMulti.run() != WL_CONNECTED) 
   Serial.print(".");  
}

void connectWifi() {
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

void connectServe() {
  server.on("/data.txt", []() {
    sprintf(text, "<p>Número de pessoas: %d Pessoas<p>Entraram: %d pessoas<p>Sairam: %d pessoas<p>Last PIR detection: %d s ago", count, pIn, pOut, ((int64_t)(LastPIRDetection - millis()) / 1000));
    server.send(200, "text/html", text);
  });

  //std::bind(func* a, args) creates a 0-arg function equivalent to a(args)
  server.on(F("/logs/"), std::bind(&EventLog::SendLogs, &EventLogger, &server));
  server.on("/", []() {
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
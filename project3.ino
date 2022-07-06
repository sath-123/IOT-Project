#include <WiFi.h>
#include <SPI.h>
#include<WebServer.h>
#include "time.h"
#include "HTTPClient.h"
#include "ESP32_MailClient.h"
WebServer serve(80);
String data="";
String page="";
#define emailSenderAccount    "sathvikamaheshspr@gmail.com"
#define emailSenderPassword   "Spr@1438"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "[ALERT] ESP32 Temperature"

// Default Recipient Email Address
String inputMessage = "sathvikamaheshspr@gmail.com";
String enableEmailChecked = "checked";
String inputMessage2 = "true";
// Default Threshold Temperature Value
String inputMessage3 = "25.0";
String lastTemperature;


const char* ssid     = "Galaxy A31BA08";
const char* password = "chikky12345";

String apiKey="DQT3021Z6IKL6TCF";

//const char* server="api.thingspeak.com";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 3600;
String cse_ip="192.168.72.94";
String cse_port="8080";
String server="http://"+cse_ip+":"+cse_port+"/~/in-cse/in-name/";
String ae="ProjectUltrasensor";
String cnt="node1";

WiFiClient client;
int pir=0;
int trig=0;

#define buzzerpin 26
#define ldrpin 35
#define pirpin 12
const int trigPin = 32;
const int echoPin = 33;
int s=0;


#define Speed 0.034
#define cm_inch 0.393701

long duration;
float distanceCm;
String distanceInch;


//int pir;
void createCI(String& val)
{
  HTTPClient http;
  http.begin(server + ae + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
  Serial.println(code);
  if (code == -1) {
     Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
    http.end();
}



void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   pinMode(buzzerpin,OUTPUT);
   digitalWrite(buzzerpin,LOW);
   pinMode(ldrpin,INPUT);
   pinMode(pirpin,INPUT);
   pinMode(trigPin, OUTPUT); 
   pinMode(echoPin, INPUT);
   digitalWrite(trigPin,LOW); 
   Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  serve.on("/data.txt", [](){
     if(trig){
       data = "Person Detected!";
     }else{
       data = "Person not detected";
     }
     serve.send(200, "text/html", data);
   });
   serve.on("/", [](){
     page = "<h1>PIR Sensor to NodeMCU</h1><h1>Data:</h1> <h1 id=\"data\">""</h1>\r\n";
     page +="<input id=\"rd1\" type=\"radio\">sathvika</input>\r\n";
    
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
     page += "function fn1(){\r\n";
     page += " trig = 1;\r\n";
     page += "}\r\n";
     
     page += "</script>\r\n";
     serve.send(200, "text/html", page);
  });

  serve.begin();
  
  
  
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();

  //disconnect WiFi as it's no longer needed
 // WiFi.disconnect(true);
 // WiFi.mode(WIFI_OFF);
}

void loop() {
  // put your main code here, to run repeatedly:
    int ldr=digitalRead(ldrpin);
    
   
    //int ldr=0;
    Serial.println(ldr);
     if(ldr==1 || s==1)
    {
      pir=digitalRead(pirpin);
      if(pir==0)
      {
        Serial.println("motion not detected");
        digitalWrite(buzzerpin,LOW);
         client.print("<h1>No motion detected </h1>");
         trig=0;
      }
      else
      {
        Serial.println("motion detected");
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        distanceCm = duration * Speed;
        distanceInch = distanceCm * cm_inch; 
        Serial.print("Distance (cm): ");
        Serial.println(distanceCm);
        Serial.print("Distance (inch): ");
        Serial.println(distanceInch);
        createCI(distanceInch);
        //sendEmailNotification();
        client.print("<h1> motion detected </h1>");
        Serial.println();
       // printLocalTime();
        digitalWrite(buzzerpin,HIGH);
        delay(3000);
        digitalWrite(buzzerpin,LOW);
        trig=1;
      }}
     
     delay(1000); }
   serve.handleClient();
  
    

}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  Serial.println();
}

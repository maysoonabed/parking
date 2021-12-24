#include <FirebaseESP32.h>
#include <WiFi.h>

#include <stdio.h>
#include <stdlib.h>

static const int irPin1 = 13;
static const int irPin2 = 0;
int count=0;
int x;
#define FIREBASE_HOST "mysonp-e7463-default-rtdb.firebaseio.com/" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "F1X5G48TyRCdhAFxx4OPxEWcDoOSIpbyvn5kLuWh"

#define API_KEY "AIzaSyBPREyYkTUDZiXmPYn8mzOnhf0ZqHi-lUs"


#define WIFI_SSID  "Tobasi2020" // "Samah"   // "Abdelqader"
#define WIFI_PASSWORD  "135790bb"//"Abdor177" "smsmaia1998"  


// Define Firebase Data Object
FirebaseData firebaseData;

// Root Path
String path = "/Esp/region/Tammon/84a2132d";

void setup() {
  //Serial.begin(115200);
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  
  //Serial.print("Connecting to ");
  //Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  // Print local IP address and start web server
  //Serial.println("");
  //Serial.println("WiFi connected.");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
 
}

void loop() {
  Firebase.getInt(firebaseData, "/Esp/irflag");
  x = firebaseData.intData() ;
  Firebase.getInt(firebaseData, path);
  count = firebaseData.intData() ;
  //Serial.println(x);
  //Serial.println(x!="");
  //**************************************************
 if(x!=0){
  //**************************************************
   if(!digitalRead(irPin1)){
     //Serial.println("IR1:");
     //Serial.println(digitalRead(irPin1));
     Firebase.setInt(firebaseData,"Esp/IR's/ir1",1);
     delay(500);
     Firebase.setInt(firebaseData,"Esp/IR's/ir1",0);
     //Serial.println("After Delay");
     if ( !digitalRead(irPin2) ){
        //Serial.println("Entering into bus");
        if(count<7)
           count++;
        Firebase.setInt(firebaseData,path,count);
        delay(100);
        //Serial.print("No. of persons inside the bus: ");
        //Serial.println(count);
      }
    }
  //**************************************************
  if (!digitalRead(irPin2)){
     //Serial.println("IR2:");
     //Serial.println(digitalRead(irPin2));
     Firebase.setInt(firebaseData,"Esp/IR's/ir2",1);
     delay(500);
     Firebase.setInt(firebaseData,"Esp/IR's/ir2",0);
     //Serial.println("After Delay");
     if ( !digitalRead(irPin1) ){
       //Serial.println("Exiting from bus");
       if(count>0)
        count--;
       Firebase.setInt(firebaseData,path,count);
       delay(100);
       //Serial.print("No. of persons inside the bus: ");
       //Serial.println(count);
    }
  }
 } 
}

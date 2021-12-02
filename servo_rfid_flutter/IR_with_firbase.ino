#include <FirebaseESP32.h>
#include <WiFi.h>

#include <stdio.h>
#include <stdlib.h>

static const int irPin1 = 13;
static const int irPin2 = 0;
int count=0;
boolean state1 = true;
boolean state2 = true;
boolean insideState = false;
boolean outsideIr=false;
boolean isPeopleExiting=false;
int i=1;

#define FIREBASE_HOST "mysonp-e7463-default-rtdb.firebaseio.com/" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "F1X5G48TyRCdhAFxx4OPxEWcDoOSIpbyvn5kLuWh"

#define API_KEY "AIzaSyBPREyYkTUDZiXmPYn8mzOnhf0ZqHi-lUs"


#define WIFI_SSID    "Tobasi2020"   // "Abdelqader"
#define WIFI_PASSWORD   "135790bb"  //"Abdor177"


// Define Firebase Data Object
FirebaseData firebaseData;

// Root Path
String path = "/Esp/PassengerCNT";

void setup() {

 
  Serial.begin(115200);
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
 
}

void loop() {

  if (!digitalRead(irPin1) && i==1 && state1){//1
     outsideIr=true;
     delay(50);
     i++;
     state1 = false;
  }

   if (!digitalRead(irPin2) && i==2 &&   state2){//2
     Serial.println("Entering into bus");
     outsideIr=true;
     delay(50);
     i = 1 ;
     count++;
     Firebase.setInt(firebaseData, "/Esp/PassengerCNT",count);
     delay(1000);
     Serial.print("No. of persons inside the bus: ");
     Serial.println(count);
     state2 = false;
  }

   if (!digitalRead(irPin2) && i==1 && state2 ){//3
     outsideIr=true;
     delay(50);
     i = 2 ;
     state2 = false;
  }

  if (!digitalRead(irPin1) && i==2 && state1 ){//4
     Serial.println("Exiting from bus");
     outsideIr=true;
     delay(50);
     count--;
     Firebase.setInt(firebaseData, "/Esp/PassengerCNT",count);
        delay(1000);
       Serial.print("No. of persons inside the bus: ");
       Serial.println(count);
     i = 1;
     state1 = false;
  }  

    if (digitalRead(irPin1)){//5
     state1 = true;
    }

     if (digitalRead(irPin2)){//6
     state2 = true;
    }
  
}


void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else
  {
    Serial.println(data.payload());
  }
}

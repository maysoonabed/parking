#include <FirebaseESP32.h>
#include <WiFi.h>
#include <Servo.h>

#include <SPI.h>
#include <MFRC522.h>

#include <stdio.h>
#include <stdlib.h>

#define RST_PIN         22          // Configurable, see typical pin layout above
#define SS_PIN          21        // Configurable, see typical pin layout above
#define SS_PIN2         5         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_out(SS_PIN2, RST_PIN);  // Create MFRC522 instance


static const int servoPin = 4;
static const int servoPin2 = 13;

Servo servo1;
Servo servOut;


#define FIREBASE_HOST "mysonp-e7463-default-rtdb.firebaseio.com/" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "F1X5G48TyRCdhAFxx4OPxEWcDoOSIpbyvn5kLuWh"

#define API_KEY "AIzaSyBPREyYkTUDZiXmPYn8mzOnhf0ZqHi-lUs"


#define WIFI_SSID    "maysoon"   // "Abdelqader"
#define WIFI_PASSWORD   "maysoonAbd"  //"Abdor177"


// Define Firebase Data Object
FirebaseData firebaseData;

// Root Path
String path = "/Esp/ledStatus";
 
char  buf [10];
char buffer [100];

void setup() {

 
  Serial.begin(115200);
  servo1.attach(servoPin);
  servOut.attach(servoPin2);


 
 
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

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  SPI.begin();            // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  mfrc522_out.PCD_Init();    // Init MFRC522
  mfrc522_out.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

}

void loop() {

  mfrc522.PCD_Init(); 
    if (  mfrc522.PICC_IsNewCardPresent()&& mfrc522.PICC_ReadCardSerial()) {
  
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
   buffer[0] =  '\0';

      Serial.print("UID In: ");
      for (int i = 0; i < mfrc522.uid.size; i++) {
    
       ultoa (mfrc522.uid.uidByte[i],buf,HEX);
        strcat(buffer, buf);

      }
      Serial.println(buffer);
      Serial.println();
      std::string str(buffer);
      Firebase.setString(firebaseData, "/Esp/rfid/in",buffer);
        delay(1000); 

  if (Firebase.getDouble(firebaseData, path + "/ledOn"))
  {
    Serial.println("PASSED");
 
    Serial.println();

    if (firebaseData.intData() == 1)
    {
 for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        servo1.write(posDegrees);
         delay(20);
    }

    for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
        servo1.write(posDegrees);
         delay(20);
    }      Serial.println("Servo on");
    }

    else
    {
      Serial.println("Servo off");
      }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Push integer test...");
  }
 
  mfrc522_out.PCD_Init(); 
    if (  mfrc522_out.PICC_IsNewCardPresent()&& mfrc522_out.PICC_ReadCardSerial()) {
  
  MFRC522::PICC_Type piccType = mfrc522_out.PICC_GetType(mfrc522_out.uid.sak);
   buffer[0] =  '\0';

      Serial.print("UID Out: ");
      for (int i = 0; i < mfrc522_out.uid.size; i++) {
    
       ultoa (mfrc522_out.uid.uidByte[i],buf,HEX);
        strcat(buffer, buf);

      }
      Serial.println(buffer);
      Serial.println();
      std::string str(buffer);
      Firebase.setString(firebaseData, "/Esp/rfid/out",buffer);
        delay(1000); 


         if (Firebase.getDouble(firebaseData, path + "/ledOff"))
  {
    Serial.println("PASSED");
 
    Serial.println();

    if (firebaseData.intData() == 1)
    {
 for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        servOut.write(posDegrees);
         delay(20);
    }

    for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
        servOut.write(posDegrees);
         delay(20);
    }      Serial.println("Servo on");
    }

    else
    {
      Serial.println("Servo off");
      }
  }

   
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Push integer test...");


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

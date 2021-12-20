#include <Servo.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <stdio.h>
#include <stdlib.h>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char * strsIn[50] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49"};

String strs[50] = {""};
String ins[50] = {""};
String rgs[50] = {""};

char *str;
int hay  ;
const char *reg;
int num[12] =  { 0 };
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define RST_PIN         22          // Configurable, see typical pin layout above
#define SS_PIN          2
#define SS_PIN2         5
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_out(SS_PIN2, RST_PIN);

StaticJsonDocument<200> doc;
StaticJsonDocument<2000> doc1;


static const int servoPin = 4;
static const int servoPin2 = 13;
Servo servo1;
Servo servOut;
//********Ultrasonic*********
const int trigPin = 0;//5
const int echoPin = 16;//18
const int ZamorPin = 14;
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;

bool USflag = true;
bool t1 = false;
bool t = false;

hw_timer_t * timer2 = NULL;
 
void IRAM_ATTR onTimer2() {
  digitalWrite(ZamorPin, HIGH);
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.println("******************************");
}
//***************************

#define FIREBASE_HOST "mysonp-e7463-default-rtdb.firebaseio.com/" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "F1X5G48TyRCdhAFxx4OPxEWcDoOSIpbyvn5kLuWh"


#define WIFI_SSID "Abdelqader"
#define WIFI_PASSWORD "Abdor177"



//#define WIFI_SSID    "maysoon"
//#define WIFI_PASSWORD   "maysoonAbd"
//#define WIFI_SSID "Tobasi2020"
//#define WIFI_PASSWORD "135790bb"


const char* serverName = "http://192.168.1.114/parking/phpfiles/esp.php";


// Define Firebase Data Object
FirebaseData firebaseData;
int regNum;
// Root Path
String path = "/Esp/ledStatus";

char  buf [10];
char buffer [100];
//********Timer*********
hw_timer_t * timer = NULL;
hw_timer_t * timer1 = NULL;



void IRAM_ATTR onTimer() {
  t = true;
}
void IRAM_ATTR onTimer1() {
  t1 = true;

}
//**********************
void setup() {


  // initialize LCD
  lcd.begin();
  // turn on LCD backlight
  lcd.backlight();

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
  mfrc522_out.PCD_Init();
  mfrc522_out.PCD_DumpVersionToSerial();



  //*****************************************************************************************************

  HTTPClient http1;
  http1.begin("http://192.168.1.114/parking/phpfiles/espRegions.php");
  http1.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode1 = http1.POST("");

  if (httpResponseCode1 > 0) {
    //check for a return code - This is more for debugging.
    String response1 = http1.getString();
    Serial.println(httpResponseCode1);
    Serial.println(response1);

    //////////////////////
    // Deserialize the JSON document
    DeserializationError error1 = deserializeJson(doc1, response1);

    // Test if parsing succeeds.
    if (error1) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error1.f_str());
      return;
    }

    regNum = doc1["num"];
    for (int r = 0; r < regNum; r++) {

      const char *x = doc1[strsIn[r]]["engname"];
      Serial.println(x);

      strs[r] = x;
    }

    ////////////////////////////
  }
  else {
    Serial.print("Error on sending post");
    Serial.println(httpResponseCode1);
  }
  //closde the HTTP request.
  http1.end();


  //***************************************************************************************************
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));


  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    lcd.print(strs[i]);
    lcd.setCursor(10, i);
    lcd.print(strs[i + 4]);
    lcd.setCursor(8, i);
    lcd.print("0");
    lcd.setCursor(19, i);
    lcd.print("0");
  }

  //********Ultrasonic*********
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(ZamorPin, OUTPUT);
  //***************************
}

void loop() {

  if (t == true) {
    Firebase.setString(firebaseData, "/Esp/rfid/in", "0");
    for (int posDegrees = 0; posDegrees <= 100; posDegrees++) {
      //Serial.println(posDegrees);
      servo1.write(posDegrees);
      delay(20);
    }
    t = false;
  }

  if (t1 == true) {
    Firebase.setString(firebaseData, "/Esp/rfid/out", "0");
    for (int posDegrees = 100; posDegrees >= 10; posDegrees--)
    {
      servOut.write(posDegrees);
      delay(20);
    }
    t1 = false;
  }

  //*************************************************************************
  mfrc522.PCD_Init();
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    buffer[0] =  '\0';
    Serial.print("UID In: ");
    for (int i = 0; i < mfrc522.uid.size; i++) {
      ultoa (mfrc522.uid.uidByte[i], buf, HEX);
      strcat(buffer, buf);
    }
    Serial.println(buffer);
    Serial.println();
    std::string str(buffer);

    //**************************
    String rc(buffer);

    //Open a connection to the server
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST("&uid=" + rc);

    if (httpResponseCode > 0) {
      //check for a return code - This is more for debugging.
      String response = http.getString();
      Serial.println(httpResponseCode);
      //Serial.println(response);

      //////////////////////
      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, response);

      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      hay = doc["hay"];

      ////////////////////////////
    }
    else {
      Serial.print("Error on sending post");
      Serial.println(httpResponseCode);
    }
    //closde the HTTP request.
    http.end();

    for (int x = 0; x < 50; x++) {
      if (ins[x] == rc) {
        hay = 0;
        Serial.println("Already in");
        break;
      }
    }
    //*********************

    if (hay == 1)
    {
      reg = doc["region"];
      for (int x = 0; x < 50; x++) {
        if (ins[x] == "") {
          ins[x] = rc;
          rgs[x] = reg;
          break;
        }
      }
      Firebase.setString(firebaseData, "/Esp/rfid/in", buffer);
      //*************************************************************************

      for (int posDegrees = 100; posDegrees >= 0; posDegrees--) {
        servo1.write(posDegrees);
        delay(20);
      }
      //********Timer*********
      /* Use 1st timer of 4 */
      /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
      timer = timerBegin(0, 80, true);// hw_timer_t * timerBegin(uint8_t num, uint16_t divider, bool countUp)
      /* Attach onTimer function to our timer */
      timerAttachInterrupt(timer, &onTimer, true);
      /* Set alarm to call onTimer function every second 1 tick is 1us => 1 second is 1000000us */
      /* Repeat the alarm (third parameter) */
      timerAlarmWrite(timer, 8000000, false);//true
      /* Start an alarm */
      timerAlarmEnable(timer);
      //**********************
      for (int i = 0; i < 50; i++)
      {
        if (strs[i] == reg) {
          num[i]++;
          lcd.setCursor(11 * (i / 4) + 8, i % 4);
          lcd.print(num[i]);
        }
      }
    }
    else
    {
      Serial.println("Servo off");
    }
    Serial.println("------------------------------------");
    Serial.println("Push integer test...");
    //*************************************************************************

  }
  //*************************************************************************
  mfrc522_out.PCD_Init();
  if (mfrc522_out.PICC_IsNewCardPresent() && mfrc522_out.PICC_ReadCardSerial()) {
    MFRC522::PICC_Type piccType = mfrc522_out.PICC_GetType(mfrc522_out.uid.sak);
    buffer[0] =  '\0';
    Serial.print("UID Out: ");
    for (int i = 0; i < mfrc522_out.uid.size; i++) {
      ultoa (mfrc522_out.uid.uidByte[i], buf, HEX);
      strcat(buffer, buf);
    }
    Serial.println(buffer);
    Serial.println();
    std::string str(buffer);
    //////////////////////////////////////////////////////////////////////
    String ro(buffer);
    bool b = true;
    for (int x = 0; x < 50; x++) {
      if (ins[x] == ro) {
        b = false;
        Firebase.setString(firebaseData, "/Esp/rfid/out", buffer);
        for (int posDegrees = 0; posDegrees <= 100; posDegrees++)
        {
          servOut.write(posDegrees);
          delay(20);
        }


        for (int i = 0; i < 50; i++)
        {
          if (strs[i] ==  rgs[x]) {
            num[i]--;
            lcd.setCursor(9 * (i / 8) + 8, i % 4);
            lcd.print(num[i]);
            break;
          }
        }


        Serial.println("Servo on");
        ins[x] = "";
        rgs[x] = "";
        break;
      }
    }
    if (b)     Serial.println("Not in");


    ///////////////////////////////////////////////////

    Serial.println("------------------------------------");
    Serial.println("Push integer test...");


  }
  //*************************************************************************

  //********Ultrasonic*********
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  //  Serial.print("Distance (cm): ");
  //  Serial.println(distanceCm);
  //  Serial.println("******************************");
  if (distanceCm <= 5) {
    //أول مرة بفحص اذا المسافة أقل من 5 وفيه حال أول مرة ببلش التايمر فيه حال ما كان أول مرة بضل التايمر يعد
    //********Timer*********
    if (USflag == true) { //First Time
      timer2 = timerBegin(2, 80, true);
      timerAttachInterrupt(timer2, &onTimer2, true);
      timerAlarmWrite(timer2, 100000000, false);//true//300sec=3minute
      timerAlarmEnable(timer2);
      USflag = false;
    }
    //**********************
  } else {
    USflag = true;
    digitalWrite(ZamorPin, LOW);
  }
  //***************************

}

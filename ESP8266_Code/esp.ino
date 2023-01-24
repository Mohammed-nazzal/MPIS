/**
 * This is a kind of unit test for DEV for now
 * It contains many of the public methods
 * 
 */
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Firebase_ESP_Client.h>
#include "DHT.h"
#include <time.h>
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#include <stdio.h>
#include <NTPClient.h>
DHT sensor2(D5, DHT22);
DHT sensor1(D3, DHT22);
#define USEOTA
#define FIREBASE_HOST "https://multi-pay-irrigation-system-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyDhrHsPBHnOoUqhRvLxbxsbslPRDxF9hyk"
// enable OTA
#ifdef USEOTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif
const long utcOffsetInSeconds = 7200;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;
const char* modes[] = { "NULL", "STA", "AP", "STA+AP" };
int sm1,sm2;
unsigned long mtime = 0;
int ENA1 = D7;
int ENA2 = D6;
int pay1=1;
int pay2=1;
int ferstart;
int ferflag=1;

int S0 = D0;
int S1 = D4;
int flag =1;
int RelayPin2 = D2;
int RelayPin1= D1;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);



WiFiManager wm;


// TEST OPTION FLAGS
bool TEST_CP         = false; // always start the configportal, even if ap found
int  TESP_CP_TIMEOUT = 90; // test cp timeout

bool TEST_NET        = true; // do a network test after connect, (gets ntp time)
bool ALLOWONDEMAND   = true; // enable on demand
int  ONDDEMANDPIN    = 0; // gpio for button
bool WMISBLOCKING    = true; // use blocking or non blocking mode, non global params wont work in non blocking





void saveWifiCallback(){
  Serial.println("[CALLBACK] saveCallback fired");
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("[CALLBACK] configModeCallback fired");
  
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  // wm.stopConfigPortal();
}

void bindServerCallback(){
  wm.server->on("/custom",handleRoute); // this is now crashing esp32 for some reason
  // wm.server->on("/info",handleRoute); // you can override wm!
}

void handleRoute(){
  Serial.println("[HTTP] handle route");
  wm.server->send(200, "text/plain", "hello from user code");
}

void handlePreOtaUpdateCallback(){
  Update.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("CUSTOM Progress: %u%%\r", (progress / (total / 100)));
  });
}

void setup() {
  // WiFi.mode(WIFI_STA); // explicitly set mode, esp can default to STA+AP
  /* Assign the api key (required) */
  Serial.begin(115200);
  
    pinMode(ENA1, OUTPUT);
    pinMode(ENA2, OUTPUT);
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    sensor1.begin();
    sensor2.begin();
    timeClient.begin();
  // put your setup code here, to run once:
  
   digitalWrite(RelayPin1, HIGH);
   digitalWrite(RelayPin2, HIGH);
   analogWrite(ENA1, 0);
   analogWrite(ENA2, 0);
	

  	

  // Serial.setDebugOutput(true);

  Serial.println("\n Starting");
  // WiFi.setSleepMode(WIFI_NONE_SLEEP); // disable sleep, can improve ap stability

  Serial.println("Error - TEST");
  Serial.println("Information- - TEST");

  Serial.println("[ERROR]  TEST");
  Serial.println("[INFORMATION] TEST");  

  wm.setDebugOutput(true);
  wm.debugPlatformInfo();

  WiFiManagerParameter custom_html("<p style=\"color:pink;font-weight:Bold;\">This Is Custom HTML</p>"); // only custom html
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", "", 6);
  WiFiManagerParameter custom_token("api_token", "api token", "", 16);
  WiFiManagerParameter custom_tokenb("invalid token", "invalid token", "", 0); // id is invalid, cannot contain spaces
  WiFiManagerParameter custom_ipaddress("input_ip", "input IP", "", 15,"pattern='\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}'"); // custom input attrs (ip mask)
  WiFiManagerParameter custom_input_type("input_pwd", "input pass", "", 15,"type='password'"); // custom input attrs (ip mask)

  const char _customHtml_checkbox[] = "type=\"checkbox\""; 
  WiFiManagerParameter custom_checkbox("my_checkbox", "My Checkbox", "T", 2, _customHtml_checkbox,WFM_LABEL_AFTER);

  const char *bufferStr = R"(
  <!-- INPUT CHOICE -->
  <br/>
  <p>Select Choice</p>
  <input style='display: inline-block;' type='radio' id='choice1' name='program_selection' value='1'>
  <label for='choice1'>Choice1</label><br/>
  <input style='display: inline-block;' type='radio' id='choice2' name='program_selection' value='2'>
  <label for='choice2'>Choice2</label><br/>

  <!-- INPUT SELECT -->
  <br/>
  <label for='input_select'>Label for Input Select</label>
  <select name="input_select" id="input_select" class="button">
  <option value="0">Option 1</option>
  <option value="1" selected>Option 2</option>
  <option value="2">Option 3</option>
  <option value="3">Option 4</option>
  </select>
  )";

  WiFiManagerParameter custom_html_inputs(bufferStr);

  // callbacks
  wm.setAPCallback(configModeCallback);
  wm.setWebServerCallback(bindServerCallback);
  wm.setSaveConfigCallback(saveWifiCallback);
  wm.setSaveParamsCallback(saveParamCallback);
  wm.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);

  // add all your parameters here
  wm.addParameter(&custom_html);
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_token);
  wm.addParameter(&custom_tokenb);
  wm.addParameter(&custom_ipaddress);
  wm.addParameter(&custom_checkbox);
  wm.addParameter(&custom_input_type);

  wm.addParameter(&custom_html_inputs);

  // set values later if you want
  custom_html.setValue("test",4);
  custom_token.setValue("test",4);

 
  const char* menuhtml = "<form action='/custom' method='get'><button>Custom</button></form><br/>\n";
  wm.setCustomMenuHTML(menuhtml);

  
  wm.setDarkMode(true);
  std::vector<const char *> menu = {"wifi","wifinoscan","info","param","custom","close","sep","erase","update","restart","exit"};
  wm.setMenu(menu); // custom menu, pass vector
  
  
  wm.setHostname(("WM_"+wm.getDefaultAPName()).c_str());
 
  if(!WMISBLOCKING){
    wm.setConfigPortalBlocking(false);
  }

  wm.setConfigPortalTimeout(120);
  
  
  wm.setBreakAfterConfig(true); 


  wifiInfo();


  if(!wm.autoConnect("IRRIGATION_SYSTEM_WIFI_CON","12345678")) {
    Serial.println("failed to connect and hit timeout");
  }
  else if(TEST_CP) {
    // start configportal always
    delay(1000);
    Serial.println("TEST_CP ENABLED");
    wm.setConfigPortalTimeout(TESP_CP_TIMEOUT);
    wm.startConfigPortal("IRRIGATION_SYSTEM_WIFI_CON","12345678");
  }
  else {
    //if you get here you have connected to the WiFi
     Serial.println("connected...yeey :)");
  }
  
  wifiInfo();
  pinMode(ONDDEMANDPIN, INPUT_PULLUP);

  #ifdef USEOTA
    ArduinoOTA.begin();
  #endif

  config.api_key = FIREBASE_AUTH;
  

  /* Assign the RTDB URL (required) */
  config.database_url = FIREBASE_HOST;
 
/* Sign up */
if (Firebase.signUp(&config, &auth, "", "")){
  Serial.println("ok");
  
}
else{
  Serial.printf("%s\n", config.signer.signupError.message.c_str());
}

  Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
}

void wifiInfo(){
  // can contain gargbage on esp32 if wifi is not ready yet
  Serial.println("[WIFI] WIFI INFO DEBUG");
  // WiFi.printDiag(Serial);
  Serial.println("[WIFI] SAVED: " + (String)(wm.getWiFiIsSaved() ? "YES" : "NO"));
  Serial.println("[WIFI] SSID: " + (String)wm.getWiFiSSID());
  Serial.println("[WIFI] PASS: " + (String)wm.getWiFiPass());
  Serial.println("[WIFI] HOSTNAME: " + (String)WiFi.getHostname());
}

void loop() {

    timeClient.update();
    int day=timeClient.getDay();
    int h=timeClient.getHours();
    int m=timeClient.getMinutes();
    int hou,min,endhou,endmin;
    String Mode1,Mode2;
    if (Firebase.RTDB.getString(&fbdo, "/Pay1/Mode")) {
        Mode1 = fbdo.stringData();
        if (Firebase.RTDB.getString(&fbdo, "/Pay2/Mode")) {
        Mode2 = fbdo.stringData();
        Serial.print("Pay2 Mode: ");Serial.println(Mode2);
        Serial.print("Pay1 Mode: ");Serial.println(Mode1);
     }}

else {
  Serial.println(fbdo.errorReason());
}
int irrNow1H,irrNow1M,irrNow2H,irrNow2M;

if (Firebase.RTDB.getInt(&fbdo, "/irrigateNow1/endHour")) {

   irrNow1H  = fbdo.intData();
   if (irrNow1H != 99){
    digitalWrite(RelayPin1, LOW);//pay1 
    digitalWrite(RelayPin2, HIGH);
    analogWrite(ENA1, 255);
    analogWrite(ENA2, 255);
                          
    if (Firebase.RTDB.getInt(&fbdo, "/irrigateNow1/endMinute")) {
        irrNow1M  = fbdo.intData();
        if(h==irrNow1H){
            if(m==irrNow1M){
                Firebase.RTDB.setInt(&fbdo, "/irrigateNow1/endHour",99);
                digitalWrite(RelayPin1, HIGH);//pay1 
                digitalWrite(RelayPin2, HIGH);
                analogWrite(ENA1, 0);
                analogWrite(ENA2, 0);
             }
         }
    }


   }
}
 

 
if (Firebase.RTDB.getInt(&fbdo, "/irrigateNow2/endHour")) {

    irrNow2H  = fbdo.intData();
    if (irrNow2H != 99){
    digitalWrite(RelayPin1, HIGH);
    digitalWrite(RelayPin2, LOW);
    analogWrite(ENA1, 255);
    analogWrite(ENA2, 255);
   if (Firebase.RTDB.getInt(&fbdo, "/irrigateNow2/endMinute")) {
    irrNow2M  = fbdo.intData();
     if(h==irrNow2H){
       if(m==irrNow2M){
         Firebase.RTDB.setInt(&fbdo, "/irrigateNow2/endHour",99);
     digitalWrite(RelayPin1, HIGH);//pay1 
                           digitalWrite(RelayPin2, HIGH);
                          analogWrite(ENA1, 0);
                           analogWrite(ENA2, 0);
     }
    }
}


   }
}
 




int shmode ,shmode2;
shmode=3;
shmode2=3;
if(Mode1=="Automatic")shmode=0;
if(Mode1=="EveryDay")shmode=1;
if (Mode1=="EveryNumDays")shmode=2;

if(Mode2=="Automatic")shmode2=0;
if(Mode2=="EveryDay")shmode2=1;
if (Mode2=="EveryNumDays")shmode2=2;


switch (shmode)
{
      case 0 : 
if (sm1 < 70 && sensor1.readTemperature() >15 && flag && pay2){

digitalWrite(RelayPin1, LOW);
digitalWrite(RelayPin2, HIGH);
Serial.println("case 0 for pay one");
 analogWrite(ENA1, 255);
 pay1=0;
}
else if((sm1 >= 70 || sensor1.readTemperature() <= 15) && flag && pay2){
digitalWrite(RelayPin1, HIGH);
digitalWrite(RelayPin2, HIGH);
Serial.println("case 0 for pay one");
 analogWrite(ENA1,0);
pay1=1;
}

 break ;


      case 1 :
      int numoftimes;
          if (Firebase.RTDB.getInt(&fbdo, "Pay1/numOfTimes")){
            numoftimes=fbdo.intData();
          }
               
                   
                    if (Firebase.RTDB.getArray(&fbdo, "Pay1/data/")){
                     FirebaseJsonArray arr =fbdo.jsonArray();
                     FirebaseJsonData hours,minutes,endHours,endMinutes,fer;
int hh[numoftimes] ;int mm[numoftimes] ;int eh[numoftimes] ;int em[numoftimes] ;int ferr[numoftimes];
                     for(int i=0 ;i<numoftimes;i++)
                     {  
                
                     arr.get(hours, "[" + String(i) + "]" + "/data/hour");
                     arr.get(minutes, "[" + String(i) + "]" + "/data/minute");
                     arr.get(endHours, "[" + String(i) + "]" + "/data/endHour");
                     arr.get(endMinutes, "[" + String(i) + "]" + "/data/endMinute");
                     arr.get(endMinutes, "[" + String(i) + "]" + "/data/endMinute");
                     arr.get(fer, "[" + String(i) + "]" + "/data/fertilizer");
                     hh[i]=hours.intValue;
                     mm[i]=minutes.intValue;
                     eh[i]=endHours.intValue;
                     em[i]=endMinutes.intValue;
                     ferr[i]=fer.intValue;

                     int o=(mm[i]+ferr[i])%60;
              int now=(h*60 )+ m;
int start = (hh[i]*60)+mm[i];
 int end=(eh[i]*60) +em[i];

                     if( start <= now < end){
                      
Serial.println("Start engines");
                         digitalWrite(RelayPin1, LOW);//pay1 
                           digitalWrite(RelayPin2, HIGH);
                          analogWrite(ENA1, 255);
                           analogWrite(ENA2, 255);
                           flag=0;
                            if(ferflag){
                           ferstart=(m+ferr[i])%60;
                           ferflag=0;
                           }
                           flag=0;
                           if (end -now <5 )
                           analogWrite(ENA2, 0);

                         
                   
                     }
                     if (ferstart <= m)
                            analogWrite(ENA2, 0);

                           if(now >= end){
                             ferflag=1;
Serial.println(" engines Off");
                                digitalWrite(RelayPin1, HIGH);
                           digitalWrite(RelayPin2, HIGH);
                          analogWrite(ENA1, 0);
                           analogWrite(ENA2, 0);
                           flag=1;
                             }
                           }
                     
                     }

                     
            break;

case 2: 
int nod;//number of days
int nd; //next day
int sd; //strat day
int ss;
  if (Firebase.RTDB.getInt(&fbdo, "Pay1/startday")){
            sd=fbdo.intData();
          }
            if (Firebase.RTDB.getInt(&fbdo, "Pay1/Numofdays")){
            nod=fbdo.intData();
          }
          ss=sd+1;
          

nd=(sd+nod)%7;
            
  
          
          if(sd==day){
 int numoftimes;
          if (Firebase.RTDB.getInt(&fbdo, "Pay1/numOfTimes")){
            numoftimes=fbdo.intData();
          }
               
                
                    if (Firebase.RTDB.getArray(&fbdo, "Pay1/data/")){
                     FirebaseJsonArray arr =fbdo.jsonArray();
                     FirebaseJsonData hours,minutes,endHours,endMinutes,fer;
int hh[numoftimes] ;int mm[numoftimes] ;int eh[numoftimes] ;int em[numoftimes] ;int ferr[numoftimes];
                     for(int i=0 ;i<numoftimes;i++)
                     {  
                      
                     arr.get(hours, "[" + String(i) + "]" + "/data/hour");
                     arr.get(minutes, "[" + String(i) + "]" + "/data/minute");
                     arr.get(endHours, "[" + String(i) + "]" + "/data/endHour");
                     arr.get(endMinutes, "[" + String(i) + "]" + "/data/endMinute");
                         arr.get(fer, "[" + String(i) + "]" + "/data/fertilizer");
                     hh[i]=hours.intValue;
                     mm[i]=minutes.intValue;
                     eh[i]=endHours.intValue;
                     em[i]=endMinutes.intValue;
                        ferr[i]=fer.intValue;
                        int o=(mm[i]+ferr[i])%60;

                        int now=(h*60 )+ m;
int start = (hh[i]*60)+mm[i];
 int end=(eh[i]*60) +em[i];

                     if( start <= now < end){
                   
Serial.println("Start engines");
                         digitalWrite(RelayPin1, LOW);
                           digitalWrite(RelayPin2, HIGH);
                          analogWrite(ENA1, 255);
                           analogWrite(ENA2, 255);
                           flag=0;
                            if(ferflag){
                           ferstart=(m+ferr[i])%60;
                           ferflag=0;
                           }
                           flag=0;
                           if (end -now <5 )
                           analogWrite(ENA2, 0);
                           
                     }


                     if (ferstart <= m)
                            analogWrite(ENA2, 0);

                           if(now >= end){
                             ferflag=1;
Serial.println(" engines Off");
                                digitalWrite(RelayPin1, HIGH);
                           digitalWrite(RelayPin2, HIGH);
                          analogWrite(ENA1, 0);
                           analogWrite(ENA2, 0);
                            if (Firebase.RTDB.setInt(&fbdo, "Pay1/startday",  nd)){

Serial.println("FIREBASE Nextday for Pay1 WRITE DONE");

  }
  else{
Serial.println("FIREBASE nextday for pay1 WRITE ERROR");

  }
                           flag=1;
                             }
                           }
                     
                     }

                     }
           

          



          break;
case 3:  analogWrite(ENA1, 0); analogWrite(ENA2, 0); break;
}
    
switch(shmode2){



  case 0 :
if (sm2 < 70 && sensor2.readTemperature() >15 && flag && pay1){

digitalWrite(RelayPin1, HIGH);//pay2
digitalWrite(RelayPin2, LOW);
Serial.println("case 0 for pay two");
 analogWrite(ENA1, 255);
 pay2=0;

}
else if((sm2 >= 70 || sensor2.readTemperature()) <= 15 && flag && pay1 ){
digitalWrite(RelayPin1, HIGH);//pay2
digitalWrite(RelayPin2, HIGH);
Serial.println("pay 2 Auto OFF");
 analogWrite(ENA1, 0);
 pay2=1;

}
 break ;

      case 1 :
      int numoftimes;
          if (Firebase.RTDB.getInt(&fbdo, "Pay2/numOfTimes")){
            numoftimes=fbdo.intData();
          }
               
              
                    if (Firebase.RTDB.getArray(&fbdo, "Pay2/data/")){
                     FirebaseJsonArray arr =fbdo.jsonArray();
                     FirebaseJsonData hours,minutes,endHours,endMinutes,fer;
int hh[numoftimes] ;int mm[numoftimes] ;int eh[numoftimes] ;int em[numoftimes];int ferr[numoftimes];
                     for(int i=0 ;i<numoftimes;i++)
                     {  
                       
                     arr.get(hours, "[" + String(i) + "]" + "/data/hour");
                     arr.get(minutes, "[" + String(i) + "]" + "/data/minute");
                     arr.get(endHours, "[" + String(i) + "]" + "/data/endHour");
                     arr.get(endMinutes, "[" + String(i) + "]" + "/data/endMinute");
                     arr.get(fer, "[" + String(i) + "]" + "/data/fertilizer");
                     hh[i]=hours.intValue;
                     mm[i]=minutes.intValue;
                     eh[i]=endHours.intValue;
                     em[i]=endMinutes.intValue;
                      ferr[i]=fer.intValue;
                        int o=(mm[i]+ferr[i])%60;

int now=(h*60 )+ m;
int start = (hh[i]*60)+mm[i];
 int end=(eh[i]*60) +em[i];


                     if( start <= now < end){
                       
Serial.println("Start engines");
                      digitalWrite(RelayPin1, HIGH);//pay2
                      digitalWrite(RelayPin2, LOW);
                      analogWrite(ENA1, 255);
                      analogWrite(ENA2, 255);
                      flag=0;
                      if(ferflag){
                        ferstart=(m+ferr[i])%60;
                        ferflag=0;
                      }
                      flag=0;
                      if (end -now <5 )
                      analogWrite(ENA2, 0);
                     }

              if (ferstart <= m)
                            analogWrite(ENA2, 0);

                           if(now >= end){
                             ferflag=1;
Serial.println(" engines Off");
                                digitalWrite(RelayPin1, HIGH);
                           digitalWrite(RelayPin2, HIGH);
                          analogWrite(ENA1, 0);
                           analogWrite(ENA2, 0);
                           flag=1;
                           
                             }
                           }
                     
                     }

                     
            break;

case 2 : 
int nod;//number of days
int nd; //next day
int sd; //strat day
int ss;
  if (Firebase.RTDB.getInt(&fbdo, "Pay2/startday")){
            sd=fbdo.intData();
            Serial.print("sd= ");Serial.println(sd);
          }
            if (Firebase.RTDB.getInt(&fbdo, "Pay2/Numofdays")){
            nod=fbdo.intData();
            Serial.print("nod= ");Serial.println(nod);
          }
          ss=sd+1;
          

nd=(sd+nod)%7;

    
          
          if(sd==day){
 int numoftimes;
          if (Firebase.RTDB.getInt(&fbdo, "Pay2/numOfTimes")){
            numoftimes=fbdo.intData();
          }
               
                   Data * arr= new Data[numoftimes];
                    if (Firebase.RTDB.getArray(&fbdo, "Pay2/data/")){
                     FirebaseJsonArray arr =fbdo.jsonArray();
                     FirebaseJsonData hours,minutes,endHours,endMinutes,fer;
int hh[numoftimes] ;int mm[numoftimes] ;int eh[numoftimes] ;int em[numoftimes] ;int ferr[numoftimes];
                     for(int i=0 ;i<numoftimes;i++)
                     {  
                       Serial.println("im here");
                     arr.get(hours, "[" + String(i) + "]" + "/data/hour");
                     arr.get(minutes, "[" + String(i) + "]" + "/data/minute");
                     arr.get(endHours, "[" + String(i) + "]" + "/data/endHour");
                     arr.get(endMinutes, "[" + String(i) + "]" + "/data/endMinute");
                     arr.get(fer, "[" + String(i) + "]" + "/data/fertilizer");
                     hh[i]=hours.intValue;
                     mm[i]=minutes.intValue;
                     eh[i]=endHours.intValue;
                     em[i]=endMinutes.intValue;
                       ferr[i]=fer.intValue;
                        

int now=(h*60 )+ m;
int start = (hh[i]*60)+mm[i];
 int end=(eh[i]*60) +em[i];

                     if( start <= now < end){
                      
Serial.println("Start engines pay2");
                         digitalWrite(RelayPin1, HIGH);//pay2
                         digitalWrite(RelayPin2, LOW);
                          analogWrite(ENA1, 255);
                           analogWrite(ENA2, 255);

                           if(ferflag){
                           ferstart=(m+ferr[i])%60;
                           ferflag=0;
                           }
                           flag=0;
                           if (end -now <5 )
                           analogWrite(ENA2, 0);
                          
                     }
                             if (ferstart <= m)
                            analogWrite(ENA2, 0);

                           if(now >= end){
                             ferflag=1;
Serial.println(" engines Off pay2");
                                digitalWrite(RelayPin1, HIGH);
                           digitalWrite(RelayPin2, HIGH);
                          analogWrite(ENA1, 0);
                           analogWrite(ENA2, 0);
                           flag=1;
                                 if (Firebase.RTDB.setInt(&fbdo, "Pay2/startday",  nd)){

Serial.println("FIREBASE Nextday for Pay2 WRITE DONE");

  }
  else{
Serial.println("FIREBASE nextday for Pay2 WRITE ERROR");

  }
                             
                           }
                     
                     }

                     }
           

          }



          break;
case 3:  analogWrite(ENA1, 0); analogWrite(ENA2, 0); break;

}
  //motor code.............................

  //END MOTOR CODE....................
//LDR CODE.............................
int sensorValue;
float voltage;
digitalWrite(S0, LOW);
digitalWrite(S1, LOW);

sensorValue = analogRead(A0); 
voltage = sensorValue * (5.0 / 1023.0); 

	Serial.print("LDR2: "); 
  Serial.println(voltage); 
  
 if (Firebase.RTDB.setFloat(&fbdo, "PayStat/LDR2", voltage)){

Serial.println("FIREBASE LDR2 WRITE DONE");

  }
  else{
Serial.println("FIREBASE LDR2 WRITE ERROR");

  }

  digitalWrite(S0, HIGH);
digitalWrite(S1, LOW);

sensorValue = analogRead(A0); 
voltage = sensorValue/10; 

	Serial.print("Soil_2: "); 
  Serial.println(voltage); 
  sm2=voltage;
   if (Firebase.RTDB.setFloat(&fbdo, "PayStat/SM2", voltage)){

Serial.println("FIREBASE SM2 WRITE DONE");

  }
  else{
Serial.println("FIREBASE SM2 WRITE ERROR");

  }


  digitalWrite(S0, LOW);
digitalWrite(S1, HIGH);

sensorValue = analogRead(A0); 
voltage = sensorValue * (5.0 / 1023.0); 

	Serial.print("LDR1: "); 
  Serial.println(voltage); 
  
 if (Firebase.RTDB.setFloat(&fbdo, "PayStat/LDR1", voltage)){

Serial.println("FIREBASE LDR1 WRITE DONE");

  }
  else{
Serial.println("FIREBASE LDR1 WRITE ERROR");

  }

  digitalWrite(S0, HIGH);
digitalWrite(S1, HIGH);

sensorValue = analogRead(A0); 
voltage = sensorValue /10; 

	Serial.print("Soil_1: "); 
  Serial.println(voltage); 
  sm1=voltage;
   if (Firebase.RTDB.setFloat(&fbdo, "PayStat/SM1", voltage)){

Serial.println("FIREBASE SM1 WRITE DONE");

  }
  else{
Serial.println("FIREBASE SM1 WRITE ERROR");

  }

  //END LDR CODE...................

  //begin of DHT22------------------------------------
float sensor1_temp,
      sensor2_temp;

float sensor1_humi,
      sensor2_humi;

        sensor1_temp = sensor1.readTemperature();
  sensor1_humi = sensor1.readHumidity();
    sensor2_temp = sensor2.readTemperature();
  sensor2_humi = sensor2.readHumidity();


  if (Firebase.RTDB.setFloat(&fbdo, "PayStat/Heat1", sensor1_temp)){

Serial.println("FIREBASE HEAT1 WRITE DONE");

  }
  else{
Serial.println("FIREBASE HEAT1 WRITE ERROR");

  }
   if (Firebase.RTDB.setFloat(&fbdo, "PayStat/Humidity1", sensor1_humi)){

Serial.println("FIREBASE Humidity1 WRITE DONE");

  } else{
Serial.println("FIREBASE Humidity1 WRITE ERROR");

  }
  
  
  if (Firebase.RTDB.setFloat(&fbdo, "PayStat/Heat2", sensor2_temp)){

Serial.println("FIREBASE HEAT2 WRITE DONE");

  }
  else{
Serial.println("FIREBASE HEAT2 WRITE ERROR");

  }
   if (Firebase.RTDB.setFloat(&fbdo, "PayStat/Humidity2", sensor2_humi)){

Serial.println("FIREBASE Humidity2 WRITE DONE");

  } else{
Serial.println("FIREBASE Humidity2 WRITE ERROR");

  }
  

    Serial.print("Sensor-1 Readings:  ");
  Serial.print(sensor1_temp,1); Serial.print(String(char(176))+"C  ");
  Serial.print(sensor1_humi,1); Serial.println("%RH");
  Serial.println();
   
  Serial.print("Sensor-2 Readings:  ");
  Serial.print(sensor2_temp,1); Serial.print(String(char(176))+"C  ");
  Serial.print(sensor2_humi,1); Serial.println("%RH");
  Serial.println();
   delay(3000);
//end of DHT22 -----------------------------------------

  if(!WMISBLOCKING){
    wm.process();
  }

  #ifdef USEOTA
  ArduinoOTA.handle();
  #endif
  // is configuration portal requested?
  if (ALLOWONDEMAND && digitalRead(ONDDEMANDPIN) == LOW ) {
    delay(100);
    if ( digitalRead(ONDDEMANDPIN) == LOW ){
      Serial.println("BUTTON PRESSED");
      wm.setConfigPortalTimeout(140);
      wm.setParamsPage(false); // move params to seperate page, not wifi, do not combine with setmenu!
      if (!wm.startConfigPortal("OnDemandAP","12345678")) {
        Serial.println("failed to connect and hit timeout");
        
        delay(3000);
      }
    }
    else {
      //if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");

      getTime();
    }
  }

  // every 10 seconds
  if(millis()-mtime > 10000 ){
    if(WiFi.status() == WL_CONNECTED){
     
      getTime();
    }
    else{ Serial.println("No Wifi");  
  

   
        
       }   mtime = millis();
  }
  // put your main code here, to run repeatedly:
  delay(100);
}

void getTime() {
  int tz           = -5;
  int dst          = 0;
  time_t now       = time(nullptr);
  unsigned timeout = 5000; // try for timeout
  unsigned start   = millis();
  configTime(tz * 3600, dst * 3600, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  while (now < 8 * 3600 * 2 ) { // what is this ?
    delay(100);
    Serial.print(".");
    now = time(nullptr);
    if((millis() - start) > timeout){
      Serial.println("\n[ERROR] Failed to get NTP time.");
      return;
    }
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
  
}

void debugchipid(){
  // WiFi.mode(WIFI_STA);
  // WiFi.printDiag(Serial);
  // Serial.println(modes[WiFi.getMode()]);
  
  // ESP.eraseConfig();
  // wm.resetSettings();
  // wm.erase(true);
  WiFi.mode(WIFI_AP);
  // WiFi.softAP();
  WiFi.enableAP(true);
  delay(500);
  // esp_wifi_start();
  delay(1000);
  WiFi.printDiag(Serial);
  delay(60000);
  ESP.restart();

  // AP esp_267751
  // 507726A4AE30
  // ESP32 Chip ID = 507726A4AE30
}
void everyNumberOfDays(int payNumber, FirebaseJsonArray array){
  
}
void everyDay(int payNumber, FirebaseJsonArray array){
    
}
void checkIrrigateNow(int payNumber, FirebaseJson data){
  int irrNowH,irrNowM;

  if (Firebase.RTDB.getInt(&fbdo, "/irrigateNow"+payNumber+"/endHour")) {

  irrNowH  = fbdo.intData();
  if (irrNow1H != 99){
  payNumber == 1 ? digitalWrite(RelayPin1, LOW) : digitalWrite(RelayPin1, HIGH);
  payNumber == 1 ? digitalWrite(RelayPin2, HIGH) : digitalWrite(RelayPin2, Low);
  analogWrite(ENA1, 255);
  analogWrite(ENA2, 255);
                          
  if (Firebase.RTDB.getInt(&fbdo, "/irrigateNow"+payNumber+"/endMinute")) {
    irrNowM  = fbdo.intData();
    if(h==irrNowH){
        if(m==irrNowM){
            Firebase.RTDB.setInt(&fbdo, "/irrigateNow"+payNumber+"/endHour",99);
            digitalWrite(RelayPin1, HIGH);//pay1 
            digitalWrite(RelayPin2, HIGH);
            analogWrite(ENA1, 0);
            analogWrite(ENA2, 0);
         }
     }
  }


   }
}
 
}
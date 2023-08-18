//master-------------------
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <iostream>
#include <cstring>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String Update_signal;
#define API_KEY_BACKUP "pxHquPLdl2Eq0nYDZJUZ9R7wMk0APJyRmAC1qKfx"
#define DATABASE_URL_BACKUP "https://minda-7b7cc-default-rtdb.firebaseio.com/"
#define API_KEY "8fddd102db5d49a8f218cd9e613ea6c115cc5b6f"
#define DATABASE_URL "https://minda-4c2d2-default-rtdb.firebaseio.com/"
#define URL_fw_Bin "https://raw.githubusercontent.com/Tuananh98kthd/Minda_project/master/mindaUpload.ino.esp32.bin"
#define S1 21
#define S2 22
#define S3 23
#define led0 16
#define led1 17
#define led2 18
#define led3 19
#define ledAlarm 13
#define EEPROM_SIZE 128 
LiquidCrystal lcd(14, 27, 32, 33, 25, 26);
String ssid;
String pss; 
String Line[36]={"Line0","Line01","Line02","Line03","Line04","Line05","Line06","Line07","Line08","Line09","Line10","Line11","Line12","Line13","Line14","Line15","Line16","Line17","Line18","Line19","Line20","Line21","Line22","Line23","Line24","Line25","Line26","Line27","Line28","Line29","Line30","Line31","Line32","Line33","Line34","Line35"};
String LineSTT_Value[36];
String LineSTT;
String Station;
int led[4]={led0,led1,led2,led3};
int numSTT[36];
int count_call;
int count,count_clear;
String _data;
FirebaseJson json;
void cfgFirebase2();
void cfgFirebase1();
void firmwareUpdate();
void smartconfig();
void Alarm(int a);
void SetWifi();
int findsubstr(String mother, String sub);
void Firebase_clear();

void setup() {
lcd.begin(20, 4);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Connecting to Wifi");
for(int i=0;i<4;i++)pinMode(led[i],OUTPUT);
pinMode(S1,INPUT_PULLUP);
pinMode(S2,INPUT_PULLUP);
pinMode(S3,INPUT_PULLUP);
Serial.begin(115200);
if (!EEPROM.begin(EEPROM_SIZE))
{
  Serial.println("failed to init EEPROM");
  delay(1000);
}
else
{
  ssid = EEPROM.readString(0);
  Serial.print("SSID = ");
  Serial.println(ssid);
  pss = EEPROM.readString(20);
  Serial.print("PASS = ");
  Serial.println(pss);
}
if((digitalRead(S1)==LOW)&&(digitalRead(S2)==LOW)&&(digitalRead(S2)==LOW)){
  smartconfig();
  ssid = EEPROM.readString(0);
  Serial.print("SSID = ");
  Serial.println(ssid);
  pss = EEPROM.readString(20);
  Serial.print("PASS = ");
  Serial.println(pss);
}
WiFi.begin(ssid.c_str(), pss.c_str());
while(WiFi.status() != WL_CONNECTED) 
{
    digitalWrite(ledAlarm,HIGH);
    digitalWrite(led0,HIGH);
    delay(200);
    digitalWrite(ledAlarm,LOW);
    digitalWrite(led0,LOW);
    delay(200);
    SetWifi();
}
Serial.println("Connect Success");

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Wifi is Connected");
lcd.setCursor(0,1);
lcd.print(WiFi.localIP());
Serial.println("Success");
cfgFirebase1();
delay(1000);
Serial.println("Read Firebase 1");
Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "Update", &Update_signal ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
if(Update_signal=="1234"){
  Serial.println("update firmware V2.0");
  firmwareUpdate();
}
Serial.println("Start Primary Firebase");
if(Update_signal!="8888")cfgFirebase2();
WiFi.setAutoReconnect(true);
Firebase_clear();
}
void loop() 
{
  String Message="Line: ";
  if(digitalRead(S1)==LOW)Station="Station 1";
  if(digitalRead(S2)==LOW)Station="Station 2";
  if(digitalRead(S3)==LOW)Station="Station 3";
  if((digitalRead(S3)==HIGH)&&(digitalRead(S2)==HIGH)&&(digitalRead(S3)==HIGH))Station="Station 1";
  if(Firebase.ready())
  {
  Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, Station, &LineSTT ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  } else {
      Serial.println("Khong vao dc Firebase");
  }
  for(int i=1;i<36;i++)
  {
    LineSTT_Value[i] = LineSTT.substring(findsubstr(LineSTT,Line[i])+Line[i].length()+3,findsubstr(LineSTT,Line[i])+Line[i].length()+4 );
    Serial.print(LineSTT_Value[i]);Serial.print("-");
  }
  Serial.println();
  for(int i=1;i<36;i++)
  {
    numSTT[i]=0;
    if(LineSTT_Value[i]=="1")
    {
      Message+= String(i);
      Message+= " ";
      count_call++;
      numSTT[i]=1;
    }
  }
  lcd.clear();
  if(WiFi.isConnected()==true)
  {
   lcd.setCursor(0,0);
   lcd.print("Wifi is Connected");
   lcd.setCursor(0,1);
   lcd.print(WiFi.localIP());
  } 
  else
  {
   lcd.setCursor(0,0);
   lcd.print("Wifi is Disconnected");
  }
  if(Message.length()<=20) {
    lcd.setCursor(0,2);
    lcd.print(Message);
  }
  else{
    lcd.setCursor(0,2);
    lcd.print(Message.substring(0,19));
    lcd.setCursor(0,3);
    lcd.print(Message.substring(20,(int)Message.length()-1));
  }
    
  if(count_call>0)
    digitalWrite(led1,LOW);
  else
    digitalWrite(led1,HIGH);
  count_call=0;
  Serial.println(Message);
  count_clear++;
  delay(2000);
}


int findsubstr(String mother, String sub)
{
  if(mother.length()<=sub.length())return -1;
  for(int i=0;i<(mother.length()-sub.length()+1);i++)
  {
    int count=0;
    if(sub[0]==mother[i]){
      for(int j=0;j<sub.length();j++){
        if(sub[j]==mother[i+j])count++;
      }
    }
    if(count==sub.length())return i;
  }
    return -1;
}

void SetWifi()
{
  if(Serial.available()){
      while(Serial.available()){
        _data+=(char)Serial.read();
        count++;
      } 
      if(count>=100){
        _data=" ";
        count=0;
        }
  }
  if((findsubstr(_data,"SSID:")!=-1)&&(findsubstr(_data,"-PASSWORD:")!=-1)&&(findsubstr(_data,"-END")!=-1))
  {
    ssid=_data.substring(findsubstr(_data,"SSID:")+5,findsubstr(_data,"-PASSWORD:"));
    pss=_data.substring(findsubstr(_data,"-PASSWORD:")+10,findsubstr(_data,"-END"));
    Serial.println(ssid);
    Serial.println(pss);
    EEPROM.writeString(0,ssid);
    EEPROM.commit();
    delay(100);
    EEPROM.writeString(20,pss);
    EEPROM.commit();
    _data=" ";
    Serial.println("Write Successfuly");
  }
}

void set_new()
{
  for(int i=1;i<36;i++){
    if(Firebase.ready()){
      for(int i=1;i<36;i++){
        Firebase.RTDB.setString(&fbdo,"Station 3/"+Line[i],0);
      }
    }
  }
  while(1);
  }

void Alarm(int a)
{
  int b=a*2;
  for(int i=0;i<=b;i++)
  {
    if(i<=a)
    {
      digitalWrite(led0,LOW);
      digitalWrite(ledAlarm,LOW);
      delay(400);
      digitalWrite(led0,HIGH);
      digitalWrite(ledAlarm,HIGH);
      delay(400);
    }
    else 
    {
      digitalWrite(led0,HIGH);
      digitalWrite(ledAlarm,HIGH);
      delay(100);
    }
  }
}
void smartconfig()
{
  Serial.println("Start Smart Config");
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) 
  {
    Serial.print(".");
    delay(1000);
    Alarm(2);
  }
  Serial.println("");
  Serial.println("SmartConfig received.");  
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Alarm(4);
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  ssid = WiFi.SSID();
  pss = WiFi.psk();
  EEPROM.writeString(0,ssid);
  EEPROM.commit();
  delay(100);
  EEPROM.writeString(20,pss);
  EEPROM.commit();
  delay(100);
  Serial.println("WiFi Connected");
}
void Firebase_clear()
{
  if(digitalRead(S1)==LOW)Station="Station 1/";
  if(digitalRead(S2)==LOW)Station="Station 2/";
  if(digitalRead(S3)==LOW)Station="Station 3/";
  if((digitalRead(S3)==HIGH)&&(digitalRead(S2)==HIGH)&&(digitalRead(S3)==HIGH))Station="Station 1/";
  Serial.println("clear");

  if(Firebase.ready())
  for(int numLine=1; numLine<36; numLine++)
  {
    if(numSTT[numLine]==1)
    {
      Firebase.RTDB.setString(&fbdo, Station+Line[numLine], "0")? "ok" : fbdo.errorReason().c_str(); 
      Serial.println(Station+Line[numLine]);  
    }
    
    //Serial.println(fbdo.errorReason().c_str()) ;
      /* code */
  }
}
void cfgFirebase1(){
config.api_key = API_KEY_BACKUP;
config.token_status_callback = tokenStatusCallback;
config.database_url = DATABASE_URL_BACKUP;
config.signer.tokens.legacy_token = "YzU3RDxkvdwXvTRVtuIFlYdu7hFQV3onD8VCtS0l";
fbdo.setResponseSize(2048);   
Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
Firebase.setDoubleDigits(5);
config.timeout.serverResponse = 10 * 1000; 
}
void cfgFirebase2(){
config.api_key = API_KEY;
config.token_status_callback = tokenStatusCallback;
config.database_url = DATABASE_URL;
config.signer.tokens.legacy_token = "YzU3RDxkvdwXvTRVtuIFlYdu7hFQV3onD8VCtS0l";
fbdo.setResponseSize(2048);   
Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
Firebase.setDoubleDigits(5);
config.timeout.serverResponse = 10 * 1000; 
}
void firmwareUpdate() {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);
  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}

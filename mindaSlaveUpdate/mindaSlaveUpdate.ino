#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <iostream>
#include <cstring>
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
#define URL_fw_Bin "https://raw.githubusercontent.com/Tuananh98kthd/Minda_project/master/mindaSlaveUpdate.ino.esp32.bin"
#define sw7 34
#define sw6 35
#define sw5 32
#define sw4 33
#define sw3 14
#define sw2 27
#define sw1 26
#define sw0 25
#define led0 16
#define led1 17
#define led2 18
#define led3 19
#define ledAlarm 13
#define S1 21
#define S2 22
#define S3 23
#define LENGTH(x) (strlen(x) + 1)   // length of char string
#define EEPROM_SIZE 128 
String ssid;                        //string variable to store ssid
String pss; 
String Link1 = "Station 1/";
String Link2 = "Station 2/";
String Link3 = "Station 3/";
static String g_Stt[3];
String Line[36]={"Line0","Line01","Line02","Line03","Line04","Line05","Line06","Line07","Line08","Line09","Line10","Line11","Line12","Line13","Line14","Line15","Line16","Line17","Line18","Line19","Line20","Line21","Line22","Line23","Line24","Line25","Line26","Line27","Line28","Line29","Line30","Line31","Line32","Line33","Line34","Line35"};
int sw[8]={sw0,sw1,sw2,sw3,sw4,sw5,sw6,sw7};
int led[4]={led0,led1,led2,led3};
FirebaseJson json;
void cfgFirebase2();
void cfgFirebase1();
void firmwareUpdate();
void Alarm(int a);
void SetWifi();
void hienthiLed();
int findsubstr(String mother, String sub);
int Stt1_T=-1,Stt1_S,Stt2_T=-1,Stt2_S,Stt3_T=-1,Stt3_S,STT1,STT2,STT3;
String Stt1,Stt2,Stt3;
int W1,W2,W3;
int count;
byte numLine;
String _data;
void setup() {
for(int i=0;i<8;i++)pinMode(sw[i],INPUT_PULLUP);
for(int i=1;i<4;i++)pinMode(led[i],INPUT);
pinMode(ledAlarm,OUTPUT);
digitalWrite(ledAlarm,HIGH);
pinMode(S1,INPUT_PULLUP);
pinMode(S2,INPUT);
pinMode(S3,INPUT);
pinMode(4,INPUT_PULLUP);
pinMode(5,INPUT_PULLUP);
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
    Serial.print(".");
}
Serial.println("Connect Success");
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
for(int i=5;i>=0;i--)
{
  if(digitalRead(sw[i])==LOW)
  {
    numLine|=((1<<i));
  }
}
if((numLine<36)&&(numLine>0))
{
    Link1 += Line[numLine];
    Link2 += Line[numLine];
    Link3 += Line[numLine];
}
else 
{
    Alarm(5);
    Serial.print("Line khong hop le");
    Serial.println(numLine);
}
Stt1_T=-1;Stt2_T=-1;Stt3_T=-1;
}
void loop() 
{
if((digitalRead(S1)==HIGH)&&(digitalRead(S2)==HIGH)&&(digitalRead(S3)==HIGH))SetWifi();
hienthiLed();
if(digitalRead(S1)==LOW)Stt1_T=1; else Stt1_T=0;
if(digitalRead(S2)==LOW)Stt2_T=1; else Stt2_T=0;
if(digitalRead(S3)==LOW)Stt3_T=1; else Stt3_T=0;
STT1=Stt1_T-Stt1_S;
STT2=Stt2_T-Stt2_S;
STT3=Stt3_T-Stt3_S;
if(digitalRead(S1)==LOW)
{
delay(1000);
if(digitalRead(S1)==LOW)Stt1_S=1; 
}else Stt1_S=0;
if(digitalRead(S2)==LOW)
{
delay(1000);
if(digitalRead(S2)==LOW)Stt2_S=1;
} else Stt2_S=0;

if(digitalRead(S3)==LOW){
 delay(1000);
 if(digitalRead(S3)==LOW) Stt3_S=1;
 } else Stt3_S=0;
 
if(STT1==(1)||(W1==1)){
  W1=1;
  Serial.printf("Set string1... %s\n", Firebase.RTDB.setString(&fbdo, Link1, "1") ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  delay(1000);
  Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, Link1, &Stt1 ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  if(Stt1=="1")W1=0;
  delay(1000);
} 

if(STT1==(-1)||(W1==-1)){
  W1=-1;
  Serial.printf("Set string1... %s\n", Firebase.RTDB.setString(&fbdo, Link1, "0") ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  delay(1000);
  Serial.printf("Get string1... %s\n", Firebase.RTDB.getString(&fbdo, Link1, &Stt1 ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  if(Stt1=="0")W1=0;
  delay(1000);
}
if(STT2==(1)||(W2==1)){
  W2=1;
  Serial.printf("Set string2... %s\n", Firebase.RTDB.setString(&fbdo, Link2, "1") ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  delay(1000);
  Serial.printf("Get string2... %s\n", Firebase.RTDB.getString(&fbdo, Link2, &Stt2 ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  if(Stt2=="1")W2=0;
  delay(1000);
} 
if(STT2==(-1)||(W2==-1)){
  W2=-1;
  Serial.printf("Set string2... %s\n", Firebase.RTDB.setString(&fbdo, Link2, "0") ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  delay(1000);
  Serial.printf("Get string2... %s\n", Firebase.RTDB.getString(&fbdo, Link2, &Stt2 ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  if(Stt2=="0")W2=0;
  delay(1000);
}
if((STT3==1)||(W3==1)){
  W3=1;
  Serial.printf("Set string3... %s\n", Firebase.RTDB.setString(&fbdo, Link3, "1") ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  delay(1000);
  Serial.printf("Get string3... %s\n", Firebase.RTDB.getString(&fbdo, Link3, &Stt3 ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  if(Stt3=="1")W3=0;
  delay(1000);
} 
if(STT3==(-1)||(W3==-1)){
  W3=-1;
  Serial.printf("Set string3... %s\n", Firebase.RTDB.setString(&fbdo, Link3, "0") ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  delay(1000);
  Serial.printf("Get string3... %s\n", Firebase.RTDB.getString(&fbdo, Link3, &Stt3 ) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  if(Stt3=="0")W3=0;
  delay(1000);
}

if(!WiFi.isConnected())
{
  Alarm(7);
  Serial.println("Mat ket noi");
}

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

void Alarm(int a){
  int b=a*2;
  for(int i=0;i<=b;i++)
  {
    if(i<=a)
    {
      digitalWrite(led0,LOW);
      digitalWrite(ledAlarm,LOW);
      delay(400);
      digitalWrite(led0,HIGH);
      delay(400);
    }
    else 
    {
      digitalWrite(led0,HIGH);
      digitalWrite(ledAlarm,HIGH);
      delay(400);
    }

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
void hienthiLed(){
    if((digitalRead(S1)==LOW)||(digitalRead(S2)==LOW)||(digitalRead(S3)==LOW))
  {
    digitalWrite(led0,LOW);
    digitalWrite(ledAlarm,LOW);
  }
  else
  {
    digitalWrite(led0,HIGH);
    digitalWrite(ledAlarm,HIGH);   
  }
}

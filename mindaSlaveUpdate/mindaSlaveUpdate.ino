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
#define URL_fw_Bin "https://raw.githubusercontent.com/Tuananh98kthd/Minda_project/master/mindaUpload.ino.esp32.bin"
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
String Stt_S1,Stt_S2,Stt_S3;
static String g_Stt[3];
int stt1,stt2,stt3;
int on1,on2,on3,off1,off2,off3;
static int g_On[3] = {0};
static int g_Off[3] = {0};
int sw[8]={sw0,sw1,sw2,sw3,sw4,sw5,sw6,sw7};
int led[4]={led0,led1,led2,led3};
FirebaseJson json;
void cfgFirebase2();
void cfgFirebase1();
void firmwareUpdate();
void Alarm(int a);
void SetWifi();
void kt1();
void kt2();
void kt3();
int findsubstr(String mother, String sub);
int count;
byte numLine;
String _data;
void setup() {
for(int i=0;i<8;i++)pinMode(sw[i],INPUT_PULLUP);
for(int i=0;i<4;i++)pinMode(led[i],OUTPUT);
pinMode(ledAlarm,OUTPUT);
digitalWrite(ledAlarm,HIGH);
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
}

void loop() 
{
if((digitalRead(S1)==HIGH)&&(digitalRead(S2)==HIGH)&&(digitalRead(S3)==HIGH))SetWifi();
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

  for(int i=5;i>=0;i--){
    if(digitalRead(sw[i])==LOW){
    numLine|=((1<<i));
    }
  }
  if((numLine<36)&&(numLine>0))
  {
  kt1();
  kt2();
  kt3();
  }
  else 
  {
    Alarm(5);
    Serial.print("Line khong hop le");
    Serial.println(numLine);
  }
  if(!WiFi.isConnected()){
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
void kt1()
{
  if (digitalRead(S1) == LOW)off1 = 0;
  if (digitalRead(S1) == HIGH)on1 = 0;
  if ((digitalRead(S1) == LOW) && (on1 == 0))
  {
    if (Firebase.ready())
    {
    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "Station1", &Stt_S1) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    }
    else 
    {
      Serial.println("Khong vao dc Firebase");
    }
    if (Stt_S1 != NULL)
    {
      if (Stt_S1[numLine] == '1')
      {
        on1 = 1;
      }
      else
      {
        Stt_S1[numLine] = '1';
        Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, "Station1", Stt_S1) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
      }
    }
  }

  if ((digitalRead(S1) == HIGH) && (off1 == 0))
  {
    if (Firebase.ready())
    {
    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "Station1", &Stt_S1) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.println(Stt_S1);
    }
    else 
    {
      Serial.println("Khong vao dc Firebase");
    }
    if (Stt_S1 != NULL)
    {
      if (Stt_S1[numLine] == '0')
      {
        off1 = 1;
      }
      else
      {
        Stt_S1[numLine] = '0';
        Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, "Station1", Stt_S1) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
        Serial.println(Stt_S1);
      }
    }
  }
}
void kt2()
{
  if (digitalRead(S2) == LOW)off2 = 0;
  if (digitalRead(S2) == HIGH)on2 = 0;
  if ((digitalRead(S2) == LOW) && (on2 == 0))
  {
    if (Firebase.ready())
    {
    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "Station2", &Stt_S2) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    }
    else 
    {
      Serial.println("Khong vao dc Firebase");
    }
    if (Stt_S2 != NULL)
    {
      if (Stt_S2[numLine] == '1')
      {
        on2 = 1;
      }
      else
      {
        Stt_S2[numLine] = '1';
        Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, "Station2", Stt_S2) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
      }
    }
  }

  if ((digitalRead(S2) == HIGH) && (off2 == 0))
  {
    if (Firebase.ready())
    {
    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "Station2", &Stt_S2) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.println(Stt_S2);
    }
    else 
    {
      Serial.println("Khong vao dc Firebase");
    }
    if (Stt_S2 != NULL)
    {
      if (Stt_S2[numLine] == '0')
      {
        off2 = 1;
      }
      else
      {
        Stt_S2[numLine] = '0';
        Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, "Station2", Stt_S2) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
        Serial.println(Stt_S2);
      }
    }
  }
}

void kt3()
{
  if (digitalRead(S3) == LOW)off3 = 0;
  if (digitalRead(S3) == HIGH)on3 = 0;
  if ((digitalRead(S3) == LOW) && (on3 == 0))
  {
    if (Firebase.ready())
    {
    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "Station3", &Stt_S3) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    }
    else 
    {
      Serial.println("Khong vao dc Firebase");
    }
    if (Stt_S3 != NULL)
    {
      if (Stt_S3[numLine] == '1')
      {
        on3 = 1;
      }
      else
      {
        Stt_S3[numLine] = '1';
        Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, "Station3", Stt_S3) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
      }
    }
  }

  if ((digitalRead(S3) == HIGH) && (off3 == 0))
  {
    if (Firebase.ready())
    {
    Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, "Station3", &Stt_S3) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    Serial.println(Stt_S3);
    }
    else 
    {
      Serial.println("Khong vao dc Firebase");
    }
    if (Stt_S3 != NULL)
    {
      if (Stt_S3[numLine] == '0')
      {
        off3 = 1;
      }
      else
      {
        Stt_S3[numLine] = '0';
        Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, "Station3", Stt_S3) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
        Serial.println(Stt_S3);
      }
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

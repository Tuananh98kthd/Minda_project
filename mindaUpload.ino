#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"

const char * ssid = "AnhHuong";
const char * password = "00000000";

#define URL_fw_Bin "https://raw.githubusercontent.com/Tuananh98kthd/mindaUpdate/main/Minda.bin"


void setup() {

  Serial.begin(115200);
  connect_wifi();
  pinMode(21,INPUT_PULLUP);
  Serial.println("V2.102....-=");
}
void loop() {
  while(digitalRead(21)==HIGH){
    Serial.println("V2.102");
  }
  if (digitalRead(21)==LOW) 
  { 
    Serial.println("Firmware update Starting..");
    firmwareUpdate();
  }
  while(digitalRead(21)==LOW);
}

void connect_wifi() {
  Serial.println("Waiting for WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void firmwareUpdate(void) {
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

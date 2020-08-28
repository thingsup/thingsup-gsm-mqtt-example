/*
  This code demonstrates the functionality of connecting to Thingsup cloud MQTT broker via GSM modem.

  It connects to an Thingusup MQTT broker then:
  - publishes "hello world" to the every 10 seconds
  - subscribes to the topic and printing out any messages

  The code is tested with SIM800L and should also work with following modems
  SIMCom SIM800 series (SIM800A, SIM800C, SIM800L, SIM800H, SIM808, SIM868)
  SIMCom SIM900 series (SIM900A, SIM900D, SIM908, SIM968)
  SIMCom WCDMA/HSPA/HSPA+ Modules (SIM5360, SIM5320, SIM5300E, SIM5300E/A)
  SIMCom LTE Modules (SIM7100E, SIM7500E, SIM7500A, SIM7600C, SIM7600E)
  SIMCom SIM7000E/A/G CAT-M1/NB-IoT Module
  Ai-Thinker A6, A6C, A7, A20
  ESP8266 (AT commands interface, similar to GSM modems)
  Digi XBee WiFi and Cellular (using XBee command mode)
  Neoway M590
  u-blox 2G, 3G, 4G, and LTE Cat1 Cellular Modems (many modules including LEON-G100, LISA-U2xx, SARA-G3xx, SARA-U2xx, TOBY-L2xx, LARA-R2xx, MPCI-L2xx)
  u-blox LTE-M/NB-IoT Modems (SARA-R4xx, SARA-N4xx, but NOT SARA-N2xx)
  Sequans Monarch LTE Cat M1/NB1 (VZM20Q)
  Quectel BG96
  Quectel M95
  Quectel MC60 (alpha)

  Tested on BLYNK_V1.3 board.

*/


#include "stdio.h"
#include "stdlib.h"
#include "Arduino.h"

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <WiFi.h>

bool mqtt_connection_flag = false;

boolean willRetain = 0;
const char* willMessage = "0";
uint8_t willQos = 1;

/**********************Sim800 variable & function**********************/
#define TINY_GSM_MODEM_SIM800                                   // Define the modem used, in this case its SIM800. refer TinyGSM docs for more details.
#define TINY_GSM_RX_BUFFER     1024
#define SerialAT               Serial1

#include <TinyGsmClient.h>
#include <PubSubClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

#define Sim800_RST          5                                  // ESP32 PIn connected to reset of GSM modem
#define Sim800_PWKEY        4                                  // ESP32 Pin connected to Power Key pin of GSM
#define Sim800_POWER_ON     23                                 // ESP32 pin connected to hardware controlling GSM's power supply - Optional
#define Sim800_TX           27                                 // ESP32 pin connected to TX of GSM modem
#define Sim800_RX           26                                 // ESP32 pin connected to RX of GSM modem

TinyGsmClientSecure GSMclient(modem);                          // TinyGSM Client for Internet connection
PubSubClient MQTTclient;                                       // MQTT client

#define GSM_LED 19

const char apn[]      = "internet";                      //APN of simcard = internet for 'Idea' Users
const char gprsUser[] = "";
const char gprsPass[] = "";

const char* mqtt_server = "mqtt.thingsup.io";                // MQTT broker URL for thingsup
const char* mqtt_password = "ESP32";                         // Device Password set in the device addition stage in thingsup
const char* mqtt_username = "3p309x1kcec5i7g:ESP32";         // Device Key set in the device addition stage in thingsup
const int  mqtt_port = 1883;                                 // MQTT port for SSL connection in thingsup
const char* mqtt_clientID = "GATEWAY_1";                     // Client ID set in the device addition stage in thingsup
const char* accountID = "3p309x1kcec5i7g";                   // Thingsup Account ID Automatically generated when account created with thingsup

String publish_topic = "";
String subscribe_topic = "";

/* Description: This function converts initialize the GSM modem
 * Params: byte array
 * return: String
*/

void setup_gprs_modem()
{
  Serial.println(__func__);
  //
  MQTTclient.setClient(GSMclient);
  pinMode(Sim800_PWKEY, OUTPUT);
  pinMode(Sim800_RST, OUTPUT);
  pinMode(Sim800_POWER_ON, OUTPUT);

  digitalWrite(Sim800_PWKEY, LOW);
  digitalWrite(Sim800_RST, HIGH);
  digitalWrite(Sim800_POWER_ON, HIGH);

  SerialAT.begin(115200, SERIAL_8N1, Sim800_RX, Sim800_TX);
  delay(500);

  Serial.print("Initializing Sim800...\n");
  Serial.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

  Serial.print("sim_apn=");
  Serial.println(apn);
  modem.gprsConnect(apn, gprsUser, gprsPass);
  Serial.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println("waitForNetwork fail");

  }
  Serial.println(" success");

  if (modem.isNetworkConnected()) {
    Serial.println("Network connected");
  }
  Serial.println("Connecting to ");
  Serial.println(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    Serial.println(" fail");

  }
  Serial.println(" success");
  if (modem.isGprsConnected()) {
    Serial.println("GPRS connected");
  }
  String time_network = modem.getGSMDateTime(DATE_FULL);
  Serial.print("time_network;");
  Serial.println(time_network);

}

/* Description: This function converts char array to arduino string object
 * Params: byte array
 * return: String
*/

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
  Serial.println(__func__);
  String payloadS;
  if (strcmp(topic , (char*)(subscribe_topic.c_str())) == 0)
  {
    for (int i = 0; i < length; i++)
    {
      payloadS += (char)payload[i] ;
    }

    Serial.println(payloadS);
  }
}

/* Description: This function converts char array to arduino string object
 * Params: byte array
 * return: String
*/
String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]);
    s += buf;
  }
  return s;
}

/* Description: This function gives Mac address of esp32
 * Params: Null
 * return: Mac address of esp32 in String format
*/
String composeMAC() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String clientId;
  clientId += "";
  clientId += mac2String(mac);
  return clientId;
}

/* Description: This function takes care of reconnection to cloud
 * Params: Null
*/
void reconnect()
{
  if (!MQTTclient.connected())
  {
    if (!modem.isGprsConnected()) {
      pinMode(Sim800_RST, OUTPUT);
      digitalWrite(Sim800_RST, LOW);
      vTaskDelay(10);
      digitalWrite(Sim800_RST, HIGH);
      Serial.println("GPRS connection try");

      Serial.println("Waiting for network...");

      if (modem.isNetworkConnected()) {
        Serial.println("Network connected");
      }
      modem.gprsConnect(apn, gprsUser, gprsPass);

      if (!modem.gprsConnect(apn, gprsUser, gprsPass))
      {
        Serial.println(" fail");
      }
    }

    Serial.print("MQTT : ");

    Serial.print("clientId : ");
    Serial.println(mqtt_clientID);
    Serial.print("mqtt_user=");
    Serial.println(mqtt_username);
    Serial.print("mqtt_pass=");
    Serial.println(mqtt_password);

    if (MQTTclient.connect(mqtt_clientID, mqtt_username, mqtt_password))
    {
      Serial.print("MQTT :CONNECTED ");
      MQTTclient.subscribe((char*)(subscribe_topic.c_str()));
      if (MQTTclient.publish((char*)publish_topic.c_str(), "HEllo") == true)
        {
          Serial.println("Message Published");
        }
        else
        {
          Serial.println("Message Publish failed");
        }
    }
    else
    {
      Serial.print("MQTT :NOT CONNECTED ");
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_gprs_modem();

  publish_topic = "/" + String(accountID) + "/" + "telemetry/" + String(composeMAC()) + "/TEST";
  subscribe_topic = "/" + String(accountID) + "/" + "command/" + String(composeMAC()) + "/TEST";
  Serial.println(String(composeMAC()));
  MQTTclient.setServer(mqtt_server, mqtt_port);
  MQTTclient.setCallback(mqtt_callback);
}

long t1 = 0;

void loop() {


  if (MQTTclient.publish_connected)
  {
    mqtt_connection_flag = true;
    if (millis() - t1 > 10000)
    {
      t1=millis();
      if (MQTTclient.publish_connected)
      {
        Serial.print("Publishing on topic=");
        Serial.println(publish_topic);
        if (MQTTclient.publish((char*)publish_topic.c_str(), "HEllo") == true)
        {
          Serial.println("Message Published");
        }
        else
        {
          Serial.println("Message Publish failed");
        }
      }
    }
  }
  else
  {
    reconnect();
  }

  MQTTclient.loop();

  /* This code is for reseting watchdog Timer to avoid crashes due to watchdog timer overflow*/
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;

}

/* 
 *  PLEASE DONT DELETE AND EDIT 
 *  Develop by          : Nurman Hariyanto
 *  Email               : nurman.hariyanto13@gmail.com
 *  Project             : PKL-ONLINE Soil Moisture
 *  Version             : 1.0
 *  Description         : This code for get data from soil moisture (example) and send data soil to MQTT/RMQ
 *  Microcontroller     : Wemos Mini ESP8266
 *                        NodeMCU ESP8266
 *  Device              : Soil Moisture Sensor Module                   
 */




/*
 * YOU CAN EDIT NOW
 */

/*
 * Include library 
 */
#include "HX711.h"
 #include <PubSubClient.h>
 #include <ESP8266WiFi.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Connection parameter
 * -Wifi -> internet connection foor data communication
 * -RabbitMQ/MQTT -> protoocol data communication
 */
const char* wifiSsid              = "LSKKHomeAuto";
const char* wifiPassword          = "1234567890";
const char* mqttHost              = "rmq2.pptik.id";
const char* mqttUserName          = "/workshopmitra:workshopmitra";
const char* mqttPassword          = "passwordnyaqwerty";
//const char* mqttClient            = "IOT-Water-Pumpp";
const char* mqttQueueTimbangan       = "Timbangan";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*7
 * Device parameter
 * -Guid -> Id Device (unique) you can generate from here (https://www.uuidgenerator.net/version1) 
 * -status device -> save last state from the pump is on or off (1 = on , 0 = off) 
 * -pin microcontroller 
 * -mac device
 * 
 */
int deviceDout =  D5;
int deviceClk = D6;
HX711 scale(deviceDout, deviceClk);
float calibration_factor = 119525;

/*
 * Wifi setup WiFi client and mac address 
 */
WiFiClient espClient;
PubSubClient client(espClient);
byte mac[6]; //array temp mac address
String MACAddress;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Set up WiFi connection
 */
 void setup_wifi(){
  delay(10);
  //We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to :");
  Serial.println(wifiSsid);
  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Get Mac Address from microcontroller
 */
 
String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Print Mac Address 
 */
 void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * Function for Get message payload from MQTT rabbit mq
 */
void callback(char* topic, byte* payload, unsigned int length){
  char message[5]; //variable for temp payload message
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Messagge :");
  for(int i = 0;i < length;i++){
    Serial.print((char)payload[i]);
    message[i] = (char)payload[i]; //initiate value from payload to message variable
    
  }
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Reconnecting to MQTT/RabbitMQ 
 */
void reconnect() {
  // Loop until we're reconnected
  printMACAddress();
  const char* CL;
  CL = MACAddress.c_str();
  Serial.println(CL);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CL, mqttUserName, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);

    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Function for Setup Pin Mode,wifi,mqtt,and serial
 */
void setup()
{
  Serial.begin(115200);
  pinMode(deviceDout, INPUT);
  scale.set_scale(105000);
  scale.tare(); //Reset the scale to 0
 
  long zero_factor = scale.read_average(); 
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);
  setup_wifi();
  printMACAddress();
  client.setServer(mqttHost, 1883);
  client.setCallback(callback);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * This functioon for loop your program
 */
void loop() {
  //if you disconnected from wifi and mqtt
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
scale.set_scale(calibration_factor); 
float dataWeight;
 dataWeight = ((scale.get_units()));
 String convertDataWeight = String(dataWeight);
 char dataToMQTT[50];
 convertDataWeight.toCharArray(dataToMQTT, sizeof(dataToMQTT));
 client.publish(mqttQueueTimbangan,dataToMQTT);
  
  delay(10000);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

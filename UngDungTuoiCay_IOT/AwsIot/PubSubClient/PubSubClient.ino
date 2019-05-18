#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#error Platform not supported
#endif
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
 //thu vien tai tai day https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h> 
#include <time.h>
#define emptyString String()

#define motor D8
int   nhietDo=25;
int thoiGianDung=3;//////////////////////
int thoiGianKhongKichHoat=15;
int  ErrActiveTime=0;
int maKichHoat=1111;
int maKhongKichHoat=0000;

float TmpProcessing(){
  long tmp2=0,value;
  for(int i=0;i<30;i++)
      {
        tmp2+=analogRead(A0);
      }
  value=tmp2/30;
  float per=map(value,72,450,0,100);
  per=100-per;
  return per;
}

int  Motor(int R){
     if(R==0)
     {
         digitalWrite(motor, LOW);
         return 0;
      }
      else if(R==1){
         digitalWrite(motor, HIGH);
         return 1;      
        }
     return 0;
 }
 
String rMotor(float tmp)
{
  if(tmp<nhietDo && ErrActiveTime<(thoiGianDung+1)){
    Motor(1);
    ErrActiveTime+=1; 
    return "Đang bơm nước !";
    }
  else if (ErrActiveTime<(thoiGianDung+1)) {
    Motor(0); 
    ErrActiveTime=0;
    return "Không  bơm nước !";
    }

   if (ErrActiveTime>thoiGianDung)
        {
          if(ErrActiveTime>thoiGianKhongKichHoat)
            {  
                Motor(1);  
                return "Bơm nước(Nguoi Dung)!";        
            }
            else  if(ErrActiveTime==thoiGianKhongKichHoat)
                 {
                     Motor(0);
                     ErrActiveTime=0;
                     return "Dừng  bơm nước !";     
                  } 
            else{
                  Motor(0);
                  return "Lỗi nguồn nước!";
              }
        }  
  }

//nhap khoa secrets.h ▼
#include "secrets.h"

#if !(ARDUINOJSON_VERSION_MAJOR == 6 and ARDUINOJSON_VERSION_MINOR >= 7)
#error "Install ArduinoJson v6.7.0-beta or higher"
#endif

const int MQTT_PORT = 8883;
const char MQTT_SUB_TOPIC[] = "$aws/things/DoAnIotThing/shadow/update";
const char MQTT_PUB_TOPIC[] = "$aws/things/DoAnIotThing/shadow/update";

#ifdef USE_SUMMER_TIME_DST
uint8_t DST = 1;
#else
uint8_t DST = 0;
#endif

WiFiClientSecure net;

#ifdef ESP8266
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
#endif

PubSubClient client(net);

unsigned long lastMillis = 0;
time_t now;
time_t nowish = 1510592825;

void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  int A[20];
  int S=0,n=0;
  for (int i=length -4;i>=0;i--)
    {
      if((char)payload[i]>='0' &&(char)payload[i]<= '9')
        {
          A[n]=(char)payload[i];
          n++;
        }      
    }
    for (int i=n-1;i>=0;i--)        
        S+=((int)A[i]-48)*pow(10,i);
    
      if(S==maKichHoat){
          ErrActiveTime=thoiGianKhongKichHoat+1;
       }
      if(S==maKhongKichHoat)
      {
         ErrActiveTime=thoiGianKhongKichHoat;
        }
}

void pubSubErr(int8_t MQTTErr)
{
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
    Serial.print("Connection tiemout");
  else if (MQTTErr == MQTT_CONNECTION_LOST)
    Serial.print("Connection lost");
  else if (MQTTErr == MQTT_CONNECT_FAILED)
    Serial.print("Connect failed");
  else if (MQTTErr == MQTT_DISCONNECTED)
    Serial.print("Disconnected");
  else if (MQTTErr == MQTT_CONNECTED)
    Serial.print("Connected");
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
    Serial.print("Connect bad protocol");
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
    Serial.print("Connect bad Client-ID");
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
    Serial.print("Connect unavailable");
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
    Serial.print("Connect bad credentials");
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
    Serial.print("Connect unauthorized");
}

void connectToMqtt(bool nonBlocking = false)
{
  Serial.print("MQTT connecting ");
  while (!client.connected())
  {
    if (client.connect(THINGNAME))
    {
      Serial.println("connected!");
      if (!client.subscribe(MQTT_SUB_TOPIC))
        pubSubErr(client.state());
    }
    else
    {
      Serial.print("failed, reason -> ");
      pubSubErr(client.state());
      if (!nonBlocking)
      {
        Serial.println(" < try again in 5 seconds");
        delay(5000);
      }
      else
      {
        Serial.println(" <");
      }
    }
    if (nonBlocking)
      break;
  }
}

void connectToWiFi(String init_str)
{
  if (init_str != emptyString)
    Serial.print(init_str);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  if (init_str != emptyString)
    Serial.println("ok!");
}

void checkWiFiThenMQTT(void)
{
  connectToWiFi("Checking WiFi");
  connectToMqtt();
}

unsigned long previousMillis = 0;
const long interval = 5000;

void checkWiFiThenMQTTNonBlocking(void)
{
  connectToWiFi(emptyString);
  if (millis() - previousMillis >= interval && !client.connected()) {
    previousMillis = millis();
    connectToMqtt(true);
  }
}

void checkWiFiThenReboot(void)
{
  connectToWiFi("Checking WiFi");
  Serial.print("Rebooting");
  ESP.restart();
}

void sendData(void)
{
  DynamicJsonDocument jsonBuffer(JSON_OBJECT_SIZE(3) + 100);
  JsonObject root = jsonBuffer.to<JsonObject>();
  JsonObject state = root.createNestedObject("state");
  JsonObject state_reported = state.createNestedObject("reported");
  //nhiet do
  float tmp =TmpProcessing();
  state_reported["temperature"] = tmp;

  String status=rMotor(TmpProcessing());
  state_reported["status"]=status;
  
  Serial.printf("Sending  [%s]: ", MQTT_PUB_TOPIC);
  Serial.println();
  
  Serial.println(status);
  
  serializeJson(root, Serial);
  Serial.println();
  char shadow[measureJson(root) + 1];
  serializeJson(root, shadow, sizeof(shadow));
  if (!client.publish(MQTT_PUB_TOPIC, shadow, false))
    pubSubErr(client.state());
}

void setup()
{
  //thiet dat motor
  Serial.begin(9600);
  pinMode(5,INPUT_PULLUP);
  pinMode(motor,OUTPUT);
  digitalWrite(motor, LOW);
  //
  delay(2000);
  Serial.println();
  Serial.println();
#ifdef ESP32
  WiFi.setHostname(THINGNAME);
#else
  WiFi.hostname(THINGNAME);
#endif
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  connectToWiFi(String("Attempting to connect to SSID: ") + String(ssid));

  NTPConnect();

#ifdef ESP32
  net.setCACert(cacert);
  net.setCertificate(client_cert);
  net.setPrivateKey(privkey);
#else
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
#endif

  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(messageReceived);

  connectToMqtt();
}

void loop()
{
  now = time(nullptr);
  if (!client.connected())
  {
    checkWiFiThenMQTT();
    //checkWiFiThenMQTTNonBlocking();
    //checkWiFiThenReboot();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > 5000)
    {
      lastMillis = millis();
      sendData();
    }
  }
}

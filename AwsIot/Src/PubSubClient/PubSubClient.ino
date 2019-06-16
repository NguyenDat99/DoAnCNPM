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
#define led_Do D2
#define led_Vang D6

int nhietDo=25;
int thoiGianDung=3;//////////////////////
int thoiGianKhongKichHoat=15;
int ErrActiveTime=0;
int maKichHoat=1111;
int maKhongKichHoat=0000;
int thoiGianChoMoiTask=5000;


void led_Do_Sang()
{
  digitalWrite(led_Do, HIGH);
  }
  void led_Vang_Sang()
{
  digitalWrite(led_Do, HIGH);
  }
void led_Do_ChopTat()
{
  digitalWrite(led_Do, HIGH);
  delay(1000);
  digitalWrite(led_Do, LOW);
  delay(1000);  
}
void led_Vang_ChopTat()
{
  digitalWrite(led_Do, HIGH);
  delay(1000);
  digitalWrite(led_Do, LOW);
  delay(1000);  
}
void hai_led_bat()
  {
    digitalWrite(led_Do, HIGH);
    digitalWrite(led_Vang, HIGH);
    }
void hai_led_ChopTat(){
  digitalWrite(led_Do, HIGH);
  digitalWrite(led_Vang, HIGH);
  delay(1000);
  digitalWrite(led_Do, LOW);
  digitalWrite(led_Vang, LOW);
  delay(1000);    
  }
 void led_Do_tat(){
  digitalWrite(led_Do, LOW);
  }    
 void led_Vang_tat(){
  digitalWrite(led_Vang, LOW);
  
  }



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
    led_Do_Sang();
    led_Vang_tat();
    ErrActiveTime+=1; 
    return "Đang bơm nước !";
    }
  else if (ErrActiveTime<(thoiGianDung+1)) {
    Motor(0);
    led_Do_tat();
    led_Vang_tat(); 
    ErrActiveTime=0;
    return "Không  bơm nước !";
    }

   if (ErrActiveTime>thoiGianDung)
        {
          if(ErrActiveTime>thoiGianKhongKichHoat)
            {  
                Motor(1);
                led_Vang_Sang();
                led_Do_tat();  
                return "Bơm nước(Nguoi Dung)!";        
            }
            else  if(ErrActiveTime==thoiGianKhongKichHoat)
                 {
                     Motor(0);
                     ErrActiveTime=0;
                     led_Vang_tat();
                     led_Do_tat(); 
                     return "Dừng  bơm nước !";     
                  } 
            else{
                  Motor(0);
                  led_Do_ChopTat();
                  led_Vang_tat();
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
const char MQTT_SUB_TOPIC[] = "$aws/things/Pump/shadow/update";
const char MQTT_PUB_TOPIC[] = "$aws/things/Pump/shadow/update";

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
  int thoiGianTask[20],cheDoBom[20],NhietDo[20];
  int S=0,n=0,k1=0,k2=0;
  // thoi gian cho 1 tien trinh
   for (int i=length-1;i>=0;i--)        
   {
      if((char)payload[i]=='*'){k1=i;break;}
      if((char)payload[i]>='0' &&(char)payload[i]<= '9')
        {
          thoiGianTask[n]=(char)payload[i];
          n++;
        }  
    }
    for (int i=n-1;i>=0;i--)
    {        
        S+=((int)thoiGianTask[i]-48)*pow(10,i);
    }
    thoiGianChoMoiTask=S;
    S=0;
    n=0;
  // che do bom
  for (int i=k1-1;i>=0;i--)
    {
      if((char)payload[i]=='!'){k2=i-1;break;}
      if((char)payload[i]>='0' &&(char)payload[i]<= '9')
        {
          cheDoBom[n]=(char)payload[i];
          n++;
        }      
    }
    for (int i=n-1;i>=0;i--)        
        S+=((int)cheDoBom[i]-48)*pow(10,i);
    
      if(S==maKichHoat){
          ErrActiveTime=thoiGianKhongKichHoat+1;
       }
      if(S==maKhongKichHoat)
      {
         ErrActiveTime=thoiGianKhongKichHoat;
        }
        S=0;
        n=0;
    //nhietDo
    for (int i=k2;i>=0;i--)
    {
      if((char)payload[i]=='*')break;
      if((char)payload[i]>='0' &&(char)payload[i]<= '9')
        {
          NhietDo[n]=(char)payload[i];
          n++;
        }      
    }
    for (int i=n-1;i>=0;i--)        
        S+=((int)NhietDo[i]-48)*pow(10,i);
    nhietDo=S;
   Serial.println(" ");
   Serial.println(" ");
   Serial.println(S);
   Serial.println(thoiGianChoMoiTask);
   Serial.println(" ");
   Serial.println(" ");
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
    led_Vang_ChopTat();
    led_Do_tat();
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
    hai_led_ChopTat();
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
  pinMode(led_Do, OUTPUT);
  pinMode(led_Vang, OUTPUT);
  digitalWrite(motor, LOW);
  //
  Serial.println(" ");
  Serial.println(" ");
  Serial.println("Bat dau SmartConfig");
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  delay(500);
  WiFi.beginSmartConfig();
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("SmartConfig hoan thanh!");
  ssid=WiFi.SSID();
  pass=WiFi.psk();
  WiFi.disconnect();
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
  connectToWiFi(String("Bat  dau ket noi den SSID: ") + String(ssid));

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
    hai_led_bat();
    delay(1000);
  }
  else
  {
    led_Do_tat();
    led_Vang_tat();
    client.loop();
    if (millis() - lastMillis > thoiGianChoMoiTask)
    {
      lastMillis = millis();
      sendData();
    }
  }
}

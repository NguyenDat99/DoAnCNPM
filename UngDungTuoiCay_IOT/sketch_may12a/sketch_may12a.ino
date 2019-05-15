#include <ESP8266WiFi.h>
//#include <SocketIOClient.h>
//SocketIOClient client;
#define motor D8

const char* ssid ="sa";
const char* password ="12345678";

char host[] = "192.168.43.29";
int port = 3000;
extern String RID;
extern String Rfull;


void setupNetwork() {
  WiFi.begin(ssid, password);
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.print("Đang ket noi wifi:  ");
  while (WiFi.status() != WL_CONNECTED) 
    {    
        delay(500);
        Serial.print(".");
    }
  Serial.println("");
  Serial.print("da ket noi toi Wifi co ip: ");
  Serial.println(WiFi.localIP());
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
  if(tmp<25){  Motor(1); return "Đang bơm nước !";}
  else {Motor(0); return "Không  bơm nước !";}
}
 

void setup() {
  Serial.begin(9600);
  pinMode(5,INPUT_PULLUP);
  pinMode(motor,OUTPUT);
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  digitalWrite(motor, LOW);
 // setupNetwork();
}

void loop() {  
  Serial.println(rMotor(TmpProcessing()));
}

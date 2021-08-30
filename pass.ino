#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "FirebaseESP8266.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const char *ssid = "TP-Link_1C40";
const char *password = "18597691";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org");

String weekDays[7]={"Domingo", "lunes", "martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
String months[12]={"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

const char *FIREBASE_HOST = "https://capacity-compulago-default-rtdb.firebaseio.com";
const char *FIREBASE_AUTH = "FCNXRjgYV6rY6ihOOUqj9fG7gl9Q5VEltklMFo8s";


FirebaseData firebaseData;

LiquidCrystal_I2C lcd(0x27, 16, 2);

int contador = 5;

void setup() {
  Serial.begin(115200);
  pinMode(13, INPUT);
  pinMode(12, OUTPUT);
  pinMode(16, INPUT);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println("");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(250);
  }
  Serial.print("\nConectado al Wi-Fi");
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  timeClient.begin();
  timeClient.setTimeOffset(-18000);
  
  lcd.begin();

  lcd.clear();

  
  String nodo = "COMPULAGO";
  Firebase.getInt(firebaseData, nodo + "/Contador");
  contador = firebaseData.intData();
   Serial.println("inicial contador:  ");
  Serial.print(firebaseData.intData());
}

void loop() {

  
  String nodo = "COMPULAGO";

 digitalWrite(12, LOW);
      
  lcd.setCursor(0, 0);
  lcd.print("CUPOS DISPONIBLE");
  lcd.setCursor(0, 1);
  lcd.print(">>");
  lcd.setCursor(4, 1);
  lcd.print(contador);
  if(contador  == 0){

     lcd.setCursor(7, 1);
     lcd.print("(LLENO)");
     
    }
    
    if (digitalRead(13) == LOW) {

      if(contador > 0){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("    ENTRANDO");

        
        contador = contador - 1;
        ///////////////////////////////

          //guardamos el registro en firebase
            Serial.print("Enviando datos ");
            timeClient.update();

            unsigned long epochTime = timeClient.getEpochTime();
            String weekDay = weekDays[timeClient.getDay()];
            struct tm *ptm = gmtime ((time_t *)&epochTime); 
            int monthDay = ptm->tm_mday;
            int currentMonth = ptm->tm_mon+1;
            String currentMonthName = months[currentMonth-1];
            int currentYear = ptm->tm_year+1900;
            String currentDate = String(weekDay) + ", " + String(monthDay) + " de " + String(currentMonthName) + " del " + String(currentYear);
    
            
            Firebase.pushString(firebaseData, nodo + "/Sensor", "Entrada");
            delay(150);;            
            Firebase.pushString(firebaseData, nodo + "/Hora", timeClient.getFormattedTime());
            delay(150);
            Firebase.pushString(firebaseData, nodo + "/Fecha", currentDate);
            delay(150);
            Firebase.setInt(firebaseData, nodo + "/Contador", contador);
            delay(150);
            
            Firebase.end(firebaseData);
        
            Serial.println("Resetting ESP");
            ESP.restart(); //ESP.reset();
        //////////////////////////////
        }else{
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("CAPACIDAD MAXIMA");
          lcd.setCursor(0, 1);
          lcd.print("      X X");
          
          delay(3000);
          lcd.clear();
          
          }
      
      
    }
    if (digitalRead(16) == LOW) {
      
      if(contador >= 5){
        
      }else{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("    SALIENDO");
        digitalWrite(12, HIGH);

        contador = contador + 1;
        ///////////////////////////////
            //guardamos el registro en firebase
            Serial.print("Enviando datos ");
            timeClient.update();   

            unsigned long epochTime = timeClient.getEpochTime();
            String weekDay = weekDays[timeClient.getDay()];
            struct tm *ptm = gmtime ((time_t *)&epochTime); 
            int monthDay = ptm->tm_mday;
            int currentMonth = ptm->tm_mon+1;
            String currentMonthName = months[currentMonth-1];
            int currentYear = ptm->tm_year+1900;
            String currentDate = String(weekDay) + ", " + String(monthDay) + " de " + String(currentMonthName) + " del " + String(currentYear);
    
                     
            Firebase.pushString(firebaseData, nodo + "/Sensor", "salida");
            delay(150);
            Firebase.pushString(firebaseData, nodo + "/Hora", timeClient.getFormattedTime());
            delay(150);
            Firebase.pushString(firebaseData, nodo + "/Fecha", currentDate);
            delay(150);
            Firebase.setInt(firebaseData, nodo + "/Contador", contador);
            delay(150);
            Firebase.end(firebaseData);
        
            Serial.println("Resetting ESP");
            ESP.restart(); //ESP.reset();
        //////////////////////////////
        }
      }


}

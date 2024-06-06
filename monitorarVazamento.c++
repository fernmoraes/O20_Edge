#include <LiquidCrystal_I2C.h>  // Biblioteca para LCD I2C
#include <LiquidCrystal.h>
#include <RTClib.h> // Biblioteca para Relógio em Tempo Real
#include <Wire.h>   // Biblioteca para comunicação I2C
#include <EEPROM.h>

#define PIRPIN 2 // Pino onde o sensor PIR está conectado
#define LOG_OPTION 1 // Opção para ativar a leitura do log
#define SERIAL_OPTION 1 // Opção de comunicação serial: 0 para desligado, 1 para ligado
#define UTC_OFFSET 0 // Ajuste de fuso horário para UTC-3

//Configuração do display
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Configuração do RTC
RTC_DS1307 rtc;

// Configuração da EEPROM (Ainda não foi implementada.)
const int maxRecords = 100; 
const int recordSize = 8; 
int startAdress = 0; 
int endAdress = maxRecords * recordSize; 
int currentAdress = 0; 

//Configuração dos LEDs
int redPin = 3; 
int yellowPin = 4; 
int greenPin = 5; 

  // Marca o tempo de início
unsigned long startTime = millis(); 

  // Variavel da contagem de vazamentos 
  int exits = 0; 
  int previousState = LOW; 

void setup() {
  Serial.begin(9600); 

  lcd.begin(16, 2);

  while(millis() - startTime < 5000){
  lcd.setCursor(0, 0); 
  lcd.print("INICIANDO"); 
  delay(2000); 

    if (!rtc.begin()) {
    lcd.setCursor(0, 0); 
    lcd.print(" RTC não foi iniciado"); 
    while(1); 
  } else {
    lcd.setCursor(0, 1); 
    lcd.print("RTC foi iniciado"); 
    delay(2000); 
  }

  }
  pinMode(PIRPIN, INPUT); 
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT); 

  EEPROM.begin();
}

void loop() {
  int oilExit = digitalRead(PIRPIN); 

  DateTime now = rtc.now();
  
  // Calculando o deslocamento do fuso horário
  int offsetSeconds = UTC_OFFSET * 3600; // Convertendo horas para segundos
  now = now.unixtime() + offsetSeconds; // Adicionando o deslocamento ao tempo atual

  DateTime adjustedTime = DateTime(now); 

  if(oilExit == HIGH && previousState == LOW){
    exits += 1; 

    previousState = oilExit;
  delay(50);

    lcd.setCursor(0, 0); 
    lcd.print((exits)); 
    lcd.print(" VAZAMENTO"); 

    lcd.setCursor(0, 1); 
    lcd.print(adjustedTime.day()); 
    lcd.print("/"); 
    lcd.print(adjustedTime.month()); 
    lcd.print("/"); 
    lcd.print(adjustedTime.year()); 
    lcd.print(" ");
    lcd.print(adjustedTime.hour() < 10 ? "0" : ""); 
    lcd.print(adjustedTime.hour()); 
    lcd.print(":"); 
    lcd.print(adjustedTime.minute() < 10 ? "0" : ""); 
    lcd.print(adjustedTime.minute()); 
    lcd.print(":"); 
    lcd.print(adjustedTime.second() < 10 ? "0" : ""); 
    lcd.print(adjustedTime.second()); 
  }
  previousState = oilExit;
  delay(50);

  if(exits <= 2){
    digitalWrite(greenPin, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(redPin, LOW);
  }

  if(exits >= 3 && oilExit <= 4){
    digitalWrite(greenPin, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(redPin, LOW);
  }

  if(exits >= 5){
    digitalWrite(greenPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(redPin, HIGH);
  }

}
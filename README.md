Aqui está um arquivo README detalhado para o código baseado no circuito mostrado na imagem:

---

# Sistema de Monitoramento de Vazamentos de Óleo

Este projeto utiliza um Arduino Uno para monitorar vazamentos de óleo utilizando um sensor PIR, LEDs indicativos e um display LCD para exibir mensagens. Um RTC (Real Time Clock) é usado para registrar a data e hora dos eventos detectados. Os dados são armazenados em uma EEPROM para leitura posterior.

## Componentes Utilizados

1. **Arduino Uno**
2. **Sensor PIR**
3. **LEDs (Vermelho, Amarelo e Verde)**
4. **Display LCD 16x2**
5. **RTC DS1307**
6. **EEPROM**
7. **Resistores e fios jumper**

## Conexões

1. **Sensor PIR**:
   - VCC: 5V
   - GND: GND
   - OUT: Pino digital 2

2. **LEDs**:
   - LED Vermelho: Pino digital 3
   - LED Amarelo: Pino digital 4
   - LED Verde: Pino digital 5

3. **Display LCD 16x2**:
   - RS: Pino digital 12
   - EN: Pino digital 11
   - D4: Pino digital 10
   - D5: Pino digital 9
   - D6: Pino digital 8
   - D7: Pino digital 7
   - VCC: 5V
   - GND: GND

4. **RTC DS1307**:
   - SDA: A4
   - SCL: A5
   - VCC: 5V
   - GND: GND

## Código

```cpp
#include <LiquidCrystal_I2C.h>  // Biblioteca para LCD I2C
#include <LiquidCrystal.h>
#include <RTClib.h> // Biblioteca para Relógio em Tempo Real
#include <Wire.h>   // Biblioteca para comunicação I2C
#include <EEPROM.h>

#define PIRPIN 2 // Pino onde o sensor PIR está conectado
#define LOG_OPTION 1 // Opção para ativar a leitura do log
#define SERIAL_OPTION 1 // Opção de comunicação serial: 0 para desligado, 1 para ligado
#define UTC_OFFSET 0 // Ajuste de fuso horário para UTC-3

// Configuração do display
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Configuração do RTC
RTC_DS1307 rtc;

// Configuração da EEPROM (Ainda não foi implementada.)
const int maxRecords = 100; 
const int recordSize = 8; 
int startAdress = 0; 
int endAdress = maxRecords * recordSize; 
int currentAdress = 0; 

// Configuração dos LEDs
int redPin = 3; 
int yellowPin = 4; 
int greenPin = 5; 

// Marca o tempo de início
unsigned long startTime = millis(); 

// Variável da contagem de vazamentos 
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
```

## Descrição do Funcionamento

1. **Configuração Inicial**:
   - O display LCD exibe "INICIANDO" durante os primeiros 5 segundos.
   - O RTC é inicializado e sua presença é verificada.
   - Os pinos para o sensor PIR e LEDs são configurados.

2. **Loop Principal**:
   - O sensor PIR monitora a detecção de vazamentos.
   - A data e hora atualizadas são obtidas do RTC.
   - Quando um vazamento é detectado (mudança de estado do PIR), a contagem de vazamentos é incrementada e o evento é exibido no LCD com a data e hora do vazamento.
   - Os LEDs indicam o nível de perigo com base na quantidade de vazamentos detectados:
     - Verde: Menos de 2 vazamentos
     - Amarelo: Entre 3 e 4 vazamentos
     - Vermelho: 5 ou mais vazamentos

## Futuras Implementações

- **Registro em EEPROM**: Armazenar eventos de vazamento na EEPROM para posterior análise.
- **Comunicação Serial**: Enviar dados de vazamentos via comunicação serial para um monitor externo.

## Licença

Este projeto está licenciado sob a [MIT License](LICENSE).

## Contato

Se você tiver alguma dúvida, entre em contato através do email: rm555080@fiap.com.br

## Alunos

Fernando Navajas RM555080
Henrique Botti RM556187
Pedro Ferrari RM554887

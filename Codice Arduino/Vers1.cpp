#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>

// ========================= LCD =========================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ========================= PIN SENSORI =========================
int tempPin[4] = {A0, A1, A2, A3};
int umidPin[4] = {A4, A5, A6, A7};
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int ldrPin = A8;

// ========================= LED TEST =========================
const int LED_VENTOLE_IN  = 3;
const int LED_VENTOLE_OUT = 4;
const int LED_POMP[4]     = {5,6,7,8};

// ========================= VARIABILI MISURE =========================
float Temp[4];
int Umid[4];
float TempSerra;
float UmiditaSerra;
int Irraggiamento;

// ========================= VARIABILI SOGLIE =========================
float SogliaTemp[4];
float SogliaUmidita[4];
float SogliaTempSerra;
float SogliaUmiditaSerra;
int SogliaIrraggiamento;

// ========================= VARIABILI ATTUATORI =========================
bool VentoleIN, VentoleOUT;
bool Pomp[4];

// ========================= ETHERNET =========================
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);   // Cambia se serve
EthernetServer server(80);

// ========================= SETUP =========================
void setup() {

  lcd.init();
  lcd.backlight();
  dht.begin();

  InizializzaSoglie();

  pinMode(LED_VENTOLE_IN, OUTPUT);
  pinMode(LED_VENTOLE_OUT, OUTPUT);
  for(int i=0;i<4;i++) pinMode(LED_POMP[i], OUTPUT);

  InizializzaEthernet();
}

// ========================= SENSORI =========================
float leggiLM35(int pin) {
  int val = analogRead(pin);
  float tensione = val * (5.0 / 1023.0);
  return tensione * 100;
}

int leggiUmidita(int pin) {
  int val = analogRead(pin);
  return map(val, 1023, 0, 0, 100);
}

int leggiLuce(int pin) {
  int val = analogRead(pin);
  return map(val, 1023, 0, 0, 100);
}

float leggiTempAria() { return dht.readTemperature(); }
float leggiUmiditaAria() { return dht.readHumidity(); }

// ========================= DISPLAY =========================
void mostraPiante(int idx1,int idx2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P1 T="); lcd.print((int)Temp[idx1]); lcd.print((char)223); lcd.print(" U="); lcd.print(Umid[idx1]);
  lcd.setCursor(0,1);
  lcd.print("P2 T="); lcd.print((int)Temp[idx2]); lcd.print((char)223); lcd.print(" U="); lcd.print(Umid[idx2]);
  delay(2000);
}

void mostraPiante2(int idx3,int idx4){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P3 T="); lcd.print((int)Temp[idx3]); lcd.print((char)223); lcd.print(" U="); lcd.print(Umid[idx3]);
  lcd.setCursor(0,1);
  lcd.print("P4 T="); lcd.print((int)Temp[idx4]); lcd.print((char)223); lcd.print(" U="); lcd.print(Umid[idx4]);
  delay(2000);
}

void mostraAmbiente(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Aria T="); lcd.print((int)TempSerra); lcd.print((char)223); lcd.print(" U="); lcd.print((int)UmiditaSerra); lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Luce: "); lcd.print(Irraggiamento); lcd.print("%");
  delay(2000);
}

// ========================= CONTROLLO =========================
void InizializzaSoglie(){
  for(int i=0;i<4;i++){
    SogliaTemp[i]=25;
    SogliaUmidita[i]=50;
  }
  SogliaTempSerra=25;
  SogliaUmiditaSerra=60;
  SogliaIrraggiamento=50;
}

void ControlloVentole() {
  bool statoVentole = false;

  if (TempSerra > SogliaTempSerra) statoVentole = true;
  for (int i = 0; i < 4; i++)
    if (Temp[i] > SogliaTemp[i]) statoVentole = true;

  VentoleIN  = statoVentole;
  VentoleOUT = statoVentole;

  digitalWrite(LED_VENTOLE_IN,  VentoleIN);
  digitalWrite(LED_VENTOLE_OUT, VentoleOUT);
}

void ControlloPompe(){
  for(int i=0;i<4;i++){
    Pomp[i]=(Umid[i]<=SogliaUmidita[i]);
    digitalWrite(LED_POMP[i], Pomp[i]);
  }
  if(UmiditaSerra>SogliaUmiditaSerra && TempSerra<SogliaTempSerra){
    for(int i=0;i<4;i++){
      Pomp[i]=false;
      digitalWrite(LED_POMP[i], LOW);
    }
  }
}
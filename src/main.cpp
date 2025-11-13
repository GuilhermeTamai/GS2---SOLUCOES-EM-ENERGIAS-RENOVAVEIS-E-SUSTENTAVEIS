#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

#define POT1 34 // Simula tensão
#define POT2 35 // Simula corrente
#define botaoOnOff 14
#define LED_STATUS 4

LiquidCrystal_I2C lcd(0x27, 20, 4);

float tensao, corrente, potencia, energiaAcumulada;
int valorPOT_V, valorPOT_I;
bool sistemaLigado = false;
unsigned long tempoInicio = 0;
unsigned long tempoLigado = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(botaoOnOff, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  // Cabeçalho atualizado
  lcd.setCursor(0, 0); lcd.print(" Medidor de Energia ");
  lcd.setCursor(0, 1); lcd.print("   Grupo GS2 - 1CCPG ");
  lcd.setCursor(0, 2); lcd.print("     Caio e Vitor     ");
  lcd.setCursor(0, 3); lcd.print("     Guilherme        ");
  delay(3000); lcd.clear();
}

void loop() {
  static bool botaoAnterior = HIGH;
  static unsigned long ultimoToque = 0;
  const unsigned long tempoDebounce = 200;

  bool botaoAtual = digitalRead(botaoOnOff);

  // Detecta toque único com debounce
  if (botaoAnterior == HIGH && botaoAtual == LOW && millis() - ultimoToque > tempoDebounce) {
    sistemaLigado = !sistemaLigado;
    ultimoToque = millis();

    if (sistemaLigado) {
      tempoInicio = millis();
      digitalWrite(LED_STATUS, HIGH);
    } else {
      tempoLigado += millis() - tempoInicio;
      digitalWrite(LED_STATUS, LOW);
    }
  }

  botaoAnterior = botaoAtual;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status: ");
  lcd.print(sistemaLigado ? "ON" : "OFF");

  if (sistemaLigado) {
    valorPOT_V = analogRead(POT1);
    valorPOT_I = analogRead(POT2);

    tensao = map(valorPOT_V, 0, 4095, 0, 2400) / 10.0;
    corrente = map(valorPOT_I, 0, 4095, 0, 200) / 10.0;
    potencia = tensao * corrente;

    unsigned long tempoAtual = millis() - tempoInicio;
    float tempoSegundos = tempoAtual / 1000.0;
    float energiaAtual = potencia * tempoSegundos / 3600.0;
    energiaAcumulada += energiaAtual;

    lcd.setCursor(0, 1);
    lcd.print("V: "); lcd.print(tensao); lcd.print("V");

    lcd.setCursor(0, 2);
    lcd.print("I: "); lcd.print(corrente); lcd.print("A");

    lcd.setCursor(0, 3);
    lcd.print("E: "); lcd.print(energiaAcumulada, 2); lcd.print("Wh");
  } else {
    lcd.setCursor(0, 1); lcd.print("V: ---");
    lcd.setCursor(0, 2); lcd.print("I: ---");
    lcd.setCursor(0, 3); lcd.print("E: ---");
  }

  delay(500);
}
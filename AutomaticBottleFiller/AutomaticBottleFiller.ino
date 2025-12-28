#include <Arduino.h>
#include <LiquidCrystal.h>

// zmienne dla endkodera
const int ENC_A = 9;
const int ENC_B = 10;
long encoderValue = 0;
int lastA;

// zmienne dla przyciskow
const int BtnD0 = 0;
const int BtnD1 = 1;
const int BtnD2 = 2;
const int BtnD3 = 3;
bool lastBtnD0 = HIGH;
bool lastBtnD2 = HIGH;
bool lastRawD0 = HIGH;
bool lastRawD2 = HIGH;
bool debouncedBtnD0 = HIGH;
bool debouncedBtnD2 = HIGH;
const unsigned long debounceDelayMs = 30;
unsigned long lastDebounceD0 = 0;
unsigned long lastDebounceD2 = 0;

// zmienne dla diod
const int DiodaD4 = 4;
const int DiodaD5 = 5;
const int DiodaD6 = 6;
const int DiodaD7 = 7;

// LCD
LiquidCrystal lcd(22, 21, 20, 19, 18, 17, 16);

// Stany menu
enum State
{
  MAIN_SCREEN,
  NASTAWA,
  PRACA
};
State currentState = MAIN_SCREEN;
State lastState = MAIN_SCREEN;

void setup()
{
  lcd.begin(16, 2);
  lcd.print("AutomaticFiller");
  delay(1000);
  lcd.clear();

  // Piny enkodera i ustawienia
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  lastA = digitalRead(ENC_A);

  // PINMODE dla przycisków
  pinMode(BtnD0, INPUT_PULLUP);
  pinMode(BtnD1, INPUT_PULLUP);
  pinMode(BtnD2, INPUT_PULLUP);
  pinMode(BtnD3, INPUT_PULLUP);

  // PINMODE dla diód
  pinMode(DiodaD4, OUTPUT);
  digitalWrite(DiodaD4, HIGH);
  pinMode(DiodaD5, OUTPUT);
  digitalWrite(DiodaD5, HIGH);
  pinMode(DiodaD6, OUTPUT);
  digitalWrite(DiodaD6, HIGH);
  pinMode(DiodaD7, OUTPUT);
  digitalWrite(DiodaD7, HIGH);
}

void Enkoder()
{
  int a = digitalRead(ENC_A);

  // reagujemy tylko na zbocze narastające
  if (lastA == LOW && a == HIGH)
  {
    if (digitalRead(ENC_B) == LOW) // kierunek w prawo
      encoderValue++;
    else // kierunek w lewo
      encoderValue--;

    lcd.setCursor(0, 1);
    lcd.print("Val: ");
    lcd.print(encoderValue);
    lcd.print("   "); // proste czyszczenie końcówki
  }

  lastA = a;
}

void MainScreen()
{
  lcd.setCursor(0, 0);
  lcd.print("Nastawa - N (D0)");
  lcd.setCursor(0, 1);
  lcd.print("Praca   - P (D2)");
}

void Nastawa()
{
  lcd.setCursor(0, 0);
  lcd.print("Nastawa");
  Enkoder();
}

void Praca()
{
  lcd.setCursor(0, 0);
  lcd.print("Praca");
}

void Waga()
{
}

void loop()
{
  bool rawBtnD0 = digitalRead(BtnD0);
  bool rawBtnD2 = digitalRead(BtnD2);

  if (rawBtnD0 != lastRawD0)
  {
    lastDebounceD0 = millis();
    lastRawD0 = rawBtnD0;
  }

  if (rawBtnD2 != lastRawD2)
  {
    lastDebounceD2 = millis();
    lastRawD2 = rawBtnD2;
  }

  if ((millis() - lastDebounceD0) > debounceDelayMs)
  {
    debouncedBtnD0 = rawBtnD0;
  }

  if ((millis() - lastDebounceD2) > debounceDelayMs)
  {
    debouncedBtnD2 = rawBtnD2;
  }
  // ---- reakcja na zmianę stanu ----
  if (currentState != lastState)
  {
    lcd.clear();

    switch (currentState)
    {
    case MAIN_SCREEN:
      MainScreen();
      break;

    case NASTAWA:
      Nastawa();
      break;

    case PRACA:
      Praca();
      break;
    }

    lastState = currentState;
  }

  // ---- logika stanów ----
  switch (currentState)
  {
  case MAIN_SCREEN:
    if (lastBtnD0 == HIGH && debouncedBtnD0 == LOW)
      currentState = NASTAWA;

    else if (lastBtnD2 == HIGH && debouncedBtnD2 == LOW)
      currentState = PRACA;
    break;

  case NASTAWA:
    Enkoder();

    if (lastBtnD0 == HIGH && debouncedBtnD0 == LOW)
      currentState = MAIN_SCREEN;
    break;

  case PRACA:
    if (lastBtnD2 == HIGH && debouncedBtnD2 == LOW)
      currentState = MAIN_SCREEN;
    break;
  }
  lastBtnD0 = debouncedBtnD0;
  lastBtnD2 = debouncedBtnD2;
}

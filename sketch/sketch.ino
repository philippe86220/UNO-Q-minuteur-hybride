#include <Arduino_RouterBridge.h>
#include <IskakINO_LiquidCrystal_I2C.h>
#include <Modulino.h>

ModulinoButtons buttons;
ModulinoBuzzer buzzer;
LiquidCrystal_I2C lcd(20, 4);


int secondes = 0;
bool enCours = false;

void calcul(int secondes) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(secondes / 60);
  lcd.print(",");
  lcd.print(secondes % 60);
  lcd.print(" minute(s)");
  lcd.setCursor(0, 1);
  lcd.print(secondes);
  lcd.print(" Secondes");
}

void afficheSecondes() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Secondes : ");
  lcd.print(secondes);
}

void afficheChrono(int ecoule, int restant) {

  lcd.setCursor(0, 2);
  lcd.print("Ecoule : ");
  lcd.setCursor(11, 2);
  lcd.print(ecoule);
  lcd.print("     ");

  lcd.setCursor(0, 3);
  lcd.print("Restant : ");
  lcd.setCursor(11, 3);
  lcd.print(restant);
  lcd.print("     ");
}

void minuteur(int ecoule, int restant) {
  afficheChrono(ecoule, restant);
}

void joueFin() {
  enCours = false;

  for (uint8_t x = 0; x < 10; x++) {
    buzzer.tone(2800, 200);
    delay(250);
  }
}

void setup() {
  Bridge.begin();
  Monitor.begin();

  Modulino.begin();
  buttons.begin();
  buzzer.begin();

  lcd.begin();
  lcd.backlight();

  Bridge.provide("minuteur", minuteur);
  afficheSecondes();
  Bridge.provide("fin", joueFin);

  // Preuve de vie
  buzzer.tone(1000, 200);
  delay(300);
  buzzer.tone(1500, 200);
  delay(400);
}

void loop() {
  if (buttons.update()) {

    if (buttons.isPressed('A')) {
      enCours = true;
      calcul(secondes);
      Bridge.call("api_temps", secondes);
    }

    if (buttons.isPressed('B')) {
      if (enCours) return;
      secondes += 10;
      if (secondes > 600) secondes = 0;
      afficheSecondes();
    }

    if (buttons.isPressed('C')) {
      Bridge.call("stop_timer");
      secondes = 0;
      enCours = false;
      afficheSecondes();
    }
  }
}

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =========================
// Simulatore rotore AZ + EL con OLED
// AZ potenziometro su A0
// EL potenziometro su A1
// OLED SSD1306 I2C 128x32 - versione COMPACT con barre - v1.2
// =========================
// Versione 1.2: barre + frecce movimento + linea orizzontale a riposo

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String rxLine = "";

float azNow = 0.0;
float elNow = 0.0;

float azTarget = 0.0;
float elTarget = 0.0;

float azPrev = 0.0;
float elPrev = 0.0;

unsigned long lastReport = 0;
unsigned long lastMove = 0;
unsigned long lastDisplay = 0;

// Velocità movimento simulato
float azSpeed = 0.5;   // gradi per step
float elSpeed = 0.3;   // gradi per step

// Soglia per considerare un asse "in movimento"
const float MOVE_THRESHOLD = 0.50;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;); // blocco se OLED non trovato
  }

  splashScreen();
}

void loop() {
  // Leggi potenziometri come TARGET
  int rawAZ = analogRead(A0);
  int rawEL = analogRead(A1);

  azTarget = map(rawAZ, 0, 1023, 0, 3600) / 10.0;
  elTarget = map(rawEL, 0, 1023, 0, 900) / 10.0;

  // Movimento graduale realistico
  if (millis() - lastMove > 50) {
    azPrev = azNow;
    elPrev = elNow;
    moveRotor();
    lastMove = millis();
  }

  // Lettura comandi seriali
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\r' || c == '\n') {
      if (rxLine.length() > 0) {
        parseCommand(rxLine);
        rxLine = "";
      }
    } else {
      rxLine += c;
    }
  }

  // Report seriale posizione
  if (millis() - lastReport >= 1000) {
    sendPosition();
    lastReport = millis();
  }

  // Aggiorna display
  if (millis() - lastDisplay >= 100) {
    updateDisplay();
    lastDisplay = millis();
  }
}

void splashScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(12, 5);
  display.print("AZ/EL SIMULATOR");

  display.setCursor(36, 19);
  display.print("IK1APW");

  display.display();
  delay(3000);
}

void moveRotor() {
  // AZ
  if (abs(azNow - azTarget) > azSpeed) {
    if (azNow < azTarget) azNow += azSpeed;
    else azNow -= azSpeed;
  } else {
    azNow = azTarget;
  }

  // EL
  if (abs(elNow - elTarget) > elSpeed) {
    if (elNow < elTarget) elNow += elSpeed;
    else elNow -= elSpeed;
  } else {
    elNow = elTarget;
  }
}

void parseCommand(String s) {
  s.trim();
  s.toUpperCase();

  // Comandi semplici di refresh
  if (s == "C" || s == "GET" || s == "AZ" || s == "EL") {
    sendPosition();
    return;
  }

  // Se arrivano righe contenenti AZ o EL, rispondi comunque con la posizione attuale
  if (s.indexOf("AZ") >= 0 || s.indexOf("EL") >= 0) {
    sendPosition();
  }
}

void sendPosition() {
  Serial.print("AZ");
  Serial.print(azNow, 1);
  Serial.print(" EL");
  Serial.println(elNow, 1);
}

#define ARROW_DEADBAND_AZ 2.0
#define ARROW_DEADBAND_EL 1.5

char getAZDir() {
  float d = azTarget - azNow;

  if (d > 2.0) return 'R';
  if (d < -2.0) return 'L';
  return '-';
}

char getELDir() {
  float d = elTarget - elNow;

  if (d > 1.0) return 'U';
  if (d < -1.0) return 'D';
  return '-';
}

void drawArrow(int x, int y, char dir) {
  switch (dir) {
    case 'R': // Freccia verso destra / CW
      display.drawLine(x, y, x + 10, y, SSD1306_WHITE);
      display.drawLine(x + 10, y, x + 6, y - 3, SSD1306_WHITE);
      display.drawLine(x + 10, y, x + 6, y + 3, SSD1306_WHITE);
      break;

    case 'L': // Freccia verso sinistra / CCW
      display.drawLine(x, y, x + 10, y, SSD1306_WHITE);
      display.drawLine(x, y, x + 4, y - 3, SSD1306_WHITE);
      display.drawLine(x, y, x + 4, y + 3, SSD1306_WHITE);
      break;

    case 'U': // Freccia verso alto
      display.drawLine(x, y + 6, x, y - 4, SSD1306_WHITE);
      display.drawLine(x, y - 4, x - 3, y, SSD1306_WHITE);
      display.drawLine(x, y - 4, x + 3, y, SSD1306_WHITE);
      break;

    case 'D': // Freccia verso basso
      display.drawLine(x, y - 4, x, y + 6, SSD1306_WHITE);
      display.drawLine(x, y + 6, x - 3, y + 2, SSD1306_WHITE);
      display.drawLine(x, y + 6, x + 3, y + 2, SSD1306_WHITE);
      break;

    default: // Rotore fermo: linea orizzontale
      display.drawLine(x - 4, y, x + 4, y, SSD1306_WHITE);
      break;
  }
}
void drawBar(int x, int y, int w, int h, float value, float maxValue) {
  display.drawRect(x, y, w, h, SSD1306_WHITE);

  int fillW = (int)((value / maxValue) * (w - 2));
  if (fillW < 0) fillW = 0;
  if (fillW > w - 2) fillW = w - 2;

  display.fillRect(x + 1, y + 1, fillW, h - 2, SSD1306_WHITE);
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Riga AZ
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("AZ");

  display.setTextSize(2);
  display.setCursor(18, 0);
  if (azNow < 100) display.print(" ");
  if (azNow < 10)  display.print(" ");
  display.print((int)round(azNow));

  drawArrow(72, 8, getAZDir());
  drawBar(88, 2, 38, 10, azNow, 360.0);

  // Riga EL
  display.setTextSize(1);
  display.setCursor(0, 17);
  display.print("EL");

  display.setTextSize(2);
  display.setCursor(18, 16);
  if (elNow < 10) display.print(" ");
  display.print((int)round(elNow));

  drawArrow(72, 24, getELDir());
  drawBar(88, 19, 38, 10, elNow, 90.0);

  display.display();
}

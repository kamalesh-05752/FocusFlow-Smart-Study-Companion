
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

// Pin configuration
const byte START_BUTTON = 2;
const byte RESET_BUTTON = 3;
const byte LED_PIN = 4;
const byte BUZZER_PIN = 5;
const byte LDR_PIN = A0;

// Session durations
const unsigned long WORK_TIME = 25UL * 60UL;
const unsigned long BREAK_TIME = 5UL * 60UL;

// Button timing
const unsigned long DEBOUNCE_MS = 35;
const unsigned long DOUBLE_CLICK_MS = 350;

// Light threshold (calibrate after testing)
const int LOW_LIGHT_THRESHOLD = 300;

// Timer state
bool timerRunning = false;
bool workMode = true;

unsigned long remainingSeconds = WORK_TIME;
unsigned long lastSecondUpdate = 0;

// Button state
bool lastStartReading = HIGH;
bool stableStartState = HIGH;
unsigned long startDebounceTime = 0;

bool lastResetReading = HIGH;
bool stableResetState = HIGH;
unsigned long resetDebounceTime = 0;

bool waitingForSecondClick = false;
unsigned long firstClickTime = 0;

// Buzzer alert state
bool alarmActive = false;
unsigned long alarmStartTime = 0;
const unsigned long ALARM_DURATION_MS = 5000;

// OLED refresh
unsigned long lastDisplayUpdate = 0;

// --------------------------------------------------
// Initialize
// --------------------------------------------------
void setup() {
  Serial.begin(9600);

  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("OLED initialization failed"));
    while (true) {
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println(F("FOCUS FLOW"));
  display.setTextSize(1);
  display.setCursor(22, 40);
  display.println(F("Smart Study Timer"));
  display.display();

  delay(1200);
  showDisplay();
}

// --------------------------------------------------
// Main loop
// --------------------------------------------------
void loop() {
  unsigned long now = millis();

  handleStartButton(now);
  handleResetButton(now);
  updateTimer(now);
  updateAlarm(now);
  updateLED();
  updateDisplay(now);
}

// --------------------------------------------------
// Debounced start / pause button
// --------------------------------------------------
void handleStartButton(unsigned long now) {
  bool reading = digitalRead(START_BUTTON);

  if (reading != lastStartReading) {
    startDebounceTime = now;
    lastStartReading = reading;
  }

  if (now - startDebounceTime >= DEBOUNCE_MS) {
    if (reading != stableStartState) {
      stableStartState = reading;

      if (stableStartState == LOW) {
        timerRunning = !timerRunning;
        lastSecondUpdate = now;

        Serial.println(
          timerRunning ? F("Timer started")
                       : F("Timer paused")
        );
      }
    }
  }
}

// --------------------------------------------------
// Reset button:
// Single click = reset current session
// Double click = switch work / break mode
// --------------------------------------------------
void handleResetButton(unsigned long now) {
  bool reading = digitalRead(RESET_BUTTON);

  if (reading != lastResetReading) {
    resetDebounceTime = now;
    lastResetReading = reading;
  }

  if (now - resetDebounceTime >= DEBOUNCE_MS) {
    if (reading != stableResetState) {
      stableResetState = reading;

      if (stableResetState == LOW) {
        if (waitingForSecondClick &&
            now - firstClickTime <= DOUBLE_CLICK_MS) {

          waitingForSecondClick = false;
          toggleMode();

        } else {
          waitingForSecondClick = true;
          firstClickTime = now;
        }
      }
    }
  }

  // Execute single click only after the double-click window
  if (waitingForSecondClick &&
      now - firstClickTime > DOUBLE_CLICK_MS) {

    waitingForSecondClick = false;
    resetSession();
  }
}

// --------------------------------------------------
// Reset current session
// --------------------------------------------------
void resetSession() {
  remainingSeconds = workMode ? WORK_TIME : BREAK_TIME;
  timerRunning = false;

  stopAlarm();

  Serial.println(F("Session reset"));
}

// --------------------------------------------------
// Switch between work and break
// --------------------------------------------------
void toggleMode() {
  workMode = !workMode;

  remainingSeconds = workMode ? WORK_TIME : BREAK_TIME;
  timerRunning = false;

  stopAlarm();

  Serial.println(
    workMode ? F("WORK MODE") : F("BREAK MODE")
  );
}

// --------------------------------------------------
// Countdown timer
// --------------------------------------------------
void updateTimer(unsigned long now) {
  if (!timerRunning) {
    return;
  }

  if (now - lastSecondUpdate >= 1000) {
    unsigned long elapsed =
      (now - lastSecondUpdate) / 1000;

    lastSecondUpdate += elapsed * 1000;

    if (elapsed >= remainingSeconds) {
      remainingSeconds = 0;
      timerRunning = false;
      startAlarm(now);

      Serial.println(F("Session completed"));
    } else {
      remainingSeconds -= elapsed;
    }
  }
}

// --------------------------------------------------
// Buzzer sounds only when a session ends
// --------------------------------------------------
void startAlarm(unsigned long now) {
  alarmActive = true;
  alarmStartTime = now;

  tone(BUZZER_PIN, 2000);
}

void updateAlarm(unsigned long now) {
  if (alarmActive &&
      now - alarmStartTime >= ALARM_DURATION_MS) {
    stopAlarm();
  }
}

void stopAlarm() {
  alarmActive = false;
  noTone(BUZZER_PIN);
}

// --------------------------------------------------
// LED indication
// --------------------------------------------------
void updateLED() {
  // LED on during an active countdown
  digitalWrite(LED_PIN, timerRunning ? HIGH : LOW);
}

// --------------------------------------------------
// OLED display refresh
// --------------------------------------------------
void updateDisplay(unsigned long now) {
  if (now - lastDisplayUpdate >= 250) {
    lastDisplayUpdate = now;
    showDisplay();
  }
}

void showDisplay() {
  int lightValue = analogRead(LDR_PIN);

  unsigned long minutes = remainingSeconds / 60;
  unsigned long seconds = remainingSeconds % 60;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Project title
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("FOCUS FLOW"));

  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  // Session mode
  display.setCursor(0, 15);
  display.print(F("Mode: "));
  display.println(workMode ? F("WORK") : F("BREAK"));

  // Countdown
  display.setTextSize(2);
  display.setCursor(16, 27);

  if (minutes < 10) display.print('0');
  display.print(minutes);
  display.print(':');

  if (seconds < 10) display.print('0');
  display.print(seconds);

  // Status
  display.setTextSize(1);
  display.setCursor(0, 48);

  if (timerRunning) {
    display.print(F("RUNNING"));
  } else if (remainingSeconds == 0) {
    display.print(F("SESSION DONE"));
  } else {
    display.print(F("PAUSED"));
  }

  // Light sensor
  display.setCursor(70, 48);
  display.print(F("LDR:"));
  display.print(lightValue);

  display.setCursor(0, 57);
  if (lightValue < LOW_LIGHT_THRESHOLD) {
    display.print(F("Low light detected"));
  } else {
    display.print(F("Light level OK"));
  }

  display.display();
}

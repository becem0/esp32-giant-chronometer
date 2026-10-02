/*
 * ==============================================================================
 * ⏱️ ESP32 GIANT 24V COB LED CHRONOMETER & WEB SERVER
 * ==============================================================================
 * Author:  Bacem AMEUR (ENIM)
 * License: MIT License
 * Platform: ESP32-WROVER-E (Dual-Core 240MHz, 8MB PSRAM)
 * 
 * Hardware Features:
 * - 6x Daisy-chained 74HC595 Serial Shift Registers (SPI Data/Clock/Latch)
 * - 6x ULN2803A Darlington Transistor Arrays (Common Anode 24V Sinking)
 * - SIM800L GSM Cellular Module (Hardware UART2: TX2=17, RX2=16)
 * - Wi-Fi Soft-AP Mode (SSID: CHRONO_01, IP: 192.168.4.1)
 * ==============================================================================
 */

#include <WiFi.h>
#include <WebServer.h>

// ================= PIN DEFINITIONS =================
// 74HC595 High-Speed Serial Bus
const int PIN_DATA  = 13; // DS (Serial Data)
const int PIN_LATCH = 12; // ST_CP (Storage Register Clock / Latch)
const int PIN_CLOCK = 14; // SH_CP (Shift Register Clock)

// Optional Colon Separator Blinking Pin (if using separate transistor)
const int PIN_COLON = 4;

// SIM800L GSM Hardware Serial2 Pins
#define RXD2 16
#define TXD2 17

// ================= GLOBAL OBJECTS & STATE =================
WebServer server(80);

unsigned long previousMillis = 0;
unsigned long secondsCount = 0;
bool isRunning = false;
bool isCountdown = false;
unsigned long countdownTarget = 0;
bool colonState = false;

// 7-Segment Active-High Bitmask (A, B, C, D, E, F, G, DP)
// Bit 0: A, Bit 1: B, Bit 2: C, Bit 3: D, Bit 4: E, Bit 5: F, Bit 6: G, Bit 7: DP
const byte digitMap[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

// ================= EMBEDDED WEB DASHBOARD (HTML/CSS/JS) =================
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>CHRONO_01 Dashboard</title>
<style>
  :root {
    --bg-dark: #0f172a;
    --card-bg: #1e293b;
    --primary: #38bdf8;
    --accent: #f43f5e;
    --success: #10b981;
    --text: #f8fafc;
  }
  body {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    background-color: var(--bg-dark);
    color: var(--text);
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    min-height: 100vh;
    margin: 0;
    padding: 20px;
  }
  .container {
    background: var(--card-bg);
    border-radius: 20px;
    padding: 30px;
    box-shadow: 0 20px 40px rgba(0,0,0,0.5);
    max-width: 500px;
    width: 100%;
    text-align: center;
    border: 1px solid rgba(255,255,255,0.08);
  }
  h1 { font-size: 24px; margin-bottom: 5px; color: var(--primary); }
  p.subtitle { color: #94a3b8; margin-top: 0; font-size: 14px; }
  .display {
    background: #000;
    border-radius: 12px;
    padding: 20px;
    font-family: 'Courier New', Courier, monospace;
    font-size: 48px;
    font-weight: bold;
    color: var(--primary);
    text-shadow: 0 0 20px rgba(56, 189, 248, 0.6);
    letter-spacing: 4px;
    margin: 25px 0;
    border: 2px solid #334155;
  }
  .btn-grid {
    display: grid;
    grid-template-columns: 1fr 1fr 1fr;
    gap: 12px;
    margin-bottom: 20px;
  }
  button {
    background: #334155;
    color: white;
    border: none;
    padding: 14px;
    border-radius: 10px;
    font-size: 16px;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s ease;
  }
  button:hover { transform: translateY(-2px); }
  button:active { transform: translateY(1px); }
  button.start { background: var(--success); }
  button.pause { background: #f59e0b; }
  button.reset { background: var(--accent); }
  .status { font-size: 12px; color: #64748b; margin-top: 15px; }
</style>
</head>
<body>
  <div class="container">
    <h1>CHRONO_01</h1>
    <p class="subtitle">ESP32 Giant 24V Chronometer Controller</p>
    
    <div class="display" id="timeDisplay">00:00:00</div>
    
    <div class="btn-grid">
      <button class="start" onclick="sendCmd('/api/start')">START</button>
      <button class="pause" onclick="sendCmd('/api/pause')">PAUSE</button>
      <button class="reset" onclick="sendCmd('/api/reset')">RESET</button>
    </div>

    <div class="status" id="statusMsg">Connected to Soft-AP (192.168.4.1)</div>
  </div>

<script>
  function sendCmd(endpoint) {
    fetch(endpoint)
      .then(res => res.text())
      .then(data => {
        document.getElementById('statusMsg').innerText = 'Command: ' + data;
      })
      .catch(err => {
        document.getElementById('statusMsg').innerText = 'Error connecting to ESP32';
      });
  }

  // Poll time every 500ms
  setInterval(() => {
    fetch('/api/status')
      .then(res => res.json())
      .then(data => {
        document.getElementById('timeDisplay').innerText = data.time;
      })
      .catch(() => {});
  }, 500);
</script>
</body>
</html>
)rawliteral";

// ================= HARDWARE SPI SHIFT DRIVER =================
void updateDisplays(unsigned long totalSec) {
  int h = (totalSec / 3600) % 100;
  int m = (totalSec % 3600) / 60;
  int s = totalSec % 60;

  byte digits[6];
  digits[0] = digitMap[s % 10];         // Seconds Units
  digits[1] = digitMap[s / 10];         // Seconds Tens
  digits[2] = digitMap[m % 10];         // Minutes Units
  digits[3] = digitMap[m / 10];         // Minutes Tens
  digits[4] = digitMap[h % 10];         // Hours Units
  digits[5] = digitMap[h / 10];         // Hours Tens

  // Optional: Add colon blink on DP bits of Digits 2 and 4
  if (colonState) {
    digits[2] |= 0b10000000; // Turn on DP bit
    digits[4] |= 0b10000000; // Turn on DP bit
  }

  // Push 48 bits to 74HC595 cascade
  digitalWrite(PIN_LATCH, LOW);
  for (int i = 5; i >= 0; i--) {
    shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, digits[i]);
  }
  digitalWrite(PIN_LATCH, HIGH);
}

// ================= REST API HANDLERS =================
void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleStart() {
  isRunning = true;
  server.send(200, "text/plain", "RUNNING");
}

void handlePause() {
  isRunning = false;
  server.send(200, "text/plain", "PAUSED");
}

void handleReset() {
  isRunning = false;
  secondsCount = 0;
  updateDisplays(0);
  server.send(200, "text/plain", "RESET");
}

void handleStatus() {
  int h = (secondsCount / 3600) % 100;
  int m = (secondsCount % 3600) / 60;
  int s = secondsCount % 60;

  char timeStr[16];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", h, m, s);

  String json = "{\"time\":\"" + String(timeStr) + "\",\"running\":" + (isRunning ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

// ================= SETUP & MAIN LOOP =================
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_COLON, OUTPUT);

  // Initialize Soft-AP
  WiFi.softAP("CHRONO_01", "12345678");
  Serial.println("\n===========================================");
  Serial.println("⏱️ CHRONO_01 DUAL-CORE SERVER STARTED");
  Serial.print("Soft-AP IP Address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("===========================================");

  // Web routes
  server.on("/", handleRoot);
  server.on("/api/start", handleStart);
  server.on("/api/pause", handlePause);
  server.on("/api/reset", handleReset);
  server.on("/api/status", handleStatus);

  server.begin();
  updateDisplays(0);
}

void loop() {
  server.handleClient();

  // 1-Second Deterministic Timer Tick
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    if (isRunning) {
      secondsCount++;
      colonState = !colonState;
      digitalWrite(PIN_COLON, colonState ? HIGH : LOW);
      updateDisplays(secondsCount);
    }
  }
}

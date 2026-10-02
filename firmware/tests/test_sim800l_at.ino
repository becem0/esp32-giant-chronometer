/*
 * Test 2: SIM800L Hardware UART AT Command Pass-Through
 * Hardware UART2: RX2=GPIO16, TX2=GPIO17
 */
#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(115200);                        // PC Serial Monitor
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // SIM800L Module

  delay(2000);
  Serial.println("\n=======================================");
  Serial.println("📡 SIM800L GSM AT PASSTHROUGH TEST");
  Serial.println("Type 'AT' and press Enter to test link.");
  Serial.println("Type 'AT+CPIN?' to check SIM card status.");
  Serial.println("Type 'AT+CSQ' to check signal strength.");
  Serial.println("=======================================");
}

void loop() {
  if (Serial2.available()) {
    Serial.write(Serial2.read());
  }
  if (Serial.available()) {
    Serial2.write(Serial.read());
  }
}

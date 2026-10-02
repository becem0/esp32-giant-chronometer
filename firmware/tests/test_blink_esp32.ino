/*
 * Test 1: ESP32 Hardware & Serial Monitor Test
 */
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=======================================");
  Serial.println("✅ SUCCESS: ESP32 IS RUNNING PERFECTLY!");
  Serial.println("=======================================");
}

void loop() {
  Serial.println("⏳ ESP32 is running... ready for Chronometer code!");
  delay(2000);
}

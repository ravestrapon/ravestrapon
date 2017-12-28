#define LED_PIN 3
#define MON_PIN_0 0
#define MON_PIN_0_SCALER 0.3590205257  // Pin 0 has a voltage divider
#define MON_PIN_3 3
#define MON_PIN_3_SCALER 0.3518185092  // Pin 1 has a voltage divider

#define ADC_STEPS 1024.0
#define VCC 2.56 

#define DELAY_MS 100

void setup() {
  // Use the internal 1.1V reference instead of Vcc for the
  // ADC readings.  This is more stable and produces better
  // accuracy, although the voltages need to be divided down
  // to under 1.1V now.
  analogReference(INTERNAL); 

  // Turn on serial communication so we can report the readings
  Serial.begin(9600);
}

double GetVoltage(int pin, double scaler) {
  int adc_reading = analogRead(pin);
  return ((double)adc_reading / ADC_STEPS) * VCC / scaler;
}

void loop() {
  Serial.print(GetVoltage(MON_PIN_0, MON_PIN_0_SCALER));
  Serial.print("V\t");
  Serial.print(GetVoltage(MON_PIN_3, MON_PIN_3_SCALER));
  Serial.print("V\t");
  Serial.print("\n");

  delay(DELAY_MS);
}

#define LED_PIN 3
#define MON_PIN_0 0
#define MON_PIN_0_SCALER 1.0  // Pin 0 has no voltage divider
#define MON_PIN_1 1
#define MON_PIN_1_SCALER 2.0  // Pin 1 has a 1/2 voltage divider

#define V_MAX 5.04
#define V_MIN 0.0
#define ADC_STEPS 1024.0

#define DELAY_MS 200

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

double ConvertToVoltage(int adc_reading, double scaler) {
  return scaler * adc_reading * ((V_MAX - V_MIN) / ADC_STEPS) + V_MIN;
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

  Serial.print(ConvertToVoltage(analogRead(MON_PIN_0),
                                MON_PIN_0_SCALER));
  Serial.print("V\t");
  Serial.print(ConvertToVoltage(analogRead(MON_PIN_1),
                                MON_PIN_1_SCALER));
  Serial.print("V\n");

  digitalWrite(LED_PIN, LOW);
  delay(DELAY_MS);
}

#define LED_PIN 3
#define BATTERY_MON_PIN 0

#define V_MAX 5.0
#define V_MIN 0.0
#define ADC_STEPS 1024.0

#define DELAY_MS 500



void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

  int adc_val = analogRead(BATTERY_MON_PIN);
  double v = adc_val * ((V_MAX - V_MIN) / ADC_STEPS) + V_MIN;

  Serial.print(v);
  Serial.print("V\n");

  digitalWrite(LED_PIN, LOW);
  delay(DELAY_MS);
}

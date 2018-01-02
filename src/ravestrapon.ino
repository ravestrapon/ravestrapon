// Definition of interrupt names
// ISR interrupt service routine
#include <avr/interrupt.h>

#include <FastLED.h>

#include "addons/fuelgauge.h"

#include "animations/blockify.h"
#include "animations/drop.h"
#include "animations/fill.h"
#include "animations/pulse.h"
#include "animations/rainbow.h"
#include "animations/static.h"
#include "animations/stripes.h"
#include "animations/tracer.h"

#define UNUSED_ANALOG_INPUT 1
#define POWER_ON_DELAY_MS 1000

#define BOUNCE_TIME_MS 100
#define BRIGHTNESS_BTN_PIN 10
#define FUEL_GAUGE_BTN_PIN 11
#define FUEL_GAUGE_ADC_PIN 4

#define STATUS_LED1_PIN 12
#define STATUS_LED2_PIN 13

#define LED_PIN 0
#define NUM_LEDS 24
#define CHIPSET     WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// These global values keep track of the "brightness" setting controlled by the
// onboard button.
int brightnesses[] = {7, 20, 35, 60, 100};
int num_brightnesses = sizeof(brightnesses) / sizeof(brightnesses[0]);
int brightness = num_brightnesses / 2;
int last_button_position = false;


static unsigned long last_brightness_press_time = 0;
ISR(INT0_vect) {
  // Handing a button press of the brightness button.
  unsigned long now = millis();
  if (now - last_brightness_press_time > BOUNCE_TIME_MS &&
     !digitalRead(BRIGHTNESS_BTN_PIN)) {
    brightness = (brightness + 1) % num_brightnesses;
    FastLED.setBrightness(brightnesses[brightness]);

    // Toggle the status LED to indicate the click was registered
    digitalWrite(STATUS_LED1_PIN, !digitalRead(STATUS_LED1_PIN));
  }
  last_brightness_press_time = now;
}

static unsigned long last_fuel_gauge_press_time = 0;
bool should_read_fuel_gauge = true;
ISR(INT1_vect) {
  // Handing a button press of the fuel gauge status button.
  unsigned long now = millis();
  if (now - last_fuel_gauge_press_time > BOUNCE_TIME_MS &&
     !digitalRead(FUEL_GAUGE_BTN_PIN)) {
    should_read_fuel_gauge = true;
  }
  last_fuel_gauge_press_time = now;
}

void setup() {
  // Read from an unused analog input to get a "random" seed for the rng
  randomSeed(analogRead(UNUSED_ANALOG_INPUT));

  // Brightness button setup  (External interrupt 0 -- INT0)
  pinMode(BRIGHTNESS_BTN_PIN, INPUT_PULLUP);
  GICR &= ~( 1 << INT0); // Disable INT0 during setup
  MCUCR &= ~( 1 << ISC01); // Chaginging edge triggers button's interrupt
  MCUCR |= ( 1 << ISC00); // Chaginging edge triggers button's interrupt
  GIFR |= (1 << INTF0);  // Clear any INT0 flags that may be set
  GICR |= ( 1 << INT0);  // Enable INT2 interrupts now that the setup is done

  // Fuel Gauge button setup  (External interrupt 1 -- INT1)
  pinMode(FUEL_GAUGE_BTN_PIN, INPUT_PULLUP);
  GICR &= ~( 1 << INT1); // Disable INT0 during setup
  MCUCR &= ~( 1 << ISC11); // Chaginging edge triggers button's interrupt
  MCUCR |= ( 1 << ISC10); // Chaginging edge triggers button's interrupt
  GIFR |= (1 << INTF1);  // Clear any INT0 flags that may be set
  GICR |= ( 1 << INT1);  // Enable INT2 interrupts now that the setup is done

  // Setting up the ADC for the fuel gauge
  analogReference(INTERNAL);

  // The board has two pins attached to an LED for displaying status/debugging
  pinMode(STATUS_LED1_PIN, OUTPUT);
  digitalWrite(STATUS_LED1_PIN, HIGH);
  pinMode(STATUS_LED2_PIN, OUTPUT);
  digitalWrite(STATUS_LED2_PIN, LOW);
  
  // Initialize the LED strip
  delay(POWER_ON_DELAY_MS);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(brightnesses[brightness]);
}

typedef void (*Animation)(CRGB*, int);
Animation animations[] = {
  &Stripes::StripesAnimation,
  &Blockify::BlockifyAnimation,
  &Rainbow::RainbowAnimation,
  &Fill::FillAnimation,
  &Static::StaticAnimation,
  &Fill::CenterFillAnimation,
  &Pulse::PulseAnimation,
  &Tracer::TracerAnimation,
  &Drop::DropAnimation,
};
int num_animations = sizeof(animations) / sizeof(animations[0]);

void loop() {
  for (int i = 0; i < num_animations; i++) {
    // Run the animation
    (*animations[i])(leds, NUM_LEDS);

    // Do the fuel gauge check
    if (should_read_fuel_gauge) {
      digitalWrite(STATUS_LED1_PIN, HIGH);
      digitalWrite(STATUS_LED2_PIN, HIGH);
      displayFuelGauge(FUEL_GAUGE_ADC_PIN, leds, NUM_LEDS);
      digitalWrite(STATUS_LED1_PIN, LOW);
      digitalWrite(STATUS_LED2_PIN, LOW);

      should_read_fuel_gauge = false;
    }
  }
}

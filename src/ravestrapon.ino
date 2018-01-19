// Definition of interrupt names
// ISR interrupt service routine
#include <avr/interrupt.h>

#include <FastLED.h>

//#include "animations/fuelgauge.h"

#include "animations/blockify.h"
#include "animations/centerfill.h"
#include "animations/drop.h"
#include "animations/fill.h"
#include "animations/pulse.h"
#include "animations/rainbow.h"
#include "animations/static.h"
#include "animations/stripes.h"
#include "animations/tracer.h"

// Here we define the list of animations that the controller can play
// by building up an enum full of their names and a generator function
// that returns a generic Animation* give the type of animation.
// When adding a new animation, this is where you do the book-keeping.
enum AnimationType {DROP, STRIPES, TRACER, BLOCKIFY, RAINBOW, CENTERFILL, PULSE,
                    FILL, STATIC, NUM_ANIMATION_TYPES};
Animation* buildNewAnimation(AnimationType type);
Animation* current_animation;
Animation* standby_animation;

#define NUM_FRAMES 300

#define UNUSED_ANALOG_INPUT 1
#define POWER_ON_DELAY_MS 1000

#define BOUNCE_TIME_MS 200
#define BRIGHTNESS_BTN_PIN 10
#define FUEL_GAUGE_BTN_PIN 11
#define FUEL_GAUGE_ADC_PIN 4

#define STATUS_LED1_PIN 12
#define STATUS_LED2_PIN 13

#define USB_POWER_DETECTION_PIN 9

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
volatile bool should_read_fuel_gauge = false;
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

  // Configure the USB power detection pin as a digital input
  pinMode(USB_POWER_DETECTION_PIN, INPUT);

  // The board has two pins attached to an LED for displaying status/debugging
  pinMode(STATUS_LED1_PIN, OUTPUT);
  digitalWrite(STATUS_LED1_PIN, HIGH);
  pinMode(STATUS_LED2_PIN, OUTPUT);
  digitalWrite(STATUS_LED2_PIN, LOW);
  
  // Initialize the LED strip
  delay(POWER_ON_DELAY_MS);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(brightnesses[brightness]);

  // Set up the initial Animation to run and other animation-bookkeeping
  current_animation = buildNewAnimation(static_cast<AnimationType>(0));
  standby_animation = NULL;
}

int next_animation_type = 0;
Animation* SwitchToNextAnimation() {
  // First, avoid memory leaks by deleting any animation that's ending
  if (current_animation) {
    delete current_animation;
  }

  // If there is a standby animation waiting to restart, use it
  if (standby_animation) {
    current_animation = standby_animation;
    standby_animation = NULL;
  } else {
  // Otherwise create a new animation object of the next type.
    current_animation = buildNewAnimation(
                            static_cast<AnimationType>(next_animation_type));
    next_animation_type = (next_animation_type + 1) % NUM_ANIMATION_TYPES;
  }
}

Animation* buildNewAnimation(AnimationType type) {
  switch (type) {
    case AnimationType::FILL:
      return new Fill::FillAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::CENTERFILL:
      return new CenterFill::CenterFillAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::STRIPES:
      return new Stripes::StripesAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::STATIC:
      return new Static::StaticAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::BLOCKIFY:
      return new Blockify::BlockifyAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::PULSE:
      return new Pulse::PulseAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::RAINBOW:
      return new Rainbow::RainbowAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::TRACER:
      return new Tracer::TracerAnimation(leds, NUM_LEDS, NUM_FRAMES);
    case AnimationType::DROP:
      return new Drop::DropAnimation(leds, NUM_LEDS, NUM_FRAMES);
    default:
      return NULL;
  }
}

void loop() {
  SwitchToNextAnimation();

  while(current_animation->nextFrame()) {
    FastLED.show();
    FastLED.delay(20);

    if (should_read_fuel_gauge) {
      standby_animation = current_animation;
      current_animation = new Static::StaticAnimation(leds, NUM_LEDS, NUM_FRAMES / 10);
      should_read_fuel_gauge = false;
    }
  }
}


// Do the fuel gauge check
//if (should_read_fuel_gauge) {
//  digitalWrite(STATUS_LED2_PIN, HIGH);
//  displayFuelGauge(FUEL_GAUGE_ADC_PIN, leds, NUM_LEDS);
//  digitalWrite(STATUS_LED2_PIN, LOW);
//  should_read_fuel_gauge = false;
//}

// Temp -- update the usb power status on the LED when this button is pressed
// bool is_usb_connected = digitalRead(USB_POWER_DETECTION_PIN);
// digitalWrite(STATUS_LED2_PIN, is_usb_connected);

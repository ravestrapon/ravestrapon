#include <FastLED.h>

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

#define BRIGHTNESS_STEPS 5
#define MAX_BRIGHTNESS 255
#define BUTTON_PIN 11 

#define LED_PIN 0
#define NUM_LEDS 24
#define CHIPSET     WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// These global values keep track of the "brightness" setting controlled by the
// onboard button.
int brightnesses[] = {7, 20, 35, 60, 80, 120, 255};
int num_brightnesses = sizeof(brightnesses) / sizeof(brightnesses[0]);
int brightness = num_brightnesses / 2;
int last_button_position = false;


void setup() {
  // Read from an unused analog input to get a "random" seed for the rng
  randomSeed(analogRead(UNUSED_ANALOG_INPUT));

  // Set up switch which is used to turn leds all the way off
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize the LED strip
  delay(POWER_ON_DELAY_MS);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(brightnesses[brightness]);
}

typedef void (*Animation)(CRGB*, int);
Animation animations[] = {
  &Drop::DropAnimation,
  &Blockify::BlockifyAnimation,
  &Rainbow::RainbowAnimation,
  &Fill::FillAnimation,
  &Static::StaticAnimation,
  &Fill::CenterFillAnimation,
  &Pulse::PulseAnimation,
  &Tracer::TracerAnimation,
  &Stripes::StripesAnimation,
};
int num_animations = sizeof(animations) / sizeof(animations[0]);

void loop() {
 for (int i = 0; i < num_animations; i++) {
   (*animations[i])(leds, NUM_LEDS);
 }
}

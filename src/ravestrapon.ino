// Definition of interrupt names
// ISR interrupt service routine
#include <avr/interrupt.h>

#include <FastLED.h>

#include "animations/fuelgauge.h"

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
enum AnimationType {DROP, STRIPES, TRACER, BLOCKIFY, RAINBOW, CENTERFILL,
                    PULSE, FILL, STATIC, NUM_ANIMATION_TYPES};
Animation* buildNewAnimation(AnimationType type);
Animation* current_animation;
Animation* standby_animation;

// How many frames each animation gets to run for (duration)
constexpr int kNumFrames = 300;

// An unused (floating) ADC input that we can use to read for randomness
constexpr int kUnusedAnalogInput = 1;

// Delay a brief period before really turning on the LEDs for safety
constexpr int kPowerOnDelayMS = 500;

// Settings for the buttons on the controller
constexpr int kBrightnessBtnPin = 10;
constexpr int kFuelGaugeBtnPin = 11;
constexpr int kBtnBounceTimeMS = 200;

// Which ADC pin is connected to a 1/2 voltage divider from the battery
// for the fuel gauge to read from
constexpr int kFuelGaugeADCPin = 4;

// Which gpios are the two status LEDs connected to
constexpr int kStatusLED1Pin = 12;
constexpr int kStatusLED2Pin = 13;

// Which gpio is connected to the USB power input (with a pulldown) to detect
// whether or not power is currently plugged in
constexpr int kUsbPowerDetectionPin = 9;

// Confiuration for the actual RGB LEDs that this controller is meant to drive.
// These settings are strip-specific and are passed to FastLED which does the
// actual control.
constexpr int kMainLEDPin = 0;
constexpr int kNumLEDs = 24;
CRGB leds[kNumLEDs];


// These global values keep track of the "brightness" setting controlled by the
// onboard button.
int brightnesses[] = {7, 20, 35, 60, 100};
int num_brightnesses = sizeof(brightnesses) / sizeof(brightnesses[0]);
int brightness = num_brightnesses / 2;

// Interrupt handler for the brightness button.
// This works by debouncing itself in SW and updating the brightness
// of the LED strip via FastLEDs setBrightness() routine.
static unsigned long last_brightness_press_time = 0;
ISR(INT0_vect) {
  // Handing a button press of the brightness button.
  unsigned long now = millis();
  if (now - last_brightness_press_time > kBtnBounceTimeMS &&
     !digitalRead(kBrightnessBtnPin)) {
    brightness = (brightness + 1) % num_brightnesses;
    FastLED.setBrightness(brightnesses[brightness]);

    // Toggle the status LED to indicate the click was registered
    digitalWrite(kStatusLED1Pin, !digitalRead(kStatusLED1Pin));
  }
  last_brightness_press_time = now;
}


// Interrupt handler for the Fuel Gauge button.  This works by just
// setting the global should_read_fuel_gauge flag that is then serviced
// by the main thread.
static unsigned long last_fuel_gauge_press_time = 0;
volatile bool should_read_fuel_gauge = false;
ISR(INT1_vect) {
  // Handing a button press of the fuel gauge status button.
  unsigned long now = millis();
  if (now - last_fuel_gauge_press_time > kBtnBounceTimeMS &&
     !digitalRead(kFuelGaugeBtnPin)) {
    should_read_fuel_gauge = true;
  }
  last_fuel_gauge_press_time = now;
}


void setup() {
  // Read from an unused analog input to get a "random" seed for the rng.  This
  // probably doesn't really do much, but it does keep it from *always* starting
  // with the same animation.
  randomSeed(analogRead(kUnusedAnalogInput));

  // Brightness button setup  (External interrupt 0 -- INT0)
  pinMode(kBrightnessBtnPin, INPUT_PULLUP);
  GICR &= ~( 1 << INT0); // Disable INT0 during setup
  MCUCR &= ~( 1 << ISC01); // Chaginging edge triggers button's interrupt
  MCUCR |= ( 1 << ISC00); // Chaginging edge triggers button's interrupt
  GIFR |= (1 << INTF0);  // Clear any INT0 flags that may be set
  GICR |= ( 1 << INT0);  // Enable INT2 interrupts now that the setup is done

  // Fuel Gauge button setup  (External interrupt 1 -- INT1)
  pinMode(kFuelGaugeBtnPin, INPUT_PULLUP);
  GICR &= ~( 1 << INT1); // Disable INT0 during setup
  MCUCR &= ~( 1 << ISC11); // Chaginging edge triggers button's interrupt
  MCUCR |= ( 1 << ISC10); // Chaginging edge triggers button's interrupt
  GIFR |= (1 << INTF1);  // Clear any INT0 flags that may be set
  GICR |= ( 1 << INT1);  // Enable INT2 interrupts now that the setup is done

  // Setting up the ADC for the fuel gauge.  We want to use the more accurate
  // internal 2.56V ADC reference rather than an exteranl reference which is more
  // typical.  A 1/2 voltage divider expands this ADCs sensing range to enclude
  // the battery voltages we'll be reading.
  analogReference(INTERNAL);

  // Configure the USB power detection pin as a digital input.  This is connected
  // to a pull-down resistor on the board, and therefor can be read to see if a
  // USB power cable is currently plugged in.
  pinMode(kUsbPowerDetectionPin, INPUT);

  // The board has two pins attached to an LED for displaying status/debugging.
  // Here they are set up as digital outputs and the initial LED states are set
  pinMode(kStatusLED1Pin, OUTPUT);
  digitalWrite(kStatusLED1Pin, HIGH);
  pinMode(kStatusLED2Pin, OUTPUT);
  digitalWrite(kStatusLED2Pin, LOW);
  
  // Initialize the main RGB LEDs
  delay(kPowerOnDelayMS);
  FastLED.addLeds<WS2811, kMainLEDPin, GRB>(leds, kNumLEDs).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(brightnesses[brightness]);

  // Set up the initial Animation to run
  current_animation = buildNewAnimation(static_cast<AnimationType>(0));
  standby_animation = NULL;
}

int next_animation_type = 0;
void SwitchToNextAnimation() {
  // First, avoid memory leaks by deleting any animation that's ending
  if (current_animation) {
    // Never kill off an animation while the fuel gauge button is held down.
    // This is because presumably the animation that is playing is the fuel
    // gauge, and since the user is still holding down the button they most
    // most likely want it to keep displaying.  As soon as they release the
    // button, this check will stop preventing the animation from moving on.
    if (!digitalRead(kFuelGaugeBtnPin)) {
      return;
    }

    // Otherwise, we'll be stopping the animation and no longer need it.
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
      return new Fill::FillAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::CENTERFILL:
      return new CenterFill::CenterFillAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::STRIPES:
      return new Stripes::StripesAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::STATIC:
      return new Static::StaticAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::BLOCKIFY:
      return new Blockify::BlockifyAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::PULSE:
      return new Pulse::PulseAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::RAINBOW:
      return new Rainbow::RainbowAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::TRACER:
      return new Tracer::TracerAnimation(leds, kNumLEDs, kNumFrames);
    case AnimationType::DROP:
      return new Drop::DropAnimation(leds, kNumLEDs, kNumFrames);
    default:
      return NULL;
  }
}

void loop() {
  // This call should set up whatever is the right thing to be in current_animation
  SwitchToNextAnimation();

  bool has_more_frames = true;
  while(has_more_frames) {
    has_more_frames = current_animation->nextFrame();
    FastLED.show();
    FastLED.delay(20);

    // If the user has pressed the fuel gauge button, we should make a reading and then
    // push a fuel gauge animation into the current position.
    if (should_read_fuel_gauge) {
      // If this is a regular animation, store it's state in standby.  If there
      // is already something on standby, don't clobber it and just drop the
      // current animation entirely since it was an interrupting animation also.
      if (!standby_animation) {
        standby_animation = current_animation;
      } else {
        delete current_animation;
      }
      // Build a new animation to show how much fuel is left in the tank.
      current_animation = FuelGauge::buildFuelGaugeAnimation(kFuelGaugeADCPin, leds,
                                                             kNumLEDs);
      should_read_fuel_gauge = false;
    }
  }
}


// Temp -- update the usb power status on the LED when this button is pressed
// bool is_usb_connected = digitalRead(kUsbPowerDetectionPin);
// digitalWrite(kStatusLED2Pin, is_usb_connected);
